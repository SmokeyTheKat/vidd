#include <vidd/vidd.h>

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

	screen_save();

	client_pool = make_vidd_client_pool(20);

	intmax_t width, height;
	screen_get_size(&width, &height);

	for (list_iterate(&args.files, i, char*))
	{
		struct vidd_client client = make_vidd_client(*i, 0, 0, width, height, 0);
		vidd_load_file(&client, *i);
		vidd_client_pool_add(&client_pool, client);
	}

	if (args.read_stdin)
	{
		struct vidd_client client = make_vidd_client("_-=[NONE]=-_", 0, 0, width, height, 0);
		vidd_load_stdin(&client);
		vidd_client_pool_add(&client_pool, client);
	}

	getch_init();

	vidd_main();
	screen_restore();
	return 0;
}






