#include <stdio.h>
#include <limits.h>

/* 
 * Author: Peter Mountanos
 * Date created: Apr. 24, 2015
 * 
 * Description:
 * algorithms contains the implementation of two different page replacement algorithms,
 * namely the FIFO policy and the LRU policy. This source file also contains several 
 * utility methods which execute common functionality between the two different policies.
 * One of those utility functions is display, which provides the code for the common 
 * visual simulation for the page replacement process. Note, this function only gets 
 * called if the lru or fifo algorithms are run in verbose mode.
 * 
 * Usage:
 *   Compile with another file; there is no main function
 */

//======================================================//

/*
 * Function to search for a value in an array. 
 * 		:param arr: array to be searched
 *		:param size: size of array
 *		:param item: item to search array for
 * **Returns**: index of item if it's in the array, -1 otherwise. 
 */
int search(int arr[], int size, int item) {	
	int i;
	for (i = 0; i < size; i++) {
		if (arr[i] == item)
			return i;
	}
	return -1;
}

/*
 * Function to increment each value in an array by one.
 *		:param arr: array where values should be incremented
 *		:param size: size of array
 */
void increment_arr(int arr[], int size) {
	int i;
	for (i = 0; i < size; i++){
		arr[i]++;
	}
}

/*
 * Function to find the max value in an array. 
 *		:param arr: array to be searched
 *		:param size: size of array
 * **Returns**: the index of the maximum value in arr
 */
int find_max(int arr[], int size) {
	int i, max_index = 0;
	for (i = 1; i < size; i++) {
		if (arr[i] > arr[max_index]) 
			max_index = i;
	}
	return max_index;
}

/*
 * Function to find the optimal value to replace. This is for the 
 * extra credit replacement algorithm. It follows the optimal replacement
 * policy as described in the extra function docstring.
 *		:param pages: array of future pages to be allocated
 *		:param num_pages: number of future pages (i.e., size of pages array)
 *		:param frames: current situation of frames in physical memory
 *		:param num_frames: number of frames in physical memory (i.e., size of frames array)
 *		:param num_read: the number of pages we've read thus far
 * **Returns**: the index of the optimal victim frame. This represents the frame that has
 * a page reference the farthest in the future, or not at all. If there are multiple frames
 * with no future reference, it returns the first.
 */
 int find_opt(int pages[], int num_pages, int frames[], int num_frames, int num_read) {

 	/* start each page in memory to be maximum distance away from use */ 
 	int dist_from_use[num_frames], i;
 	for (i = 0; i < num_frames; i++)
 		dist_from_use[i] = INT_MAX;

 	/* for each frame, read through the future pages to see how far away it actually is */
 	for (i = 0; i < num_frames; i++) {
 		int dist = 0, j;
 		for (j = num_read - 1; j < num_pages; j++) {
 			/* if a future page is current frame, set dist and break */
 			if (pages[j] == frames[i]) {
 				dist_from_use[i] = dist;
 				break;
 			}
 			dist++;
 		}
 	}

 	return find_max(dist_from_use, num_frames);
 }

/*
 * Function to output the current status of the frames to stdout. The
 * specifications of how the frames are displayed are given in the 
 * assignment prompt.
 *		:param frames: array containing the current pages in physical memory
 *		:param num_frames: the number of frames allocated in physical memory
 *		:param page: the current page that has to be allocated
 *		:param faulted: "boolean" value which indicates if the page resulted
 *						in a fault of not (used for printing F is so)
 */
void display(int frames[], int num_frames, int page, int faulted) {

	/* for each frame pos, print it out followed by | */
	printf("%2d: [", page);
	int i;
	for (i = 0; i < num_frames-1; i++) {
		/* if unallocated, print two empty spaces */
		if (frames[i] == -1) 
			printf("  |");
		else 
			printf("%2d|", frames[i]);
	}

	/* for the last element, don't include | */
	if (frames[i] == -1)
		printf("  ");
	else
		printf("%2d", frames[i]);

	/* if current operation was a fault, print F after frame */
	if (faulted)
		printf("] F\n");
	else
		printf("]\n");
}

