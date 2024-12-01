#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

float *func1(float a, float b) {
	int i;
	float *result;
	result = (float *)malloc(sizeof(float) * 2);
	for(i = 0; i < 1000000000; i++) {
		result[0] = a + b;
		result[1] = a - b;
	}
	return result;
}

float *func2(float a, float b) {
	int i;
	float *result;
	result = (float *)malloc(sizeof(float) * 2);
	for(i = 0; i < 1000000000; i++) {
		result[0] = a * b;
		result[1] = a / b;
	}
	return result;
}

int main(void) {
	float a = 1.2;
	float b = 2.5;
	float *result1, *result2;

	struct timeval start, end;
	float during;

	gettimeofday(&start, NULL);

#pragma omp parallel sections
	{
#pragma omp section 
		{
			result1 = func1(a, b);
		}
#pragma omp section
		{
			result2 = func2(a, b);
		}
	}

	gettimeofday(&end, NULL);

	during = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

	printf("result1[0]=%f, result1[1]=%f\n", result1[0], result1[1]);
	printf("result2[0]=%f, result2[1]=%f\n", result2[0], result2[1]);
	printf("during time=%f\n", during);

	return 0;
}
