#include <stdio.h>
#include <sys/time.h>

int main()
{
	while (1) {
		write(1, "H", 1);
		usleep(10000);
	}
	return 0;
}
