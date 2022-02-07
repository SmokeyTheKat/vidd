#include <vidd/getch.h>

#include <vidd/vidd.h>

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

static struct termios oldt;

uint32_t getch(bool raw)
{
	if (run_buffer.length > 0)
	{
		return run_buffer.data[--run_buffer.length];
	}
	uint64_t retval = 0;
	if (raw)
	{
		uint64_t output = 0;
		read(STDIN_FILENO, &output, 1);
		retval = output;
	}
	else
	{
		uint64_t output = 0;
		read(STDIN_FILENO, &output, 8);
		retval = ((output > 255) * 255) + (output % 255);
	}

	if (macro_recording) buffer_push(&macro_buffer, retval);

	return retval;
}

void getch_test(void)
{
	printf("'q' to exit:\r\n");
	while (1)
	{
		uint32_t key = getch(false);
		if (key == 'q') break;
		printf("%d\r\n", key);
	}
}

void getch_exit(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldt);
}

void getch_init(void)
{
	tcgetattr(STDIN_FILENO, &oldt);
	struct termios newt = oldt;
	newt.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	newt.c_oflag &= ~(OPOST);
	newt.c_cflag |= (CS8);
	newt.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	newt.c_cc[VMIN] = 1;
	newt.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &newt);
}
