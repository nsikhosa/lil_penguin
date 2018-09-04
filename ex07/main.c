// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <asm/errno.h>
#include <linux/miscdevice.h>
#include <linux/jiffies.h>
#include <linux/debugfs.h>

MODULE_LICENSE("GPL");

#define NAME "nsikhosa\n"
#define LEN 9

int ret;
struct mutex lock;
char foo_buff[PAGE_SIZE];
static char msg[1024];
static struct dentry *root;
int num_bytes;

static ssize_t dev_id_read(struct file *filep, char *buf, size_t len,
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

static ssize_t dev_id_write(struct file *filep,
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

static ssize_t dev_foo_read(struct file *filep, char *buf, size_t len,
			    loff_t *offset)
{
	int bytes_read = 0;

	if (mutex_lock_interruptible(&lock))
		return -1;
	if (!offset)
		return -EINVAL;
	if (*offset >= num_bytes)
		return 0;
	while ((bytes_read < len) && (*offset < num_bytes)) {
		put_user(msg[*offset], &buf[bytes_read]);
		bytes_read++;
		*offset = *offset + 1;
	}
	mutex_unlock(&lock);
	return bytes_read;
}

static ssize_t dev_foo_write(struct file *filep,
			     const char *buff,
						size_t len, loff_t *offset)
{
	int bytes = 0;
	int append = 0;

	if (mutex_lock_interruptible(&lock))
		return -1;
	if (filep->f_flags & O_APPEND)
		append = strlen(foo_buff);
	if (*offset + append >= PAGE_SIZE)
		ret = -EINVAL;
	while ((bytes < len) && (*offset + append < PAGE_SIZE)) {
		get_user(foo_buff[append + *offset], &buff[bytes]);
		*offset = *offset + 1;
		bytes++;
	}
	mutex_unlock(&lock);
	if (bytes)
		return bytes;
	return ret;
}

static const struct file_operations my_id_fops = {
		.read = dev_id_read,
		.write = dev_id_write,
};

static const struct file_operations my_foo_fops = {
		.read = dev_foo_read,
		.write = dev_foo_write,
};

static int hello_init(void)
{
	pr_info("Hello World\n");
	strncpy(msg, "nsikhosa\n", 9);
	num_bytes = strlen(msg);

	root = debugfs_create_dir("fortytwo", NULL);

	if (!root)
		return -1;
	if (!(debugfs_create_file("id", 0666, root, NULL, &my_id_fops) &&
	    debugfs_create_ulong("jiffies", 0444, root,
			(long unsigned int *)&jiffies) &&
		debugfs_create_file("foo", 0644, root, NULL,
			&my_foo_fops)))
		return -1;
	mutex_init(&lock);
	return 0;
}

static void hello_cleanup(void)
{
	debugfs_remove_recursive(root);
	pr_info("Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);