/*
 * A first-in-first-out (FIFO) page replacement algorithm implementation, as specified
 * by the assignment.
 *		:param arr: an array of pages to be allocated
 *		:param arr_size: the number of pages to be allocated
 *		:param frame_nun: the number of frames in physical memory
 *		:param stats: an array which will eventually store the number of page faults
 *					  for this run of the algorithm (at index 0), and the number of
 *					  references (at index 1). These are then used in by the caller
 *					  to calculate the miss rate
 *		:param verbose: "boolean" to indicate whether to run in verbose mode or not;
 *						if so, then each allocation process is displayed.
 */
void fifo(int arr[], int arr_size, int frame_num, int stats[], int verbose) {
	
	/* initialize frame array to -1's */
	int frames[frame_num], i;
	for (i = 0; i < frame_num; i++) {
		frames[i] = -1;
	}

	/* initialize local variables: 
	 *	- res: stores the result of whether or not the page is already in memory
	 *	- num_faults: stores the current number of page faults that have occured
	 *	- num_refs: stores the current number of page references that have occured
	 * 	- faulted: temp boolean value to determine if the current page resulted in
	 *			   a fault or not
	 *	- is_filled: boolean value to determine if the frames are filled
	 *	- pointer: value storing the index of the current object that was first allocated
	 *			   (to be used for replacement part)
	 *	- num_allocated: stores the current number of pages allocated into a frame
	 */
	int res, num_faults = 0, num_refs = 0, is_filled = 0;
	int faulted, num_allocated = 0, pointer = 0;

	for (i = 0; i < arr_size; i++) {

		/* see if page is already in frames */
		res = search(frames, frame_num, arr[i]);
		faulted = 0;

		/* if the frame is full, count towards references */
		if (is_filled || num_allocated >= frame_num) {
			is_filled = 1;
			num_refs++;
		}

		/* is page is not in frame, replacement is needed */
		if (res == -1) {
			/* results in a fault (but may not be counted towards num of page faults) */
			num_allocated++;
			faulted = 1;

			/* replace first in with current page */
			frames[pointer] = arr[i];
			pointer = (pointer + 1) % frame_num;
		}

		/* if the frame is full, and the page wasn't in frame then count towards faults */
		if (is_filled && faulted) {
			num_faults++;
		}

		/* only print current operation if verbose mode is on */
		if (verbose)
			display(frames, frame_num, arr[i], (faulted && is_filled));
	}

	/* set number of faults and references to 'return' array so miss
	 * rate can be calculated by the caller function */
	stats[0] = num_faults;
	stats[1] = num_refs;
}

/*
 * A least-recently-used (LRU) page replacement algorithm implementation, as specified
 * by the assignment.
 *		:param arr: an array of pages to be allocated
 *		:param arr_size: the number of pages to be allocated
 *		:param frame_nun: the number of frames in physical memory
 *		:param stats: an array which will eventually store the number of page faults
 *					  for this run of the algorithm (at index 0), and the number of
 *					  references (at index 1). These are then used in by the caller
 *					  to calculate the miss rate
 *		:param verbose: "boolean" to indicate whether to run in verbose mode or not;
 *						if so, then each allocation process is displayed.
 */
void lru(int arr[], int arr_size, int frame_num, int stats[], int verbose) {

	/* initialize frame array to -1's, and last_used array to 0's */
	int frames[frame_num], last_used[frame_num], i;
	for (i = 0; i < frame_num; i++) {
		frames[i] = -1;
		last_used[i] = 0;
	}

	/* initialize local variables: 
	 *	- res: stores the result of whether or not the page is already in memory
	 *	- num_faults: stores the current number of page faults that have occured
	 *	- num_refs: stores the current number of page references that have occured
	 * 	- faulted: temp boolean value to determine if the current page resulted in
	 *			   a fault or not
	 *	- is_filled: boolean value to determine if the frames are filled
	 *	- num_allocated: stores the current number of pages allocated into a frame
	 */
	int res, faulted = 0, is_filled = 0;
	int num_faults = 0, num_refs = 0, num_allocated = 0;

	/* for each page in arr...follow LRU replacement policy */
	for (i = 0; i < arr_size; i++) {

		/* increment `age` of current pages in frames */
		increment_arr(last_used, frame_num);

		/* see if page is already in frames */
		res = search(frames, frame_num, arr[i]);
		faulted = 0;

		/* if the frame is full, count towards references */
		if (is_filled || num_allocated >= frame_num) {
			is_filled = 1;
			num_refs++;
		}

		/* is page is not in frame, replacement is needed */
		if (res == -1) {
			/* find least recently used page in frames */
			int index = find_max(last_used, frame_num);

			/* replace LRU element w/ page, and reset `age` */
			frames[index] = arr[i];
			last_used[index] = 0;

			/* increment number allocated, and set faulted to true */
			num_allocated++;
			faulted = 1;
		}
		else {
			/* if in frame, just reset `age` (b/c just called) */
			last_used[res] = 0;
		}

		/* if the frame is full, and the page wasn't in frame then count towards faults */
		if (is_filled && faulted) {
			num_faults++;
		}

		/* only print current operation if verbose mode is on */
		if (verbose)
			display(frames, frame_num, arr[i], (faulted && is_filled));
	}

	/* set number of faults and references to 'return' array so miss
	 * rate can be calculated by the caller function */
	stats[0] = num_faults;
	stats[1] = num_refs;

}

