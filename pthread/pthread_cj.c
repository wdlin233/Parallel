#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

typedef struct {
	float a;
	float b;
}Args;

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

void *thread_func1(void *arg) {
	Args *args = (Args *)arg;
	return func1(args->a, args->b);
}

void *thread_func2(void *arg) {
	Args *args = (Args *)arg;
	return func2(args->a, args->b);
}

int main(void) {
	float a = 1.2;
	float b = 2.5;
	float *result1, *result2;

	Args ab;
	ab.a = a;
	ab.b = b;
	void *r1, *r2; // to store the result of pthread
	pthread_t tid1, tid2;

	struct timeval start, end;
	float during;

	gettimeofday(&start, NULL);

	// result1 = func1(a, b);
	// result2 = func2(a, b);

	pthread_create(&tid1, NULL, thread_func1, &ab);
	pthread_create(&tid2, NULL, thread_func2, &ab);
	pthread_join(tid1, &r1);
	pthread_join(tid2, &r2);

	// implicit call pthread_exit();

	result1 = (float *)r1;
	result2 = (float *)r2;

	gettimeofday(&end, NULL);

	during = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

	printf("result1[0]=%f, result1[1]=%f\n", result1[0], result1[1]);
	printf("result2[0]=%f, result2[1]=%f\n", result2[0], result2[1]);
	printf("during time=%f\n", during);

	return 0;
}
