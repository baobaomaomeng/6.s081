#include<kernel/types.h>
#include<user/user.h>

#define RD 0
#define WR 1

const uint INT_LEN = sizeof(int);
//1表示从管道中成功读取数据
int get_first(int l_pipe[2],int *first){
    if(read(l_pipe[RD],first,sizeof(int))==sizeof(int)){
        fprintf(1,"prime %d\n",*first);
        return 1;
    }
    close(l_pipe[RD]);
    return 0;
}

void primes(int l_pipe[2]){
    close(l_pipe[WR]);
    int first;//从左管道读出的第一个数字
    int data;//用于保存从管道中读出的数字
    if(get_first(l_pipe,&first)==1){
        int p[2];
        pipe(p);
        //从左管道读取数据，如果无法被整除则传递到下一个递归
        while (read(l_pipe[RD],&data,sizeof(int))==sizeof(int)){
            if(data%first)write(p[WR],&data,sizeof(int));
        }
        close(l_pipe[RD]);
        close(p[WR]);
        if(fork()==0){
            primes(p);
        }else{
            close(p[RD]);
            wait(0);
        }
    }
    return;
}
int main(){
    int p[2];
    pipe(p);
    for(int i=2;i<=35;++i){
        write(p[WR],&i,sizeof(int));
    }
    close(p[WR]);
    if (fork()==0)
        primes(p);
    else{
        wait(0);
    }
    exit(0);
}