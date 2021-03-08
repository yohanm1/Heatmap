/*
	heatmap_parallel.c

	Parallel implementation of heat distribution map

	Usage: command line argument for number of iterations
		Ex: mpicc heatmap_parallel.c -o heatmap
			mpiexec -np 4 heatmap 1000  <----Indicates 1000 iterations

			Cannot run with -np 1

	Output: heatmap.pnm
			Graphical representation of heat dispersion
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define WHITE    "15 15 15 "
#define RED      "15 00 00 "
#define ORANGE   "15 05 00 "
#define YELLOW   "15 10 00 "
#define LTGREEN  "00 13 00 "
#define GREEN    "05 10 00 "
#define LTBLUE   "00 05 10 "
#define BLUE     "00 00 10 "
#define DARKTEAL "00 05 05 "
#define BROWN    "03 03 00 "
#define BLACK    "00 00 00 "

#define N 1000

int main(int argc, char * argv[])
{
	int i, j, rank, size;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int rows = N/size + 2;

	float old_map[rows][N];
	float new_map[rows][N];
	float test[rows*size][N];

	/* Initialize all points to 20 degrees */
	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < N; j++)
		{
			old_map[i][j] = 20;
			new_map[i][j] = 20;
		}
	}

	/* Creating the fireplace */
	if (rank == 0)
	{
		for (i = 300; i < 700; i++)
		{
			new_map[1][i] = 300;
		}
	}

	int sendbuf = rows * N;
	MPI_Gather(new_map, sendbuf, MPI_FLOAT, test, sendbuf, MPI_FLOAT, 0, MPI_COMM_WORLD);

	/* Calculate heat distribution */
	int iterations = atoi(argv[1]);
	int iter;

	for (iter = 0; iter < iterations; iter++)
	{
		/* Copy new to old */
		for (i = 0; i < rows; i++)
		{
			for (j = 0; j < N; j++)
			{
				old_map[i][j] = new_map[i][j];
			}
		}

		/* Update ghost rows */
		if (rank == 0)
		{
			MPI_Send(old_map[rows-2],N,MPI_FLOAT,rank+1,0,MPI_COMM_WORLD);
			MPI_Recv(old_map[rows-1],N,MPI_FLOAT,rank+1,0,MPI_COMM_WORLD,&status);
		}
		else if (rank != size-1)
		{
			MPI_Send(old_map[1],N,MPI_FLOAT,rank-1,0,MPI_COMM_WORLD);
			MPI_Recv(old_map[0],N,MPI_FLOAT,rank-1,0,MPI_COMM_WORLD,&status);

			MPI_Recv(old_map[rows-1],N,MPI_FLOAT,rank+1,0,MPI_COMM_WORLD,&status);
			MPI_Send(old_map[rows-2],N,MPI_FLOAT,rank+1,0,MPI_COMM_WORLD);
		}
		else if (rank == size-1)
		{
			MPI_Send(old_map[1],N,MPI_FLOAT,rank-1,0,MPI_COMM_WORLD);
			MPI_Recv(old_map[0],N,MPI_FLOAT,rank-1,0,MPI_COMM_WORLD,&status);
		}

		MPI_Barrier(MPI_COMM_WORLD);

		/* Calculate new */
		if (rank == 0)
		{
			for (i = 2; i < rows-1; i++)
			{
				for(j = 1; j < N-1; j++)
				{
					new_map[i][j] = 0.25 * (old_map[i+1][j] + old_map[i-1][j] + old_map[i][j+1] + old_map[i][j-1]);
				}
			}
		}
		else if (rank != size-1)
		{
			for (i = 1; i < rows-1; i++)
			{
				for (j = 1; j < N-1; j++)
				{
					new_map[i][j] = 0.25 * (old_map[i+1][j] + old_map[i-1][j] + old_map[i][j+1] + old_map[i][j-1]);
				}
			}
		}
		else if (rank == size-1)
		{
			for (i = 1; i < rows-2; i++)
			{
				for (j = 1; j < N-1; j++)
				{
					new_map[i][j] = 0.25 * (old_map[i+1][j] + old_map[i-1][j] + old_map[i][j+1] + old_map[i][j-1]);
				}
			}
		}

		MPI_Barrier(MPI_COMM_WORLD);
		MPI_Gather(new_map,sendbuf,MPI_FLOAT,test,sendbuf,MPI_FLOAT,0,MPI_COMM_WORLD);
	}

	/* Print result to file */
	if(rank==0)
	{
		FILE * fp;

		fp = fopen("heatmap.pnm", "w");
		fprintf(fp, "P3\n%d %d\n15\n", N, N);

		int traverse = 0;

		for (i = 1; i < rows*size; i++)
		{
			traverse++;

			for (j = 0; j < N; j++)
			{
				char * color;

				if (test[i][j] <= 20)
					color = BLACK;
				else if (test[i][j] <= 30)
					color = BROWN;
				else if (test[i][j] <= 40)
					color = DARKTEAL;
				else if (test[i][j] <= 50)
					color = BLUE;
				else if (test[i][j] <= 60)
					color = LTBLUE;
				else if (test[i][j] <= 80)
					color = GREEN;
				else if (test[i][j] <= 120)
					color = LTGREEN;
				else if (test[i][j] <= 180)
					color = YELLOW;
				else if (test[i][j] <= 250)
					color = ORANGE;
				else if (test[i][j] > 250)
					color = RED;

				fprintf(fp, "%s ", color);
			}

			if (traverse == rows-2)
			{
				i += 2;
				traverse = 0;
			}

			fprintf(fp, "\n");
		}

		fclose(fp);
	}

	// system("convert c.pnm c.gif");

	MPI_Finalize();
	return 0;
}
