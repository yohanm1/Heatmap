OpenMPI Heatmap 
Author: Yohan Moon
Due Date: 10/12/2020

Files:
	heatmap_serial.c
	heatmap_parallel.c
	readme.txt

Input:
	Provide command line argument indicating how many iterations

Usage:
	mpicc heatmap_parallel.c -o heatmap
	mpiexec -np 4 heatmap 1000

	*Above example uses 1000 iterations*
	
	Cannot run with -np 1

Team roles:
	Did not work with team. All work is my own.