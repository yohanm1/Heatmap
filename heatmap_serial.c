/*
	heatmap_serial.c

	Serial implementation of heat distribution map

	Usage: command line argument for number of iterations
		Ex: gcc heatmap_parallel.c -o heatmap
			./heatmap 1000  <----Indicates 1000 iterations

	Output: heatmap.pnm
			Graphical representation of heat dispersion
*/

#include <stdio.h>
#include <stdlib.h>

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

void printMap(float heatmap[N][N]);
void printRow(int row, float heatmap[N][N]);
void copyNewToOld(float new[N][N], float old[N][N]);
void calculateNew(float new[N][N], float old[N][N]);
void printcolors(float heatmap[N][N]);

int main(int argc, char * argv[])
{
	int i, j;

	float old_map[N][N];
	float new_map[N][N];

	/* Initialize all points to 20 degrees */
	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			old_map[i][j] = 20;
			new_map[i][j] = 20;
		}
	}

	/* Creating the fireplace */
	for (i = 300; i < 700; i++)
	{
		new_map[0][i] = 300;
	}

	/* Calculate heat distribution */
	int iterations = atoi(argv[1]);
	int iter;

	for (iter = 0; iter < iterations; iter++)
	{
		/* Copy new to old */
		copyNewToOld(new_map, old_map);

		/* Calculate new */
		calculateNew(new_map, old_map);
	}

	printcolors(new_map);

	// system("convert c.pnm c.gif");

	return 0;
}

void copyNewToOld(float new[N][N], float old[N][N])
{
	int i, j;

	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			old[i][j] = new[i][j];
		}
	}
}

void calculateNew(float new[N][N], float old[N][N])
{
	int i, j;

	for (i = 0; i < N; i++)
	{
		for (j = 0; j < N; j++)
		{
			if (i != 0 && i != N-1 && j != 0 && j != N-1)
			{
				new[i][j] = 0.25 * (old[i+1][j] + old[i-1][j] + old[i][j+1] + old[i][j-1]);
			}
		}
	}
}

void printcolors(float heatmap[N][N])
{
	FILE * fp;
	int i, j;

	fp = fopen("heatmap.pnm", "w");
	fprintf(fp, "P3\n%d %d\n15\n", N, N);

	for (i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
		{
			char * color;

			if (heatmap[i][j] <= 20)
				color = BLACK;
			else if (heatmap[i][j] <= 30)
				color = BROWN;
			else if (heatmap[i][j] <= 40)
				color = DARKTEAL;
			else if (heatmap[i][j] <= 50)
				color = BLUE;
			else if (heatmap[i][j] <= 60)
				color = LTBLUE;
			else if (heatmap[i][j] <= 80)
				color = GREEN;
			else if (heatmap[i][j] <= 120)
				color = LTGREEN;
			else if (heatmap[i][j] <= 180)
				color = YELLOW;
			else if (heatmap[i][j] <= 250)
				color = ORANGE;
			else if (heatmap[i][j] > 250)
				color = RED;

			fprintf(fp, "%s ", color);
		}

		fprintf(fp, "\n");
	}

	fclose(fp);
}
