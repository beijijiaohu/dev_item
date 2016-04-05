#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr[0])))
#endif

static inline assert_failed_expr(const char *expr, const char *file, uint32_t line)
{
	printf ("=====================================================\n");
	printf("BUG: assert failed assert(%s) %s:%u\n", expr, file, (unsigned int)line);
	printf ("=====================================================\n");
	exit(EXIT_FAILURE);
}

#undef assert
#define assert(expr)                                                    \
do {                                                                    \
    if (!(long ) (expr)) {                                              \
        assert_failed_expr(""#expr, __FUNCTION__, __LINE__);            \
    }                                                                   \
} while (0)


#endif /* _COMMON_H_ */

