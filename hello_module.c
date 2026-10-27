#include <linux/module.h> // Required for all kernel modules
#include <linux/kernel.h> // Required for KERN_INFO

/// @brief This function is called when the module is loaded.
static int __init hello_init(void)
{
    printk(KERN_INFO "Hello, World! Your RHEL kernel module have been loaded\n");
    return 0;
}

/// @brief This function is called when the module is unloaded.
static void __exit hello_exit(void)
{
    printk(KERN_INFO "Goodbye, World! Your RHEL kernel module is going away\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jane Linux Hacker");
MODULE_DESCRIPTION("A simple 'Hello World' kernel module.");
MODULE_VERSION("1.0");

