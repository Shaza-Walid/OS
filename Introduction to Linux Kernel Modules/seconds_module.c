#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>

#define PROC_NAME "seconds"   //Define the name of the proc file
static unsigned long start_jiffies;   //Store the jiffies value when the module is loaded


/* This function is called when the proc file is read */
static ssize_t seconds_proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos){

    char buffer[64];                  //Buffer to store the elapsed seconds as a string
    unsigned long elapsed_seconds;    //Store the elapsed time in whole seconds
    int length;                       //Store the number of characters written to the buffer
    elapsed_seconds = (jiffies - start_jiffies) / HZ;       //Calculate the elapsed seconds since the module was loaded
    length = scnprintf(buffer, sizeof(buffer), "%lu\n", elapsed_seconds);   //Convert the elapsed seconds into a string

    //Copy the data from the kernel buffer to the user buffer
    return simple_read_from_buffer(user_buf, count, ppos, buffer, length);
}


/* Define the operations supported by the proc file */
static const struct proc_ops seconds_proc_ops = {
    /* Register the function responsible for reading the file */
    .proc_read = seconds_proc_read,
};


//This function is called when the module is loaded
static int __init seconds_module_init(void){

    start_jiffies = jiffies;   //Save the current jiffies value as the starting point

    //Create a read-only proc file named /proc/seconds
    if (proc_create(PROC_NAME, 0444, NULL, &seconds_proc_ops) == NULL){
        //Print an error message if the proc file creation fails
        printk(KERN_ERR "seconds_module: could not create /proc/%s\n", PROC_NAME);
        return -ENOMEM;   //Return an error code to indicate initialization failure
    }
    //Print the proc file name, starting jiffies, and timer frequency
    printk(KERN_INFO "seconds_module: created /proc/%s; start jiffies=%lu, HZ=%d\n", PROC_NAME, start_jiffies, HZ);

    return 0;
}


//This function is called when the module is removed
static void __exit seconds_module_exit(void){
    remove_proc_entry(PROC_NAME, NULL);   //Remove the proc file created during module initialization
    //Print a message indicating that the proc file has been removed
    printk(KERN_INFO "seconds_module: removed /proc/%s\n", PROC_NAME);
}


module_init(seconds_module_init);     //Register the module initialization function
module_exit(seconds_module_exit);     //Register the module cleanup function


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Reports elapsed seconds through /proc/seconds");
MODULE_AUTHOR("Shaza Waleed");
