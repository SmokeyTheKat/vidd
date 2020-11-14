#include "../src/vidd.h"

int main(void)
{
	struct vidd vidd = make_vidd(170, 47);
	vidd_main_input_loop(&vidd);
	return 0;
}
