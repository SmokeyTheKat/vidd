omg hi there how are you doing to day :) omg wow this is a very long line of text that is in this file... can vidd handle it? idk we will find out shortly :)
wow i am doing soooooo good haha
1 2 3 4 5 6 7 8 9
10 11 12 13 14 15
16 17 18 19 20 21
22 23 24 25 26 27
16 17 18 19 20 21
10 11 12 13 14 15
1 2 3 4 5 6 7 8 9
Ó
static struct person a = { "omg", 69 };
int main(int argc, char** argv)
{
	bool running = true;
	if (argc <= 1) return 0;
	if (*(uint32_t*)argv[1] == *(uint32_t*)"    ")
		printf("space/tab");
	while (running == true)
	{
		int v;
		scanf("%d", &v);

		if (v == 0) running = false;

		// print out all number seperated with a '-'
		for (int i = 0; i < v; i++)
		{
			printf("%d", v);
			if (i + 1 < v) printf(" - ");
			else printf("\n");
		}
	}
	return 0;
}
--------------------------
				 ____    |
		0      _|    |_  |
	   -|-     |_______| |
	   / \      0   0    |
--------------------------
