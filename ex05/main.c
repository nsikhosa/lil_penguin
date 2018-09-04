// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/errno.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");

#define NAME "nsikhosa\n"
#define LEN 9

static char msg[1024];
int num_bytes;

static ssize_t dev_read(struct file *filep, char *buf, size_t len,
			loff_t *offset)
{
	int bytes_read = 0;

	if (!offset)
		return -EINVAL;
	if (*offset >= num_bytes)
		return 0;
	while ((bytes_read < len) && (*offset < num_bytes)) {
		put_user(msg[*offset], &buf[bytes_read]);
		bytes_read++;
		*offset = *offset + 1;
	}
	return bytes_read;
}

static ssize_t dev_write(struct file *filep,
			 const char *buff,
						size_t len, loff_t *offset)
{
	ssize_t result;
	ssize_t check;
	char buffer[LEN];

	if (len != LEN) {
		result = -EINVAL;
		return result;
	}
	check = copy_from_user(buffer, buff, LEN);
	if (check != 0) {
		result = -EINVAL;
		return result;
	}
	if (strncmp(buffer, NAME, LEN) == 0)
		result = LEN;
	else
		result = -EINVAL;
	return result;
}

static const struct file_operations my_fops = {
	.read = dev_read,
	.write = dev_write,
};

static struct miscdevice my_dev = {
	.name = "fortytwo",
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &my_fops,
};

static int hello_init(void)
{
	pr_info("Hello World\n");
	strncpy(msg, "nsikhosa\n", 9);
	num_bytes = strlen(msg);
	return misc_register(&my_dev);
}

static void hello_cleanup(void)
{
	pr_info("Cleaning up module.\n");
	misc_deregister(&my_dev);
}

module_init(hello_init);
module_exit(hello_cleanup);
