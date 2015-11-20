/* 
 * Lab Assignment #2 : The Cache Lab
 * CSCI-UA 201.003
 * Author: Peter Mountanos (pjm419)
*/

#include <stdlib.h>
#include <stdio.h>


#define N       1024
#define DIM     512
#define DIM2    128
#define LARGE   10000

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

int list[LARGE];

int level_1();
void level_2();
void level_3();
void level_4();

/* Do not change anything above this line */


/***********************************************/

/*
 * Level 1 is a simple function which increases each position in a 
 * matrix by adding 2 to it, and then multiplying the sum by 2. 
 * To improve the number of cache misses, all one needed to do is
 * flip the order of the loops. This improves spatial locality. 
 * This is because C stores arrays as row-major order, so by  
 * accessing the array row by row, instead of column by column, 
 * we allow the inner loop to have stride-1 access pattern. Note,
 * if the entire array can fit in cache, there would be no difference
 * between the two functions (here, this is not the case though).
 */
int level_1()
{
  int B[N][N];
  
  int i, j;
  
  for(i = 0; i < N; i++)
    for(j = 0; j < N; j++)
      B[i][j] = 2*(B[i][j] + 2);
   
  /* Do NOT change the the next few lines till the end of this function */  
  i = random () % N;
  j = random () % N;
  
  return(B[i][j]);
  
}

/***********************************************/

/*
 * Level 2 is a function that fills the diagonals of
 * the matrix with the sum of the elements in the same
 * column as the diagonal. To improve the number of cache
 * reads and writes, it's actually better to split this into
 * two different loops. The first initializes the diagonals to
 * 0 (stride-1 access pattern). Doing this first allows us to
 * give our inner loop of second set of loops stride-1 access
 * patterns as well. Instead of going column by column, we read
 * row by row, but then write to the appropriate diagonal based on
 * the column. 
 */
void level_2()
{
  int temp = 0;
  int i, j;
  int A[DIM][DIM];
  int B[DIM][DIM];
  
  // initialize diagonal indices to 0
  for(i = 0; i < DIM; i++)
    A[i][i] = 0;

  // update 
  for(i = 0; i < DIM; i++)
  {
    for( j = 0; j < DIM; j++)
      A[j][j] += B[i][j];
  } 
  
  /* Do NOT change the the next few lines till the end of this function */  
  i = random () % DIM;
  for(j = 0; j < DIM; j++)
    temp += B[i][j];
  
  if( temp == A[i][i] )
    printf("level 2 ... completed!\n");
}

/***********************************************/

/*
 * Level 3 is reverses the order of each row in the array. To improve its
 * cache read & write misses, I actually switched the order of the way the 
 * swaps happen. It performs the swaps based on the row by row reading, so it
 * gives the inner loop a stride-1 access pattern. 
 */
void level_3()
{
  int i, j;
  int temp;
  int c[N][N];
  
  for( i = 0; i < N; i++)
    for( j = 0; j < N >> 1; j++)
    {
      temp = c[i][j];
      c[i][j] = c[i][N-j];
      c[i][N-j] = temp;
    }
      
/* Do NOT change the the next few lines till the end of this function */  
    printf("level 3 ... completed!\n");

}

/***********************************************/

/*
 * Level 4 is an implementation of bubble sort. In order to optimize
 * bubble sort, the key is to worry about temporal locality. Bubble
 * sort already has good spatial locality (comparing two adjacent items).
 * So, to do this, one should implement a bidirectional bubble sort (also
 * know as shaker sort). The idea of this is to first sort one way, and then
 * go back sorting the other way. This allows for some improvement in temporal
 * locality at the ends of the array (each part loaded into cache gets to be
 * used twice). 
 */
void level_4()
{
    int i, j, temp, sorted;
    int k = LARGE - 1;

    for (i = 0; i < k; ) {

        // start at end and sort to front of array
        for (j = k; j > i; j--)
        {
            if (list[j] < list[j-1])
            {
                temp = list[j];
                list[j] = list[j-1];
                list[j-1] = temp;
                //l = j;
            }
        }

        i++;
        sorted = 1;

        // start at front and sort to end of array
        for (j = i ; j < k; j++)
        {
            if (list[j+1] < list[j])
            {
                temp = list[j];
                list[j] = list[j+1];
                list[j+1] = temp;
                sorted = 0;
            }
        }
        if (sorted) break;
        k--;
        
/*      if (sorted) return; */
    }
      

    /* Do NOT change the the next few lines till the end of this function */  
    i = random () % LARGE;
    if( list[0] < list[i] )
        printf("level 4 ... completed!\n");
}

/***********************************************/

/*
 * Level 5 is a matrix multiplication function. To cache optimize Level 5, 
 * improving spatial locality actually isn't enough. One must also improve 
 * temporal locality. This is done by a technique known as blocking. The idea 
 * of blocking is to organize the matrix into "chunks", called blocks. Here, we
 * load the block into L1 cache, and do all of the necessary reading & writing, 
 * and then discard the block and load the next one into cache. Note, depending 
 * on the b-size set, you can tune performance. So, if I set bsize to 1, I'll 
 * minimize the number of write misses (36), but that number of read misses is
 * compromised (532,481). But, I set the bsize to 15, and overall reduces the
 * total number of cache misses: read (41,414) & write (3,845).
 */
void level_5() {

    // declare arrays
    double A[DIM2*DIM2];
    double B[DIM2*DIM2];
    double C[DIM2*DIM2];

    // declare local vars
    int n = DIM2;
    int i, j, k;
    int ii, jj, kk;
    int bsize = 15;
    double sum;


    for (jj = 0; jj < n; jj += bsize) {
        for (i = 0; i < n; i++) {
            for (j = jj; j < MIN(jj + bsize, n); j++) {
                C[i + j * n] = 0.0;
            }
        }

        for (kk = 0; kk < n; kk += bsize) {
            for (i = 0; i < n; i++) {
                for (k = kk; k < MIN(kk + bsize, n); k++) {
                    sum = 0.0;
                    for (j = jj; j < MIN(jj + bsize, n); j++) {
                        sum += A[i + k * n] * B[k + j *n];
                    }
                    C[i + j * n] += sum;
                }
            }
        }
    }


    /* Do NOT change the the next few lines till the end of this function */ 
    printf("level 5 ... completed!\n");

}

/***********************************************/



/* Do not change anything below this line */

int main(int argc, char *argv[])
{
  int i, j;
 
  for(i = 0; i < LARGE; i++)
    list[i] = random() % LARGE;
  
  if( level_1() % 2 == 0)
    printf("level 1 ... completed!\n");
  
  level_2();
  level_3();
  level_4();
    
  for(i = 0; i < LARGE; i++)
    list[i] = random() % LARGE;

  level_5( );
  
  
  return 1;
}
