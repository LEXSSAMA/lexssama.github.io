# ifndef _SEM_H
# define _SEM_H

#include<linux/sched.h>
#define SEM_TABLE_LEN 20
#define SEM_NAME_LEN 10

typedef struct semephore
{
    int value;
    struct task_struct* wait_sem_queue;
    char  name[SEM_NAME_LEN];
}sem_t;
 extern sem_t semtable[SEM_TABLE_LEN];
#endif
