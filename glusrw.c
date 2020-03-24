/****************************************************************
***  gluster api 读写性能对比测试  ****
*************************************************************/
#include <stdio.h>
#include <time.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/time.h>

#include <errno.h>
#include "api/glfs.h"
#include "api/glfs-handles.h"
#include <pthread.h>
// using namespace std;

#define WRITE_FILE

#define  NameSize (1024)
int nTime = 5;
// #define  PageSize (1024*1024)            /*4K*/
// int nPageSize = 1024*1024;
// #define  COUNT  ((1024*1024)/PageSize)         /* 1MB = COUNT * PageSize换算 */
// int nCOUNT = ((1024*1024)/nPageSize);   
// #define  STR_LEN  ((PageSize*1024) +1)      /*每次缓存4M有效数据写入文件 ，最后一个字节保证 null-terminated ，不写入文件*/

// char g_strUserFlow[PageSize + 1] ;  /*+1 保证字符 '\0'中止


/****************************************************************
* 功能: diffTime 返回两者时间戳之差 单位 us 
* ************************************************************/
double diffTime(struct timeval struStop, struct timeval struStart)
{
    return ( (double)((1000000.0*struStop.tv_sec + struStop.tv_usec) - (1000000.0*struStart.tv_sec + struStart.tv_usec)) )/1000000.0;
}

/****************************************************************
* 功能: write 文件（使用系统缓存），测试写文件速度
* ************************************************************/
double writeFile(int wsize, unsigned long fid);

/****************************************************************
* 功能: write 文件（使用系统缓存），测试写文件速度
* ************************************************************/
double writeApi(int wsize, unsigned long fid);

int size = 500; //默认 文件大小 500 KB 
static const char *pidname = ".glusrw.pid";
static const char *volname = "test";
static const char *hostname = "192.168.11.6";
static const char *wfilename = "/share/";

double writeFile(int wsize, unsigned long fid)
{  

    int PageSize = ((wsize * 1024));   
    int i = 0;
    int fd;
    unsigned long  ulStart;
    struct timeval struStart;
    struct timeval struStop;
    char *strData = (char *)malloc(PageSize+1);
    char combineName[NameSize] = {0};
    sprintf(combineName, "%s%s/wmount.%u",wfilename, volname, fid);
    printf("writeFile filename: %s. size：%d KB.\n\n", combineName, wsize);
    ulStart = clock();
    gettimeofday(&struStart, NULL);
    fd = open(combineName, O_CREAT | O_RDWR, 0644);

    for (i=0; i < nTime; i++)
    {
        write(fd, strData, PageSize);
    }

    close(fd);
    gettimeofday(&struStop, NULL);
    double rate = wsize * nTime;
    rate = (rate ) / diffTime(struStop, struStart);
    printf("writeFile:\t %.3lf (clock time), using mount\n all time diff:   %.3lf\n write rate %.3lf KB/s\n\n", 
        (double)(clock()-ulStart)/CLOCKS_PER_SEC,
        diffTime(struStop, struStart), 
        rate);
    return rate;
}

double readFile(int wsize, unsigned long fid)
{  
    int PageSize = ((wsize * 1024));   
    int i = 0;
    int fd;
    unsigned long  ulStart;
    struct timeval struStart;
    struct timeval struStop;
    char *strData = (char *)malloc(PageSize+1);
    char combineName[NameSize] = {0};
    sprintf(combineName, "%s%s/wmount.%u",wfilename, volname, fid);
    printf("readFile filename: %s. size：%d KB.\n\n", combineName, wsize);
    ulStart = clock();
    gettimeofday(&struStart, NULL);
    fd = open(combineName, O_CREAT | O_RDWR, 0644);

    for (i=0; i < nTime; i++)
    {
        int ret = read(fd, strData, PageSize);
        if (ret <= 0){
          nTime = i;
          break;          
        }
    }

    close(fd);
    gettimeofday(&struStop, NULL);
    double rate = wsize * nTime;
    rate = (rate ) / diffTime(struStop, struStart);
    printf("readFile:\t %.3lf (clock time), using mount\n all time diff:   %.3lf\n read rate %.3lf KB/s\n\n", 
        (double)(clock()-ulStart)/CLOCKS_PER_SEC,
        diffTime(struStop, struStart), 
        rate);
    return rate;
}

