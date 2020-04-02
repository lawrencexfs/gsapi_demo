#include "file.h"

int g_nRule = 0;
int g_nTime = 10;
unsigned int g_nRepeat = 1;

const int WriteMode = 0;
const int ReadMode = 1;
glfs_t    *fsapi = NULL;
int gInd = 0;

int g_nsize = 10240; //默认 文件大小 500 KB 
// static const char *pidname = ".glusrw.pid";
// static const char *volname = "tc1";
// static const char *hostname = "192.168.2.243";
// static const char *wfilename = "/share/";

double diffTime(struct timeval struStop, struct timeval struStart)
{
    return ( (double)((1000000.0*struStop.tv_sec + struStop.tv_usec) - (1000000.0*struStart.tv_sec + struStart.tv_usec)) )/1000000.0;
}

char localhost[NameSize] = {0};
int getLocalHost(){
    FILE *fp;
    char cmd[NameSize] = {0};
    const char *fmt = "hostname";
  
    snprintf(cmd, sizeof(cmd), fmt);
    if((fp = popen(cmd, "r")) == NULL)
    {
      perror("Fail to popen\n");
      return -1;
    }
    while(fgets(localhost, sizeof(localhost), fp) != NULL)
    {
      printf("localhost: %s", localhost);
    }
    localhost[strlen(localhost)-1] = 0;// 删除字符串尾部的换行符
    pclose(fp);
    return 0;
}


/****************************************************************
* 功能: write 文件（使用系统缓存），测试写文件速度
* ************************************************************/
double writeFile(int wsize, unsigned long fid);
double readFile(int wsize, unsigned long fid);

/****************************************************************
* 功能: write 文件（使用系统缓存），测试写文件速度
* ************************************************************/
double writeApi(int wsize, unsigned long fid);
double readApi(int wsize, unsigned long fid);

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
    sprintf(combineName, "%s%s/wmount.%u.%s",wfilename, volname, fid, localhost);
    fprintf(pOut, "writeFile filename: %s. size：%d KB.\n\n", combineName, wsize);
    ulStart = clock();
    gettimeofday(&struStart, NULL);
    fd = open(combineName, O_CREAT | O_RDWR, 0644);

    for (i=0; i < g_nTime; i++)
    {
        write(fd, strData, PageSize);
    }

    close(fd);
    gettimeofday(&struStop, NULL);
    double rate = wsize * g_nTime;
    rate = (rate ) / diffTime(struStop, struStart);
    fprintf(pOut, "writeFile:\t %.3lf (clock time), using mount\n all time diff:   %.3lf\n write rate %.3lf KB/s\n\n", 
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
    sprintf(combineName, "%s%s/wmount.%u.%s",wfilename, volname, fid, localhost);
    fprintf(pOut, "readFile filename: %s. size：%d KB.\n\n", combineName, wsize);
    ulStart = clock();
    gettimeofday(&struStart, NULL);
    fd = open(combineName, O_CREAT | O_RDWR, 0644);
    for (i=0; i < g_nTime; i++)
    {
        int ret = read(fd, strData, PageSize);
        if (ret <= 0){
          g_nTime = i;
          break;          
        }
    }
    close(fd);
    gettimeofday(&struStop, NULL);
    double rate = wsize * g_nTime;
    rate = (rate ) / diffTime(struStop, struStart);
    fprintf(pOut, "readFile:\t %.3lf (clock time), using mount\n all time diff:   %.3lf\n read rate %.3lf KB/s\n\n", 
        (double)(clock()-ulStart)/CLOCKS_PER_SEC,
        diffTime(struStop, struStart), 
        rate);
    return rate;
}

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
    sprintf(combineName, "%s.%u.%s",filename, fid, localhost);
    fprintf(pOut, "writeApi volname: %s%s. size：%d KB.\n\n", volname, combineName, wsize);

    unsigned long  ulStart;
    struct timeval struStart;
    struct timeval struStop;
    ulStart = clock();
    gettimeofday(&struStart, NULL);

    //  /* 初始化gluster环境 */
    // fsapi = glfs_new(volname);
    // if (!fsapi) {
    //     fprintf (pOut, "glfs_new: returned NULL\n");
    //     return ;
    // }
    // ret = glfs_set_volfile_server (fsapi, "tcp", hostname, 24007);
    // assert(ret >= 0); 
    // ret = glfs_set_logging (fsapi, "/dev/stderr", 1);
    // assert(ret >= 0); 
    // ret = glfs_init (fsapi);
    // fprintf (pOut, "glfs_init: returned %d\n", ret);
    // assert(ret >= 0);    
    /* 进行libgfapi函数调用 */
    fd = glfs_creat (fsapi, combineName, O_RDWR, 0644);
    assert(fd != NULL);
    fprintf(pOut, "%s: (%p) %s\n", combineName, fd, strerror (errno));
 
    for (i=0; i < g_nTime; i++)
    {
        ret = glfs_write (fd, writebuf, PageSize, 0);
    }
 
    glfs_close (fd);
    // /* Gluster环境释放 */
    // glfs_fini (fsapi);
    gettimeofday(&struStop, NULL);
    double rate = wsize * g_nTime;
    rate = (rate ) / diffTime(struStop, struStart);
    fprintf(pOut, "writeApi:\t %.3lf (clock time), using gsapi \n all time diff:   %.3lf\n write rate %.3lf KB/s\n\n", 
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
    sprintf(combineName, "%s.%u.%s",filename, fid, localhost);
    printf("readApi volname: %s%s. size：%d KB.\n\n", volname, combineName, wsize);

    unsigned long  ulStart;
    struct timeval struStart;
    struct timeval struStop;
    ulStart = clock();
    gettimeofday(&struStart, NULL);

    fd = glfs_open (fsapi, combineName, O_RDWR);
    // assert(fd != NULL);
    fprintf(pOut, "%s: (%p) %s\n", combineName, fd, strerror (errno));
    glfs_lseek (fd, 0, SEEK_SET);
    for (i=0; i < g_nTime; i++)
    {
        ret = glfs_read (fd, writebuf, PageSize, 0);
        if (ret <= 0){
          g_nTime = i;
          break;          
        }
    }
    glfs_close (fd);

    gettimeofday(&struStop, NULL);
    double rate = wsize * g_nTime;
    rate = (rate ) / diffTime(struStop, struStart);
    fprintf(pOut, "readApi:\t %.3lf (clock time), using gsapi \n all time diff:   %.3lf\n write rate %.3lf KB/s\n\n", 
        (double)(clock()-ulStart)/CLOCKS_PER_SEC,
        diffTime(struStop, struStart), 
        rate);
    return rate;
}


