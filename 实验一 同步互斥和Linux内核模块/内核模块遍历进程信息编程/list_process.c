#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/sched.h>
#include<linux/list.h>
 
 //加载该内核模块时执行该函数
static __init int init(void)
{
    struct task_struct *task,*p;  //读取进程信息的指针
    struct list_head *pos;      //链表遍历所有进程
    int running=0, interruptible=0, uninterruptible=0, zombie=0, stopped=0, dead=0, traced=0;  
      //用于对各种类型进程计数
    printk("My Kernel Module of Process Begins\n"); //开始内核模块进程便利
    task=&init_task;  //初始化
    list_for_each(pos,&task->tasks) //循环遍历进程
    {
        //pos指针指向类型为struct task_struct的所有进程的列表
        p=list_entry(pos,struct task_struct,tasks); 
        //打印当前进程信息到缓冲区
        printk("name: %s\nprocess id: %d\nprocess state: %ld\nfather name: %s\n",p->comm,p->pid,p->state,p->parent->comm);
        switch(p->state){ //根据进程状态对各类型进程计数
                   case EXIT_ZOMBIE:zombie++;break; //僵尸进程
                   case EXIT_DEAD:dead++;break; //死的进程
                   case TASK_RUNNING:running++;break; //正在运行的进程
                   case TASK_INTERRUPTIBLE:interruptible++;break; //可中断的进程
                   case TASK_UNINTERRUPTIBLE:uninterruptible++;break; //不可中断进程
                   case TASK_STOPPED:stopped++;break; //被停止的进程
                   case TASK_TRACED:traced++;break; //跟踪状态进程
                   default:break; //其他类型
         }
    }
    //所有进程数量打印到缓冲区
    printk("ALL TASK_NUMBER: %d\n",running+interruptible+uninterruptible+stopped+traced+zombie+dead);
    //正在运行的进程数量打印到缓冲区
    printk("TASK_RUNNING: %d\n",running);
    //可中断进程数量打印到缓冲区
    printk("TASK_INTERRUPTIBLE: %d\n",interruptible);
    //不可中断进程数量打印到缓冲区
    printk("TASK_UNINTERRUPTIBLE: %d\n",uninterruptible);
    //被停止的进程数量打印到缓冲区
    printk("TASK_STOPPED: %d\n",stopped);
    //跟踪状态进程数量打印到缓冲区
    printk("TASK_TRACED: %d\n",traced);
    //僵尸进程数量打印到缓冲区
    printk("EXIT_ZOMBIE: %d\n",zombie);
    //死的进程数量打印到缓冲区
    printk("EXIT_DEAD: %d\n",dead);
    //打印内核模块代码运行完毕
    printk("My Kernel Module of Process Ends\n");
    //返回值
    return 0;
}

//卸载该内核模块时进行的函数
static __exit void cleanup(void)
{
    printk("End!\n"); //打印"end!"到缓冲区
}

module_init(init);  //初始化、加载该内核模块时执行init函数
module_exit(cleanup);  //卸载该内核模块时执行cleanup函数