const int WriteMode = 0;
const int ReadMode = 1;
glfs_t    *fsapi = NULL;

double writeApi(int wsize, unsigned long fid)
{  
    int PageSize = ((wsize * 1024));  
    int i = 0;
    // glfs_t    *fsapi = NULL;
    int        ret = 0;
    glfs_fd_t *fd = NULL;
    
    const char      *filename = "/wapi";
    char *writebuf = (char *)malloc(PageSize+1);
    char combineName[NameSize] = {0};
    sprintf(combineName, "%s.%u",filename, fid);
    printf("writeApi volname: %s%s. size：%d KB.\n\n", volname, combineName, wsize);

    unsigned long  ulStart;
    struct timeval struStart;
    struct timeval struStop;
    ulStart = clock();
    gettimeofday(&struStart, NULL);

    //  /* 初始化gluster环境 */
    // fsapi = glfs_new(volname);
    // if (!fsapi) {
    //     fprintf (stderr, "glfs_new: returned NULL\n");
    //     return ;
    // }
    // ret = glfs_set_volfile_server (fsapi, "tcp", hostname, 24007);
    // assert(ret >= 0); 
    // ret = glfs_set_logging (fsapi, "/dev/stderr", 1);
    // assert(ret >= 0); 
    // ret = glfs_init (fsapi);
    // fprintf (stderr, "glfs_init: returned %d\n", ret);
    // assert(ret >= 0);    
    /* 进行libgfapi函数调用 */
    fd = glfs_creat (fsapi, combineName, O_RDWR, 0644);
    assert(fd != NULL);
    fprintf (stderr, "%s: (%p) %s\n", combineName, fd, strerror (errno));
 
    // fd2 = glfs_open (fsapi, combineName, O_RDWR);
    // fprintf (stderr, "%s: (%p) %s\n", combineName, fd, strerror (errno));

    for (i=0; i < nTime; i++)
    {
        ret = glfs_write (fd, writebuf, PageSize, 0);
    }

    // glfs_lseek (fd, 0, SEEK_SET);
    // ret = glfs_read (fd, readbuf, 32*1024, 0);
    // printf ("read %d, %s", ret, readbuf);
 
    glfs_close (fd);
    // glfs_close (fd2);
 
    // /* Gluster环境释放 */
    // glfs_fini (fsapi);
    gettimeofday(&struStop, NULL);
    double rate = wsize * nTime;
    rate = (rate ) / diffTime(struStop, struStart);
    printf("writeApi:\t %.3lf (clock time), using gsapi \n all time diff:   %.3lf\n write rate %.3lf KB/s\n\n", 
        (double)(clock()-ulStart)/CLOCKS_PER_SEC,
        diffTime(struStop, struStart), 
        rate);
    return rate;
}

double readApi(int wsize, unsigned long fid)
{  
    int PageSize = ((wsize * 1024));  
    int i = 0;
    int        ret = 0;
    glfs_fd_t *fd = NULL;
    
    const char      *filename = "/wapi";
    char *writebuf = (char *)malloc(PageSize+1);
    char combineName[NameSize] = {0};
    sprintf(combineName, "%s.%u",filename, fid);
    printf("writeApi volname: %s%s. size：%d KB.\n\n", volname, combineName, wsize);

    unsigned long  ulStart;
    struct timeval struStart;
    struct timeval struStop;
    ulStart = clock();
    gettimeofday(&struStart, NULL);

    fd = glfs_open (fsapi, combineName, O_RDWR);
    // assert(fd != NULL);
    fprintf (stderr, "%s: (%p) %s\n", combineName, fd, strerror (errno));
    glfs_lseek (fd, 0, SEEK_SET);
 
    for (i=0; i < nTime; i++)
    {
        ret = glfs_read (fd, writebuf, PageSize, 0);
        if (ret <= 0){
          nTime = i;
          break;          
        }
    }
    glfs_close (fd);

    gettimeofday(&struStop, NULL);
    double rate = wsize * nTime;
    rate = (rate ) / diffTime(struStop, struStart);
    printf("writeApi:\t %.3lf (clock time), using gsapi \n all time diff:   %.3lf\n write rate %.3lf KB/s\n\n", 
        (double)(clock()-ulStart)/CLOCKS_PER_SEC,
        diffTime(struStop, struStart), 
        rate);
    return rate;
}

