// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

static int hello_init(void)
{
	pr_info("Hello world !\n");
	return 0;
}

static void hello_cleanup(void)
{
	pr_info("Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
