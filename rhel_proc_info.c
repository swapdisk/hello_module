#include <linux/module.h>     // Required for all kernel modules
#include <linux/kernel.h>     // Required for KERN_INFO
#include <linux/proc_fs.h>    // For /proc filesystem operations
#include <linux/sched.h>      // For current, task_struct (to get process info)
#include <linux/uaccess.h>    // For copy_from_user (if we wanted to read input data)

#define PROCFS_NAME "rhel_proc_info"

static struct proc_dir_entry *rhel_proc_file;

/**
 * @brief This function is called when a user-space process writes to /proc/rhel_proc_info.
 *        Inside this function, 'current' refers to the task_struct of the writing process.
 */
static ssize_t rhel_proc_write(struct file *file, const char __user *buffer,
                               size_t count, loff_t *ppos)
{
    // Get the current process's task_struct.
    // 'current' is a special pointer to the task_struct of the currently executing process.
    struct task_struct *task = current;

    printk(KERN_INFO "rhel_proc_info: Triggered by process '%s' (PID: %d)\n",
           task->comm, task->pid);

    // Return the number of bytes written. This is important for user-space to know the write was successful.
    return count; 
}

// File operations structure for our /proc entry.
// We are primarily interested in the 'write' operation for this demo.
static const struct proc_ops rhel_proc_fops = {
    .proc_write  = rhel_proc_write,
    // For a read operation, you would define .proc_read here.
};

/**
 * @brief This function is called when the module is loaded.
 */
static int __init rhel_proc_info_init(void)
{
    // Create the /proc entry.
    // proc_create(name, mode, parent_dir, file_operations_struct)
    rhel_proc_file = proc_create(PROCFS_NAME, 0666, NULL, &rhel_proc_fops);
    if (!rhel_proc_file) {
        printk(KERN_ALERT "rhel_proc_info: Failed to create /proc/%s entry\n", PROCFS_NAME);
        return -ENOMEM; // Return an error code if creation fails
    }

    printk(KERN_INFO "rhel_proc_info: Module loaded, /proc/%s created (writeable).\n", PROCFS_NAME);
    return 0;
}

/**
 * @brief This function is called when the module is unloaded.
 */
static void __exit rhel_proc_info_exit(void)
{
    // Remove the /proc entry when the module is unloaded.
    remove_proc_entry(PROCFS_NAME, NULL);
    printk(KERN_INFO "rhel_proc_info: Module unloaded, /proc/%s removed.\n", PROCFS_NAME);
}

module_init(rhel_proc_info_init);
module_exit(rhel_proc_info_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joe Procfs Dude");
MODULE_DESCRIPTION("A simple RHEL kernel module to show current process info via /proc write.");
MODULE_VERSION("1.0");