enum Method{
   MAll = 0,
   MFile = 1,
   MApi = 2
};

struct wdata
{
  /* data */
  int flag;
  pthread_t ind;
  double file_rate;
  double api_rate;
};
struct wdata stat_out;

pthread_mutex_t smutex = PTHREAD_MUTEX_INITIALIZER; 
static void *writeTest(void *methon)
{
  struct wdata *ps = ((struct wdata *)&stat_out);
  int flag = ((struct wdata *)methon)->flag;
  double rate = 0;
  sleep(1);
  pthread_t pid = pthread_self();
  // pthread_detach(pid);
  pid = ((struct wdata *)methon)->ind;
  // printf("local pid: %u\n", pid);
  
  if (flag == MFile) {
    printf("writeFile method: %d ... local pid: %u\n", flag, pid);
    rate = writeFile(size, pid);
    pthread_mutex_lock(&smutex);  
    ps->file_rate += rate;
    pthread_mutex_unlock(&smutex);  
  }else if (flag == MApi) {
    printf("writeApi method: %d ... local pid: %u\n", flag, pid);
    rate = writeApi(size, pid);
    pthread_mutex_lock(&smutex);  
    ps->api_rate += rate;
    pthread_mutex_unlock(&smutex);  
  }else {
    printf("all in comparison . method: %d ... local pid: %u\n", flag, pid);
    rate = writeFile(size, pid);
    pthread_mutex_lock(&smutex);  
    ps->file_rate += rate;
    pthread_mutex_unlock(&smutex); 
    rate = writeApi(size, pid);
    pthread_mutex_lock(&smutex);  
    ps->api_rate += rate;
    pthread_mutex_unlock(&smutex);  
  }
  return ps;
}


static void *readTest(void *methon)
{
  struct wdata *ps = ((struct wdata *)&stat_out);
  int flag = ((struct wdata *)methon)->flag;
  double rate = 0;
  sleep(1);
  pthread_t pid = pthread_self();
  // pthread_detach(pid);
  pid = ((struct wdata *)methon)->ind;
  // printf("local pid: %u\n", pid);
  
  if (flag == MFile) {
    printf("readFile method: %d ... local pid: %u\n", flag, pid);
    rate = readFile(size, pid);
    pthread_mutex_lock(&smutex);  
    ps->file_rate += rate;
    pthread_mutex_unlock(&smutex);  
  }else if (flag == MApi) {
    printf("readApi method: %d ... local pid: %u\n", flag, pid);
    rate = readApi(size, pid);
    pthread_mutex_lock(&smutex);  
    ps->api_rate += rate;
    pthread_mutex_unlock(&smutex);  
  }else {
    printf("all in comparison . method: %d ... local pid: %u\n", flag, pid);
    rate = readFile(size, pid);
    pthread_mutex_lock(&smutex);  
    ps->file_rate += rate;
    pthread_mutex_unlock(&smutex); 
    rate = readApi(size, pid);
    pthread_mutex_lock(&smutex);  
    ps->api_rate += rate;
    pthread_mutex_unlock(&smutex);  
  }
  return ps;
}

#define NUMT 4

