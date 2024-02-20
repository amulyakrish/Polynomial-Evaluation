#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#define DEGREE 5
#define x 1


double computeSegment(int *coeffArr, int size)
{
    double partialSum = 0.0;

    printf("Received array :\n");
    for (int i = 0; i < size; i++) {
        printf("%d : %d \n",i,coeffArr[i]);
    }

    for (int i = 0; i < size; i++) {
        partialSum += coeffArr[i] * pow(x, i);
    }
    printf("partial sym %f",partialSum);
    return partialSum;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int rank;
    int numProcesses;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    int randomChunkSize = 1;
    char hostname[256];
    gethostname(hostname, 255);
    printf("Hello world! I am process number: %d on host %s\n", rank, hostname);

    srand(time(NULL));

    int MAX = DEGREE + 1;
    int *coeffArr = (int *)malloc(sizeof(int) * MAX);
   
int *received_data = (int *)malloc(sizeof(int) * MAX);
int terminationTag=3;
int workTag=0;
int receivingPartialResultTag=1;
int source=0;
int count=0;

   
    

    if (rank == 0) {
        // Master process
        int start = 0;
        int globalSum = 0;

        for (int i = 0; i < MAX; i++) {
            coeffArr[i] = i + 1; // Replace with your actual polynomial coefficients.
        }


for (int i = 1; i < numProcesses; i++) {
                if(start < MAX) {
                    int remainingTerms = MAX - start;

                    if (remainingTerms <= randomChunkSize) {
                        randomChunkSize = remainingTerms;
                    }
                    MPI_Send(&randomChunkSize, 1, MPI_INT, i, workTag, MPI_COMM_WORLD); // Send chunk size

                    MPI_Send(&coeffArr[start], randomChunkSize, MPI_INT, i, workTag, MPI_COMM_WORLD);
                    start += randomChunkSize;
                }
            }


        while (start < MAX) {

            	 double partialSum;
		MPI_Status status;
                MPI_Recv(&partialSum, 1, MPI_DOUBLE, MPI_ANY_SOURCE, receivingPartialResultTag, MPI_COMM_WORLD, &status);
                int tag = status.MPI_TAG;
                if(tag == 1) {
                    globalSum += partialSum;



                    // Send a new chunk of work to the requesting worker.

               

                        int remainingTerms = MAX - start;

                        if (remainingTerms <= randomChunkSize) {
                            randomChunkSize = remainingTerms;
                        }

                        printf("sending more work ......\n");
                        MPI_Send(&randomChunkSize, 1, MPI_INT, status.MPI_SOURCE, workTag, MPI_COMM_WORLD); // Send chunk size

                        MPI_Send(&coeffArr[start], randomChunkSize, MPI_INT, status.MPI_SOURCE, workTag, MPI_COMM_WORLD);
                        start += randomChunkSize;

                    

                }
        }

double partialSum;
MPI_Status status;
                for (int i = 1; i < numProcesses; i++) {
                    MPI_Recv(&partialSum, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status);
                    int tag = status.MPI_TAG;
                    if(tag == 1) {
                        globalSum += partialSum;
                    }
                }

                for (int i = 1; i < numProcesses; i++) {
                    int terminationMsg = -1; // Termination message
                    MPI_Send(&terminationMsg, 1, MPI_INT, i, 3, MPI_COMM_WORLD);
                }


                printf("Global sum: %d\n", globalSum); // Print the global sum here.
                MPI_Finalize();
            }
            else {
                // Worker processes
                while (1) {
                    int chunkSize;
		    MPI_Status status;
                    MPI_Recv(&chunkSize, 1, MPI_INT, source, terminationTag, MPI_COMM_WORLD, &status);
                    int tag=status.MPI_TAG;


                    if (tag == 3) {
                        printf("Worker process %d received termination message. Exiting.\n", rank);
                        MPI_Finalize();//MPI_ABORT
                    } else {
                        int received_count;
                        MPI_Recv(&received_count, 1, MPI_INT, source, workTag, MPI_COMM_WORLD, &status); // Receive chunk size
                        //MPI_Get_count(&status, MPI_INT, &received_count);
                        MPI_Recv(&received_data, chunkSize, MPI_INT, source, workTag, MPI_COMM_WORLD, &status);

                        double partialSum = computeSegment(received_data, received_count);
			++count;
			printf("Partial sum  for round %d is %d",count,partialSum);
                        MPI_Send(&partialSum, 1, MPI_DOUBLE, source, receivingPartialResultTag, MPI_COMM_WORLD);
                    }

                    
                }

            }


            free(coeffArr);

            free(received_data);
            return 0;
        }