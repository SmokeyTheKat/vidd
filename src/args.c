#include <vidd/args.h>

#include <string.h>

struct arguments parse_args(int argc, char** argv)
{
	struct arguments args = {0};

	if (args.files.data == 0)
	{
		args.files = make_list(5, char*);
	}
	for (int i = 1; i < argc; i++)
	{
		if (strcmp(argv[i], "-r") == 0)
		{
			args.readonly = true;
		}
		else if (strcmp(argv[i], "-") == 0)
		{
			args.read_stdin = true;
		}
		else
		{
			list_push(&args.files, argv[i], char*);
		}
	}
	if (list_length(&args.files, char*) == 0)
	{
		list_push(&args.files, "_-=[NONE]=-_", char*);
	}
	return args;
}