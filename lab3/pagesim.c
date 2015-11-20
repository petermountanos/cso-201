#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "algorithms.h"

#define MIN_MEMORY_FRAMES 0
#define MAX_MEMORY_FRAMES 100
#define MAX_PAGE_REFERENCES 10000

/* 
 * Author: Peter Mountanos
 * Date created: Apr. 24, 2015
 * 
 * Description:
 * pagesim reads a sequence of pages from the provided input file, and simulates
 * a page replacement algorithm, based on the inputted algorithm and frame size.
 * The input file has to contain the numbers only separated by spaces, and the
 * numbers should be from 0 to 99. Also, the algorithm input can either be 'lru'
 * or 'fifo' or 'extra', and the total number of physical memory frames must be
 * [0, 100].
 * 
 * Usage:
 *   pagesim num_memory_frames file algo
 * 
 * pagesim accepts three command line arguments
 * num_memory_frames  - the total number of physical memory frames (maximum 100)
 * file - the name of the input file that contains a list of page references
 * algo - the chosen algorithm (either lru or fifo or extra)
 */


//======================================================//
const char * usage = "Usage:"
"  pagesim file \n"
"\n"
"pagesim accepts three command line arguments     \n"
"num_memory_frames  - the total number of physical memory frames (maximum 100) \n"
"file - the name of the input file that contains a list of page references \n"
"algo - the chosen algorithm (either lru or fifo or extra) \n"
"\n"
"\n";
//======================================================//

/*
 * This function verifies the command line arguments to ensure that they
 * meet their preconditions. If these arguments don't, an error message is
 * printed and the program quits.
 *		:param num_memory_frames: the total number of physical memory
 								  frames (maximum 100)
 		:param algo: the chosen algorithm (either lru or fifo) 
 */
void verify_input(int num_memory_frames, char * algo) { 

	// total number of physical memory frames must be in set range
	if (num_memory_frames < MIN_MEMORY_FRAMES || 
		num_memory_frames > MAX_MEMORY_FRAMES) {
		printf("Error: range of number of memory frames is [%d, %d], received %d.\n",
		 	   MIN_MEMORY_FRAMES, MAX_MEMORY_FRAMES, num_memory_frames);
		exit(1);
	}

	// verify algorithm name passed in is valid
	if (strcmp(algo, "lru") != 0 && strcmp(algo, "fifo") != 0 &&
		strcmp(algo, "extra") != 0) {
		printf("Error: algorithm usage (lru, fifo, or extra); received %s.\n", algo);
		exit(1);
	}
}

/*
 * Main function for the pagesim application. This function takes in the
 * three command line arguments specified above in the file comments. After
 * all of the data is read in, the program runs either an LRU replacement 
 * algorithm simulator, or a FIFO replacement algorithm simulator, based on
 * inputted arguments. The program then prints out the miss rate for the 
 * simulation.
 */
int main(int argc, char *argv[]) {

	int num_memory_frames; /* number of physical memory frames */
	FILE *fp; 			   /* input file */
	char * algo; 		   /* chosen algorithm */
	char file_name[256];

	/* initialize page references to max size for now */
	int* page_references = malloc(MAX_PAGE_REFERENCES*sizeof(int));

	/* checking the input from the command line */
	if (argc != 4) {
		printf("Error: Invalid number of parameters.\n\n%s", usage);
		exit(1);
	}

	/* store command line arguments */
	num_memory_frames = atoi(argv[1]);
	algo = argv[3];

	/* verify arguments match preconditions */
	verify_input(num_memory_frames, algo);

	/* create the input file pointer*/
	strncpy (file_name, argv[2], 256);
	fp = fopen(file_name, "r");
	if(!fp) {
		printf("Error: cannot open file %s for reading.\n", file_name);
		exit(1);
	}


	/* read the values from the input file and accumulate them into array.  */
	int i = 0;
	while (fscanf( fp, "%d", &page_references[i++]) == 1);
	fclose(fp);

	/* reallocate memory store for array to correct size */
	int num_pages = (i-1);
	page_references = realloc(page_references, num_pages * sizeof(int));

	/* determine which page replacement algorithm to run (in verbose mode) */
	int stats[2];
	if (strcmp(algo, "lru") == 0) { 
		lru(page_references, num_pages, num_memory_frames, stats, 1);
	}
	else if (strcmp(algo, "fifo") == 0) {
		fifo(page_references, num_pages, num_memory_frames, stats, 1);
	}
	else {
		extra(page_references, num_pages, num_memory_frames, stats, 1);
	}

	/* calculate and display miss rate to user */
	double miss_rate;
	if (stats[1] == 0)
		miss_rate = NAN;
	else
		miss_rate = (((double) stats[0]) / stats[1]) * 100;

	printf("\nMiss Rate = %d / %d = %3.2f%%\n", stats[0], stats[1], miss_rate);

	return 0;
}