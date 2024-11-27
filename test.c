#include <unistd.h>

int main()
{
	//write hello world to the console
	write(1, "Hello, World!\n", 14);
}