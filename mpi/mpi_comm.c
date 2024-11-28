#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MASTER 0
#define MESSAGE 1

void master(int np, int rank) {
    char message[100];
    int i;

    for (i = 1; i < 10; i++) {
    	sprintf(message, "Hello %d", i);
	// MPI_Send(message, 100, MPI_CHAR, i, MESSAGE, MPI_COMM_WORLD);
	// because dst depends on the number of process(np).
	MPI_Send(message, 100, MPI_CHAR, i%(np-1)+1, MESSAGE, MPI_COMM_WORLD);
    }

    for (i = 1; i < np; i++) {
    	sprintf(message, "q"); // means quit flag
	MPI_Send(message, 100, MPI_CHAR, i, MESSAGE, MPI_COMM_WORLD);
    }
}

void slave(int np, int rank) {
    char message[100];
    int i;

    while(1) {
    	MPI_Recv(message, 100, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, // TAG as a category for messages
		MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	if (message[0] == 'q') {
	    break;
	}

	printf("Message received bt %d: %s\n", rank, message);
    }
}

int main(int argc, char *argv[]) {
    int np, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // to get np&rank two parameters

    if (rank == MASTER) {
        master(np, rank);
    } else {
    	slave(np, rank);
    }
    
    MPI_Finalize();

    return 0;
}
