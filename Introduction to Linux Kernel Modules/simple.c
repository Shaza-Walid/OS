#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hash.h>
#include <linux/gcd.h>
#include <linux/jiffies.h>
#include <linux/param.h>

static unsigned long loaded_jiffies;   //Store the jiffies value when the module is loaded


//This function is called when the module is loaded
static int __init simple_init(void){

       loaded_jiffies = jiffies;   //Save the current jiffies value at module load time

       printk(KERN_INFO "simple: Loading Kernel Module\n");   //Print a message indicating that the module is loading
       //Print the GOLDEN_RATIO_PRIME constant used in hashing
       printk(KERN_INFO "simple: GOLDEN_RATIO_PRIME = %lu\n",(unsigned long)GOLDEN_RATIO_PRIME);
       printk(KERN_INFO "simple: jiffies at load = %lu\n", jiffies);  //Print the current jiffies value at module load time
       printk(KERN_INFO "simple: HZ = %d\n", HZ);  //Print the number of timer ticks per second

       return 0;
}


//This function is called when the module is removed
static void __exit simple_exit(void){

       unsigned long current_jiffies = jiffies;   //Get the current jiffies value at module unload time
       //Calculate the number of jiffies elapsed since module loading
       unsigned long elapsed_jiffies = current_jiffies - loaded_jiffies;

       //Calculate and print the greatest common divisor of 3300 and 24
       printk(KERN_INFO "simple: GCD(3300, 24) = %lu\n",gcd(3300UL, 24UL));
       //Print the current jiffies value at module unload time
       printk(KERN_INFO "simple: jiffies at unload = %lu\n", current_jiffies);
       printk(KERN_INFO "simple: elapsed jiffies = %lu\n", elapsed_jiffies);      //Print the elapsed time in jiffies
       //Convert the elapsed jiffies into whole seconds and print the result
       printk(KERN_INFO "simple: elapsed seconds = %lu\n", elapsed_jiffies / HZ);
       printk(KERN_INFO "simple: Removing Kernel Module\n");      //Print a message indicating that the module is being removed
}


module_init(simple_init);    //Register the initialization function
module_exit(simple_exit);    //Register the cleanup function


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("introductory kernel module");
MODULE_AUTHOR("Shaza Waleed");
