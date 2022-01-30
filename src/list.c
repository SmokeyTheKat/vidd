#include "list.h"

#include <string.h>

intmax_t __list_index_of(struct list* list, void* find, intmax_t type_size)
{
	for (intmax_t i = 0; i < list->length; i += type_size)
		if (!memcmp(&list->data[i], (char*)find, type_size))
			return i / type_size;
	return -1;
}
