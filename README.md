# gsapi_demo
gluster api test tool

for example :
rm glusrw -f | gcc -o glusrw ./glusrw.c -L /usr/lib64/ -lgfapi -lpthread -I /usr/local/include/glusterfs/ -I /usr/include/  -ggdb
rm glusrw -f | gcc -o glusrw ./glusrw.c -L /usr/lib64/ -lgfapi -lpthread -I /usr/local/include/ -I /usr/include/  -ggdb

  ./glusrw --mode w --volname tc1 --hostname 192.168.2.243 --size 102400 --times 10 --concurrency 1 --method 1


>> 单独GDB调试当前活动文件
"label": "gcc build active file"

  Executing task: /usr/bin/gcc -g /home/code/gsapi-demo/gsapi_demo/glusrw.c -o /home/code/gsapi-demo/gsapi_demo/glusrw '-lpthread -lgsapi' '-I /usr/local/include/ -I /usr/include/ -L /usr/lib64/ -L /usr/lib/'

>>远程连接gdb服务模式
192.168.2.243:1233
gdbserver 0.0.0.0:1233 /home/code/gsapi-demo/gsapi_demo/glusrw 

# pro 项目型分支
  git co -b pro
  git push origin HEAD -u
  rm glusrw -f | gcc -o glusrw ./file.c ./glusrw.c  -L /usr/lib64/ -lgfapi -lpthread -ggdb -I include
  