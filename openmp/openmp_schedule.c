#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(void) {
	float a = 1.2;
	float b = 2.5;
	float *result;

	result = (float *)malloc(sizeof(float) * 10);
#pragma omp parallel for num_threads(4) schedule(dynamic, 2)
	// means each thread processes two tasks
	// also schedule(static, 2), schedule(guided, 2)

	for(int i = 0; i < 10; i++) {
		result[i] = i * 2;
		printf("write num from %d\n", omp_get_thread_num());
	}

#pragma omp parallel for num_threads(4) schedule(dynamic, 2)
	for(int i = 0; i < 10; i++) {
#pragma omp critical
		{
			result[i] = i * 3;
		}
		// one thread
		// critical means only one thread enter for one time
	}
#pragma omp barrier
	// barrier means wait for all threads finishing, then continue below
	// synch point
	return 0;
}
