#include <vidd/vidd.h>

#include <vidd/getch.h>
#include <vidd/commands.h>
#include <vidd/display.h>

#include <stdlib.h>
#include <locale.h>
#include <signal.h>

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "en_US.utf8");
	char* file_name = (argc > 1) ? (argv[1]) : ("_-=[NONE]=-_");
	//signal(SIGINT, signal_catch);
	screen_save();
	setbuf(stdout, NULL);

	client_pool = make_vidd_client_pool(20);

	intmax_t width, height;
	screen_get_size(&width, &height);
	struct vidd_client initial_client = make_vidd_client(file_name, 0, 0, width, height, 0);

	vidd_client_pool_add(&client_pool, initial_client);

	if (!strcmp(file_name, "-"))
		vidd_load_stdin(vidd_get_active());
	else vidd_load_file(vidd_get_active(), file_name);
	getch_init();
	vidd_redraw(vidd_get_active());

	for (int i = 2; i < argc; i++)
	{
		vidd_vsplit(vidd_get_active(), argv[i]);
	}

	vidd_main();
	screen_restore();
	return 0;
}