#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void)
{
	char buff[9];
	int fd = open("/sys/kernel/debug/fortytwo/id", O_RDWR);

	read(fd, buff, 9);
	write(fd, "nsikhosa\n", 9);
	printf("%s", buff);
	return (0);
}
