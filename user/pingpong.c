#include <kernel/types.h>
#include <user/user.h>

#define RD 0
#define WR 1
/*  
    使用两个管道进行父子进程通信，
    需要注意的是如果管道的写端没有close，
    那么管道中数据为空时对管道的读取将会阻塞。
    因此对于不需要的管道描述符，要尽可能早的关闭
*/
int
main(int argc,char *argv[]){
    
    char buf[] = {'X'};
    long len = sizeof(buf);

    int fd_c2p[2];//子进程->父进程
    int fd_p2c[2];//父进程->子进程
    pipe(fd_c2p);
    pipe(fd_p2c);

    int status;
    int pid = fork();
    if(pid==-1){
        //fork失败的话，关闭管道并打印
        fprintf(2, "fork() error!\n");
        close(fd_c2p[RD]);
        close(fd_c2p[WR]);
        close(fd_p2c[RD]);
        close(fd_p2c[WR]);
        exit(1);
    }else if(pid==0){//子进程
        //关闭子进程写端口，关闭父进程读端
        close(fd_p2c[WR]);
        close(fd_c2p[RD]);

        if (read(fd_p2c[RD], buf, len) != len) {//子进程从读入端读取字符串
            fprintf(2, "child read() error!\n");
            status = 1; //标记出错
            exit(1);
        } 

        fprintf(1, "%d: received ping\n", getpid());
        if (write(fd_c2p[WR], buf, len) != len) {
            fprintf(2, "child write() error!\n");
            status = 1;
            exit(1);
        }
        close(fd_p2c[RD]);
        close(fd_c2p[WR]);

        exit(0);
    }
    close(fd_p2c[RD]);
    close(fd_c2p[WR]);
    if (write(fd_p2c[WR],buf,len)!=len){
        fprintf(2,"parent write() error!\n");
        exit(1);
    }
    if(read(fd_c2p[RD],buf,len)!=len){
        fprintf(2,"parent read() error!\n");
        exit(1);
    }
    if (status==0)fprintf(1,"%d: received pong\n",getpid());
    exit(0);
}