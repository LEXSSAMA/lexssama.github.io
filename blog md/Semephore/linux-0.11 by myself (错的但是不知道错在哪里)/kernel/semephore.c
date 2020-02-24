 
/*The file.c compete three function of system call about semephore, respectively are sys_sem_open( ),
sys_sem_wait( ),sys_sem_post( ) and sys_sem_unlink */

# include<linux/sem.h>
# include<asm/segment.h>
#include<asm/system.h>
//# include<string.h>
# include<linux/kernel.h>
#include<linux/sched.h>

sem_t semtable[SEM_TABLE_LEN];
int used_sem=0; /*已经用了的semtable中的位置*/
sem_t* sys_sem_open(const char* name,unsigned int value){
    int name_len=0,i=0,exist=0;
    sem_t* s=NULL;
    printk("%s,%d\n",name,name_len);
    name_len = strlen(name);
    if(name_len>SEM_NAME_LEN)
    return  NULL;
    for(i=0;i<used_sem;++i)
    {
        if(strlen(semtable[i].name)!=name_len)
            continue;
        if(strcmp(semtable[i].name,name)==0)
        {
            exist =1;
            semtable[i].value=value;
             printk("%s,semtable[%d] : %d\n",semtable[i].name,i,semtable[i].value);
            s=(sem_t*)&semtable[i];
    }
    }
    if(exist)
    return s;
    else
    {
                semtable[used_sem].value =value;
                printk("%s,semtable[%d] : %d\n",semtable[used_sem].name,used_sem,semtable[used_sem].value);
                strcpy(semtable[used_sem].name,name);
                semtable[used_sem].wait_sem_queue=NULL;
                s=(sem_t*)&semtable[used_sem];
                used_sem++;
        return s;
    }
}

int sys_sem_wait(sem_t * sem)
{
    printk("come into sem_wait()\n");
    if(!sem) {
        printk("sys_sem_wait function parameter pointer is NULL\n\r");
        return -1;
    }
    cli();
while(sem->value<=0)    {//这里value--放在后面因为不需要value来记录等待进程数了
    sleep_on(&(sem->wait_sem_queue));
    }// 因为linux-0.11的sleep_on隐式的链接了等待的进程。
    sem->value--;
    sti();
    return 0;
}

int sys_sem_post(sem_t* sem)
{
    if(!sem){
        printk("sys_sem_post function parameter pointeris NULL\n\r");
        return -1;
    }
    cli();
    sem->value++;
    if(sem->value>0&&sem->wait_sem_queue!=NULL)
    wake_up(&(sem->wait_sem_queue));
    sti();
    return 0;
}

int sys_sem_unlink (const char *name)
{
    int name_len=0, i=0;
    name_len=strlen(name);
    if(name_len>SEM_NAME_LEN)
    {
        printk("sem_ulink (): parameter name over SEM_NAME_LEN");
        return -1;
    }
    for( ;i<SEM_TABLE_LEN;++i)
    {
        if(strlen(semtable[i].name)==name_len)
            continue;
            if(strcmp(name,semtable[i].name)==0)
            {
                used_sem--;
                return 0;
            }
    }
    return -1;
}
