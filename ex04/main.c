// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/usb.h>
#include <linux/hid.h>

MODULE_LICENSE("GPL");

static int my_usb_probe(struct usb_interface *interface,
			const struct usb_device_id *id)
{
	pr_info("[*] ntombi usb drive plugged\n");
	return 0;
}

static void my_usb_disconnect(struct usb_interface *interface)
{
	pr_info("[*] ntombi usb drive removed\n");
}

static struct usb_device_id my_usb_table[] = {
	{USB_INTERFACE_INFO(USB_INTERFACE_CLASS_HID,
			    USB_INTERFACE_SUBCLASS_BOOT,
			    USB_INTERFACE_PROTOCOL_KEYBOARD)},
	{ },
};

MODULE_DEVICE_TABLE(usb, my_usb_table);

static struct usb_driver my_usb = {
		.name = "ntombi",
		.id_table = my_usb_table,
		.probe = my_usb_probe,
		.disconnect = my_usb_disconnect,
};

int __init hello_init(void)
{
	pr_info("Hello world!\n");
	return usb_register(&my_usb);
}

void __exit hello_exit(void)
{
	pr_info("Cleaning up module.\n");
	usb_deregister(&my_usb);
}

module_init(hello_init);
module_exit(hello_exit);
