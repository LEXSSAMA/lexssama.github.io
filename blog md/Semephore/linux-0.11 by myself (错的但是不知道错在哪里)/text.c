#define  __LIBRARY__
#include<unistd.h>
#include<stdio.h>
#include<linux/sem.h>
#include<fcntl.h>

_syscall2(int,sem_open,const char *,name,unsigned int ,value)
_syscall1(int,sem_wait,sem_t*,sem)
_syscall1(int,sem_post,sem_t*,sem)
_syscall1(int,sem_unlink,const char*,name)

#define BUFFER_SIZE 10
#define PRODUCE 60
#define CUSTOMER 20

int P_C(int fd)
{
    int tmp,tmp1,tmp2,customer,customer1,customer2,produce;
    int data[10];
    sem_t* p_empty_buf; /*空闲资源的个数*/
    sem_t* p_full_buf;  /*生产者生产出来的个数（消费者可消费资源）*/
    sem_t* mutex;/*互斥信号量*/
   p_empty_buf=(sem_t*)sem_open("Empty",BUFFER_SIZE);
   p_full_buf=(sem_t*)sem_open("Full",0);
   mutex = (sem_t*)sem_open("Mutex",1);
    tmp = fork();
    if(tmp==0){
        /*son1*/
        int cur,i;
        customer = getpid();
        for(i =0;i<CUSTOMER;++i){
        sem_wait(p_full_buf);/*检查生产者生产的资源数如果大于0则跳出使用，否则睡眠*/
        sem_wait(mutex);
        cur = lseek(fd,0,SEEK_CUR);/*取共享缓冲区中的文件指针位置*/
        lseek(fd,0,SEEK_SET);/*让文件指针回到开头位置*/
        read(fd,(char*)data,sizeof(int)*BUFFER_SIZE); /*把共享缓冲区中的BUFFER_SIZE个数全部写入data中*/
        printf("%d : %d\n",data[0],customer);
        fflush(stdout);/*冲刷stdout流*/
        lseek(fd,0,SEEK_SET);/*把文件指针回到文件开始处*/
        write(fd,(char*)&data[1],sizeof(int)*(BUFFER_SIZE-1)); /*把data[1]开始的后面(BUFFER_SIZE)个数全部写入共享缓冲区中*/
        lseek(fd,sizeof(int)*(cur-1),SEEK_SET);/*因为取出了一个数data[0]所有修改回文件指针的时候要减一*/
        sem_post(mutex); 
        sem_post(p_empty_buf); /*空闲资源加1*/
        }
    }
    else
    {
     tmp1 = fork();
    if(tmp1==0)
    {
        /*son1*/
        int cur,i;
        customer1 = getpid();
        for(i =0;i<CUSTOMER;++i){
        sem_wait(p_full_buf);/*检查生产者生产的资源数如果大于0则跳出使用，否则睡眠*/
        sem_wait(mutex);
        cur = lseek(fd,0,SEEK_CUR);/*取共享缓冲区中的文件指针位置*/
        lseek(fd,0,SEEK_SET);/*让文件指针回到开头位置*/
        read(fd,(char*)data,sizeof(int)*BUFFER_SIZE); /*把共享缓冲区中的BUFFER_SIZE个数全部写入data中*/
        printf("%d : %d\n",data[0],customer1);
        fflush(stdout);/*冲刷stdout流*/
        lseek(fd,0,SEEK_SET);/*把文件指针回到文件开始处*/
        write(fd,(char*)&data[1],sizeof(int)*(BUFFER_SIZE-1)); /*把data[1]开始的后面(BUFFER_SIZE)个数全部写入共享缓冲区中*/
        lseek(fd,sizeof(int)*(cur-1),SEEK_SET);/*因为取出了一个数data[0]所有修改回文件指针的时候要减一*/
        sem_post(mutex); 
        sem_post(p_empty_buf); /*空闲资源加1*/
        }
    }
    else
    {
     tmp2 = fork();
    if(tmp2==0){
        /*son1*/
        int cur,i;
        customer2 = getpid();
        for(i =0;i<CUSTOMER;++i){
        sem_wait(p_full_buf);/*检查生产者生产的资源数如果大于0则跳出使用，否则睡眠*/
        sem_wait(mutex);
        cur = lseek(fd,0,SEEK_CUR);/*取共享缓冲区中的文件指针位置*/
        lseek(fd,0,SEEK_SET);/*让文件指针回到开头位置*/
        read(fd,(char*)data,sizeof(int)*BUFFER_SIZE); /*把共享缓冲区中的BUFFER_SIZE个数全部写入data中*/
        printf("%d : %d\n",data[0],customer2);
        fflush(stdout);/*冲刷stdout流*/
        lseek(fd,0,SEEK_SET);/*把文件指针回到文件开始处*/
        write(fd,(char*)&data[1],sizeof(int)*(BUFFER_SIZE-1)); /*把data[1]开始的后面(BUFFER_SIZE)个数全部写入共享缓冲区中*/
        lseek(fd,sizeof(int)*(cur-1),SEEK_SET);/*因为取出了一个数data[0]所有修改回文件指针的时候要减一*/
        sem_post(mutex); 
        sem_post(p_empty_buf); /*空闲资源加1*/
        }
    }
    else
    {
        int i;
        produce = getpid();
        for(i =0;i<PRODUCE;++i)
        {
            printf("OK!!!!!!");
            sem_wait(p_empty_buf);  /*查看有没有空闲资源位置,有就跳出来添加一个资源，没有就睡眠*/
            sem_wait(mutex);
             write(fd,(char*)&i,sizeof(int));
             sem_wait(mutex);
             sem_post(p_full_buf);/*增加一个消费者可消费资源*/
        }
        wait((int*)NULL);
       wait((int*)NULL);
       wait((int*)NULL);
       close(fd);
       sem_unlink("Empty");
       sem_unlink("Full");
       sem_unlink("Mutex");
    }
    }
    }
    return 0;
}

int main(void)
{
    int fd;
   if((fd = open("P_C_problem_buffer",O_RDWR | O_CREAT| O_TRUNC,0644 ))<0)
   {
       printf("open error \n");
       return -1;
   }
   if(P_C(fd)==0)
   {
       printf("experiment success!");
       return 0;
   }
   return -1;
}