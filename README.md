# gsapi_demo
gluster api test tool

for example :
rm glusrw -f | gcc -o glusrw ./glusrw.c -L /usr/lib64/ -lgfapi -lpthread -I /usr/local/include/glusterfs/ -I /usr/include/  -ggdb

  ./glusrw --mode w --volname tc1 --hostname 192.168.2.243 --size 102400 --times 10 --concurrency 1 --method 1
