#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

int main(void) {
	int i,j,k;

#pragma omp parallel for num_threads(4)
	for (i = 0; i < 10; i++) {
		printf("hello omp i=%d\n", i);
	}

	return 0;
}
