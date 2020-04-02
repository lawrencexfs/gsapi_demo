#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include "glusterfs/api/glfs.h"

#define pOut stderr

#define  NameSize (1024)
char localhost[NameSize];

int g_nRule;
int g_nTime;
unsigned int g_nRepeat;

const int WriteMode;
const int ReadMode;
glfs_t    *fsapi;
int gInd;

int g_nsize; //默认 文件大小 500 KB 
static const char *pidname = ".glusrw.pid";
static const char *volname = "tc1";
static const char *hostname = "192.168.2.243";
static const char *wfilename = "/share/";

enum Method{
   MAll = 0,
   MFile = 1,
   MApi = 2
};

struct StatData
{
  /* data */
  int flag;
  pthread_t ind;
  double file_rate;
  double api_rate;
};


/****************************************************************
* 功能: diffTime 返回两者时间戳之差 单位 us 
* ************************************************************/
double diffTime(struct timeval struStop, struct timeval struStart);

#define NUMT 4
int getLocalHost();
