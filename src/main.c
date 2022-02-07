#include <vidd/vidd.h>

#include <vidd/tab.h>
#include <vidd/args.h>
#include <vidd/list.h>
#include <vidd/utils.h>
#include <vidd/getch.h>
#include <vidd/commands.h>
#include <vidd/display.h>

#include <stdlib.h>
#include <locale.h>
#include <signal.h>

int main(int argc, char** argv)
{
	setlocale(LC_ALL, "en_US.utf8");
	setbuf(stdout, 0);

	struct arguments args = parse_args(argc, argv);

	if (args.test_getch)
	{
		getch_init();
		getch_test();
		getch_exit();
		return 0
	}

	screen_save();

	init_vidd_tab();

	intmax_t width, height;
	screen_get_size(&width, &height);

	for (list_iterate(&args.files, i, char*))
	{
		struct vidd_client client = make_vidd_client(*i, 0, 0, width, height, 0);
		vidd_load_file(&client, *i);
		vidd_tab_add_client(active_tab, client);
	}

	if (args.read_stdin)
	{
		struct vidd_client client = make_vidd_client("_-=[NONE]=-_", 0, 0, width, height, 0);
		vidd_load_stdin(&client);
		vidd_tab_add_client(active_tab, client);
	}

	getch_init();

	vidd_main();
	screen_restore();
	return 0;
}






