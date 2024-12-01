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
	float *result;

	result = (float *)malloc(sizeof(float) * 10);
#pragma omp parallel for num_threads(4)
	for(int i = 0; i < 10; i++) {
		result[i] = i * 2;
		printf("write num from %d\n", omp_get_thread_num());
	}

	for(int i = 0; i < 10; i++) {
		printf("%f\n", result[i]);
	}
	return 0;
}
