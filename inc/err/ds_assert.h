#ifndef DS_ASSERT_H
#define DS_ASSERT_H

#ifndef DS_NAME
#error Name of the data structure must be provided as a string via defining DS_NAME
#endif

#include <stdlib.h>

#define DS_ASSERT(BOOL_EXPR, MSG)																			\
	if (!(BOOL_EXPR)) {																						\
	fprintf(stderr, "**" DS_NAME " failure** : %s at %s:%d\n", MSG, __FILE__, __LINE__);					\
	exit(EXIT_FAILURE);																						\
}
	
#endif
