#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "test.h"
#include "anothertest.h"

int main()
{   
	printf("%s\n", TEST_STRING);
	printf("%s", ANOTHER_TEST_STRING);
	t_s ts;
	ts.number = 2321;
	printf("%d\n", ts.number);
	t_s multiplied = mult(ts);
	printf("%d\n", multiplied.number);
    return 0;
}
