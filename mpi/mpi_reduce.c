#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int np, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // to get np&rank two parameters

    int i, j, k;

    int *message;
    message = (int*)malloc(sizeof(int) * 10);

    for(i = 0; i < 10; i++) {
    	message[i] = i + 1;
    }

    int *sum;
    sum = (int*)malloc(sizeof(int) * 10);
    MPI_Reduce(message, sum, 10, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);	
    if (rank == 0) {
    	for (i = 0; i < 10; i++) {
	    printf("%d ", sum[i]);
	}
	printf("\n");
    }
    
    int *product;
    product = (int*)malloc(sizeof(int) * 10);
    MPI_Reduce(message, product, 10, MPI_INT, MPI_PROD, 0, MPI_COMM_WORLD);
    if (rank == 0) {	
    	for (i = 0; i < 10; i++) {
	    printf("%d ", product[i]);
	}
	printf("\n");
    }

    int *max;
    max = (int*)malloc(sizeof(int) * 10);
    MPI_Reduce(message, max, 10, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);	
    if (rank == 0) {
    	for (i = 0; i < 10; i++) {
	    printf("%d ", max[i]);
	}
	printf("\n");
    }

    MPI_Finalize();

    return 0;
}