int main(int argc, char **argv)
{
  int curMethon;

  ssize_t n = 0;
  int arg = 1;

  int flag = 0;
  int i = 0;
  int ret;
  int error;
  long pid;
  int result = 0;
  
  int mutinum = 1;

  int mode = WriteMode; // 读写模式 默认 0 写， 非0 读

//   memset(&test, 0, sizeof(test));

  while(argc > arg) {
    if(!strcmp("--mode", argv[arg])) {
      arg++;
      printf("write or read mode： %s\n", argv[arg]);
      if(argc > arg && !strcmp("r", argv[arg++]))
        mode = ReadMode; //写模式
    }
    else if(!strcmp("--volname", argv[arg])) {
      arg++;
      if(argc>arg)
        volname = argv[arg++];
      printf("volname: %s\n", volname);
    }
    else if(!strcmp("--hostname", argv[arg])) {
      arg++;
      if(argc>arg)
        hostname = argv[arg++];
      printf("hostname: %s\n", hostname);
    }
    else if(!strcmp("--size", argv[arg])) {
      arg++;
      if(argc>arg)
        size = atoi(argv[arg++]);
      printf("size: %d\n", size);
    }
    else if(!strcmp("--times", argv[arg])) {
      arg++;
      if(argc>arg)
        nTime = atoi(argv[arg++]);
      printf("times: %d\n", nTime);
    }
    else if(!strcmp("--concurrency", argv[arg])) {
      arg++;
      if(argc>arg)
        mutinum = atoi(argv[arg++]);
      printf("concurrency: %d\n", mutinum);
    }
    else if(!strcmp("--method", argv[arg])) {
      arg++;
      if(argc>arg)
        flag = atoi(argv[arg++]);
      curMethon = (flag);
      stat_out.flag = flag;
      printf("method: %d\n", curMethon);
    }
    else {
      puts("Usage: glusrw [option]\n"
           " --mode [r or w]\n"
           " --volname [volname](test1)\n"
           " --hostname [hostname](192.168.2.243)\n"
           " --size [size](unit KB)\n"
           " --times [times](call write times)"
           " --concurrency [concurrency](muti limit 50)\n"
           " --method [method](0 all 1 mount 2 api )\n"
           " for examples...  ./glusrw --mode w --volname test1 --hostname node1 --size 200 --times 10 --concurrency 1 --method 0");
      return 0;
    }
  }

  pid = (long)getpid();
  printf("current program pid: %d\n", pid);

  pthread_t tid[mutinum];
    /* 初始化gluster环境 */
  fsapi = glfs_new(volname);
  if (!fsapi) {
      fprintf (stderr, "glfs_new: returned NULL\n");
      return ;
  }
  ret = glfs_set_volfile_server (fsapi, "tcp", hostname, 24007);
  assert(ret >= 0); 
  ret = glfs_set_logging (fsapi, "/dev/stderr", 1);
  assert(ret >= 0); 
  ret = glfs_init (fsapi);
  fprintf (stderr, "glfs_init: returned %d\n", ret);
  assert(ret >= 0);    
  stat_out.api_rate = 0;
  stat_out.file_rate = 0;
  void *(*pcall) (void *) ;
  if (mode == WriteMode){
    pcall = writeTest;
  }else {
    pcall = readTest;
  }

  struct wdata local[mutinum];
    
  for(i = 0; i< mutinum; i++) {
    local[i].flag = flag;   
    local[i].ind = i;
    int error = pthread_create(&tid[i],
                               NULL, /* default attributes please */
                               pcall,
                               (void *)(&local[i]));
    if(0 != error)
      fprintf(stderr, "Couldn't run thread number %d, errno %d\n", local[i].ind, error);
    else
      fprintf(stderr, "Thread %d, %ul\n", i, tid[i]);
  }

  /* now wait for all threads to terminate */
  for(i = 0; i< mutinum; i++) {
    pthread_join(tid[i], NULL);
    fprintf(stderr, "Thread %d terminated\n", i);
  }
  printf("total rate file： %.3lf MB/s\n", stat_out.file_rate/1024);
  printf("total rate gapi： %.3lf MB/s\n", stat_out.api_rate/1024);
  glfs_fini (fsapi);
  printf("total buffer size： %d MB\n", size*nTime/1024);

  return result;
}
