#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>

#define PROC_NAME "jiffies"     //Define the name of the proc file


//This function is called when the proc file is read
static ssize_t jiffies_proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos){

    char buffer[64];   //Buffer to store the jiffies value as a string
    int length;        //Store the number of characters written to the buffer

    //Convert the current jiffies value to a string
    length = scnprintf(buffer, sizeof(buffer), "%lu\n", jiffies);
    //Copy the data from the kernel buffer to the user buffer
    return simple_read_from_buffer(user_buf, count, ppos, buffer, length);
}


//Define the operations supported by the proc file
static const struct proc_ops jiffies_proc_ops = {
    //Register the function responsible for reading the file
    .proc_read = jiffies_proc_read,   
};


//This function is called when the module is loaded
static int __init jiffies_module_init(void){
    //Create a read-only proc file named /proc/jiffies
    if (proc_create(PROC_NAME, 0444, NULL, &jiffies_proc_ops) == NULL){

        //Print an error message if the proc file creation fails
        printk(KERN_ERR "jiffies_module: could not create /proc/%s\n", PROC_NAME);
        return -ENOMEM;    //Return an error code to indicate initialization failure
    }
    //Print a message indicating successful proc file creation
    printk(KERN_INFO "jiffies_module: created /proc/%s\n", PROC_NAME);

    return 0;
}


/* This function is called when the module is removed */
static void __exit jiffies_module_exit(void){

    remove_proc_entry(PROC_NAME, NULL);   //Remove the proc file created during module initialization
    //Print a message indicating that the proc file has been removed 
    printk(KERN_INFO "jiffies_module: removed /proc/%s\n", PROC_NAME);
}


module_init(jiffies_module_init);   //Register the module initialization function
module_exit(jiffies_module_exit);   //Register the module cleanup function


MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Reports current jiffies through /proc/jiffies");
MODULE_AUTHOR("Shaza Waleed");
