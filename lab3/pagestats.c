#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "algorithms.h"

#define MIN_MEMORY_FRAMES 2
#define MAX_MEMORY_FRAMES 100
#define MAX_PAGE_REFERENCES 10000

/* 
 * Author: Peter Mountanos
 * Date created: Apr. 24, 2015
 * 
 * Description:
 * pagestats reads a sequence of pages from the provided input file, and simulates
 * page replacement algorithms. It does this by looping over the two (LRU & FIFO)
 * algorithms, and for each method it loops over the number of frames, starting at
 * the minimum and applying the incrmeent until it exceeds the maximum (i.e., 5,
 * 15, 25, 35, in the example pagestats 5 40 10 page_refs.txt). For each method/number
 * of frames combinations, the program calculates the page fault rate using the 
 * reference file given as input, and prints out a message containing this rate.
 * 
 * Usage:
 *   pagestats min_frames max_frames frame_inc file
 * 
 * pagesim accepts four command line arguments
 * min_frames - the minimum number of frames (no less than 2)
 * max_frames - the maximum number of frames (no more than 100)
 * frame_inc  - the frame number increment (positive integer)
 * file - the name of the input file that contains a list of page references
 */

//======================================================//
const char * usage = "Usage:"
"  pagestats file \n"
"\n"
"pagestats accepts four command line arguments     \n"
"min_frames - the minimum number of frames (no less than 2) \n"
"max_frames - the maximum number of frames (no more than 100) \n"
"frame_inc  - the frame number increment (positive integer) \n"
"file - the name of the input file that contains a list of page references \n"
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
void verify_input(int min_frames, int max_frames, int frame_inc) { 

	if (min_frames < 2) {
		printf("Error: minimum number of frames can be no less than 2; received %d\n", min_frames);
		exit(1);
	}
	
	if (max_frames > 100) {
		printf("Error: maximum number of frames can be no more than 100; received %d\n", max_frames);
		exit(1);
	}
	 
	if (min_frames > max_frames) {
		printf("Error: minimum number of frames cannot be more than maximum number of frames\n");
		exit(1);
	}
	
	if (frame_inc < 0) {
		printf("Error: frame number increment must be a positive integer; received %d\n", frame_inc);
		exit(1);
	}
}

/*
 * This function is responsible for printing the results of a certain run of 
 * an algorithm, and also writing the miss rate to a file.
 * 		:param algo: the name of the algorithm run (fifo, lru, extra)
 * 		:param frame_num: the number of frames used for the run
 *		:param stats: an array whose first index holds the number of 
 					  page faults for the run of the algorithm, and 
 					  the second index holds the number of number of
 					  references for the run of the algorithm
 		:param tf: pointer to the target file to write results to
 */
void print_results(char * algo, int frame_num, int stats[], FILE * tf) {

	/* if there were no page references, miss rate is NaN */
	double miss_rate;
	if (stats[1] == 0)
		miss_rate = NAN;
	else /* otherwise miss rate = num page faults / num page references */
		miss_rate = ((double) stats[0] / stats[1]) * 100;

	/* print to stdout first, and then to target file (tf) */
	printf("%s, %3d frames: Miss Rate = %3d / %3d = %3.2f%%\n", algo, frame_num, stats[0], stats[1], miss_rate);
	fprintf(tf, "%3.2f ", miss_rate);
}

/*
 * Main function for the pagestats application. This function takes in the
 * four command line arguments specified above in the file comments. After
 * all of the data is read in, the program runs a series of simulations on
 * the inputted data using both LRU and FIFO replacement policies, and the 
 * number of frames varies based on the input as well. The program then 
 * prints out the miss rate for the simulation.
 */
int main(int argc, char *argv[]) {

	int min_frames; 		/* min number of frames (no less than 2) */
	int max_frames;			/* max number of frames (no more than 100) */
	int frame_inc;			/* frame number increment (positive integer) */
	FILE *fp; 			    /* input file */
	char file_name[256];

	/* initialize page references to max size for now */
	int* page_references = malloc(MAX_PAGE_REFERENCES*sizeof(int));

	/* checking the input from the command line */
	if(argc != 5 ) {
		printf("Error: Invalid number of parameters.\n\n%s", usage);
		exit(1);
	}

	/* store command line arguments */
	min_frames = atoi(argv[1]);
	max_frames = atoi(argv[2]);
	frame_inc  = atoi(argv[3]);

	/* verify arguments match preconditions */
	verify_input(min_frames, max_frames, frame_inc);

	/* create the input file pointer*/
	strncpy (file_name, argv[4], 256);
	fp = fopen(file_name, "r");
	if(!fp) {
		printf("Error: cannot open file %s for reading.\n", file_name);
		exit(1);
	}

	/* set up for writing to output file*/
	char * output_file = "pagerates.txt";
	FILE * tf = fopen(output_file, "w"); 
	if (!tf) {
		printf("Error: cannot open file %s for writing.\n", output_file);
	}

	/* read the values from the input file and accumulate them into array.  */
	int i = 0;
	while (fscanf( fp, "%d", &page_references[i++]) == 1);
	fclose(fp);

	/* reallocate memory store for array to correct size */
	int num_pages = (i-1);
	page_references = realloc(page_references, num_pages * sizeof(int));

	/* first line of output file should be sequence of frames */
	for (i = min_frames; i <= max_frames; i += frame_inc)
		fprintf(tf, "%d ", i);
	fprintf(tf, "\n");

	/* run series of page replacement simulations and print out results */
	int stats[2];

	/* start with LRU algo */
	for (i = min_frames; i <= max_frames; i += frame_inc) {
		lru(page_references, num_pages, i, stats, 0);
		print_results("LRU", i, stats, tf);
	}

	printf("\n");
	fprintf(tf, "\n");

	/* then execute FIFO algo */
	for (i = min_frames; i <= max_frames; i += frame_inc) {
		fifo(page_references, num_pages, i, stats, 0);
		print_results("FIFO", i, stats, tf);
	}
	printf("\n");
	fprintf(tf, "\n");

	/* finally execute extra algo */
	for (i = min_frames; i <= max_frames; i += frame_inc) {
		extra(page_references, num_pages, i, stats, 0);
		print_results("EXTRA", i, stats, tf);
	}
	printf("\n");
	fprintf(tf, "\n");

	return 0;
}