/*
 * An extra page replacement algorithm implementation, known as the optimal page
 * replacement algorithm. Please note, this is not a realistic algorithm for OSs
 * because we don't know what's going to be called in the future. But, here we do
 * so I'm going to take advantage of it...The idea is to find the optimal replacement
 * page by looking into the future pages to be allocated. The optimal victim frame is 
 * the one that has a page reference the farthest away in the future. If there are multiple
 * frames that don't have a reference in the future, the optimal is the first frame that
 * has this property.
 * 
 *		:param arr: an array of pages to be allocated
 *		:param arr_size: the number of pages to be allocated
 *		:param frame_nun: the number of frames in physical memory
 *		:param stats: an array which will eventually store the number of page faults
 *					  for this run of the algorithm (at index 0), and the number of
 *					  references (at index 1). These are then used in by the caller
 *					  to calculate the miss rate
 *		:param verbose: "boolean" to indicate whether to run in verbose mode or not;
 *						if so, then each allocation process is displayed.
 */
void extra(int arr[], int arr_size, int frame_num, int stats[], int verbose) {

	/* initialize frame array to -1's */
	int frames[frame_num], i;
	for (i = 0; i < frame_num; i++) {
		frames[i] = -1;
	}

	/* initialize local variables: 
	 *	- res: stores the result of whether or not the page is already in memory
	 *	- num_faults: stores the current number of page faults that have occured
	 *	- num_refs: stores the current number of page references that have occured
	 * 	- faulted: temp boolean value to determine if the current page resulted in
	 *			   a fault or not
	 *	- is_filled: boolean value to determine if the frames are filled
	 *	- count: int to keep the current count of the page we're on
	 *	- num_allocated: stores the current number of pages allocated into a frame
	 */
	int res, faulted = 0, is_filled = 0, count = 0;
	int num_faults = 0, num_refs = 0, num_allocated = 0;

	/* for each page in arr...follow optimal replacement policy */
	for (i = 0; i < arr_size; i++) {

		/* see if page is already in frames */
		res = search(frames, frame_num, arr[i]);
		faulted = 0;
		count++;

		/* if the frame is full, count towards references */
		if (is_filled || num_allocated >= frame_num) {
			is_filled = 1;
			num_refs++;
		}

		/* is page is not in frame, replacement is needed */
		if (res == -1) {

			/* if the frames aren't filled, don't look for optimal, just
			 *  put it in the next free space */
			int index;
			if (!is_filled) 
				index = num_allocated;
			else
				index = find_opt(arr, arr_size, frames, frame_num, count);

			/* replace optimal element w/ page */
			frames[index] = arr[i];

			/* increment number allocated, and set faulted to true */
			num_allocated++;
			faulted = 1;
		}

		/* if the frame is full, and the page wasn't in frame then count towards faults*/
		if (is_filled && faulted) {
			num_faults++;
		}

		/* only print current operation if verbose mode is on */
		if (verbose)
			display(frames, frame_num, arr[i], (faulted && is_filled));
	}

	/* set number of faults and references to 'return' array so miss
	 * rate can be calculated by the caller function */
	stats[0] = num_faults;
	stats[1] = num_refs;
}