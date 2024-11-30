#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

const int numFish = 10;
int countFish = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// init a lock named "mutex"
pthread_cond_t cond_catchfish_finish = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_eatfish_finish = PTHREAD_COND_INITIALIZER;

void *fishman() {
	int i, j, k;
	for (i = 0; i < numFish * 2; i++) {
		pthread_mutex_lock(&mutex);
		while (countFish == numFish) {
			pthread_cond_wait(&cond_eatfish_finish, &mutex);
			// wait until recive cond_eatfish_finish signal
			// once in "wait" status release mutex, when pass automatically get mutex
		}
		countFish++;

		printf("Catching Fish! Now there are %d fish, total fish are %d\n", countFish, i+1);
		pthread_cond_signal(&cond_catchfish_finish);
		// if not have enough fish, cat will still wait because mutex not relesed

		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}

void *cat() {
	int i, j, k;
	for (i = 0; i < numFish * 2; i++) {
		pthread_mutex_lock(&mutex);
		while (countFish == 0) {
			pthread_cond_wait(&cond_catchfish_finish, &mutex);
			// wait until recive cond_eatfish_finish signal
			// once in "wait" status release mutex, when pass automatically get mutex
		}
		countFish--;

		printf("Eating Fish! Now there are %d fish\n", countFish);
		pthread_cond_signal(&cond_eatfish_finish);
		
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(NULL);
}

int main(void) {
	pthread_t fishmanThread, catThread;
	pthread_create(&fishmanThread, NULL, fishman, NULL);
	pthread_create(&catThread, NULL, cat, NULL);

	pthread_join(fishmanThread, NULL);
	pthread_join(catThread, NULL);

	printf("The cat is full!\n");

	return 0;
}
