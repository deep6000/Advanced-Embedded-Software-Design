#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/delay.h>

#define Timeout 500 // in milliseconds


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Deepesh Mahendra");
MODULE_DESCRIPTION("Timer Module logging Name and Counts ");

struct timer_list KtimerMod;
static char* TimerName = "Deepesh";
static unsigned long TimerTimeout = Timeout;
int32_t TimerCounter = 0;

module_param(TimerName,charp,S_IRUGO | S_IWUSR);
module_param(TimerTimeout,ulong, S_IRUGO | S_IWUSR);

void TimerCallback(unsigned long data)
{
  printk(KERN_INFO " Name: %s\tCounts: %d\n",TimerName,++(TimerCounter));
  mod_timer(&KtimerMod,jiffies+msecs_to_jiffies(TimerTimeout));

}

int __init TimerModInit(void)
{
  printk(KERN_INFO "TIMER MODULE INITIALIZING: %s \n Timeout: %ld milliseconds \n",  TimerName,TimerTimeout);
  setup_timer(&KtimerMod,TimerCallback, 0);
  add_timer(&KtimerMod);
  mod_timer(&KtimerMod,jiffies+msecs_to_jiffies(TimerTimeout));
 
  return 0;
}

static void __exit TimerModExit(void)
{
	printk(KERN_INFO "Deleting Timer Module: %s. \n", TimerName); 	
	del_timer(&KtimerMod);
	printk(KERN_INFO "Exiting Timer Module: %s. \n", TimerName); 
}

module_init(TimerModInit);
module_exit(TimerModExit);

