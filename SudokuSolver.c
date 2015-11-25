#include <stdio.h>
#include <omp.h>

#define SZ 12
#define SIZE 3000
#define MAX_PERM 8

int isAvailable(int puzzle[SZ][SZ], int row, int col, int num)
{
    int rowStart = (row/3) * 3;
    int colStart = (col/4) * 4;
    int i;

    for(i=0; i<SZ; ++i)
    {
        if (puzzle[row][i] == num) return 0;
        if (puzzle[i][col] == num) return 0;
        if (puzzle[rowStart + (i%3)][colStart + (i/4)] == num) return 0;
    }
    return 1;
}

int fillSudoku(int puzzle[SZ][SZ], int row, int col)
{
    int i;
    if(row<SZ && col<SZ)
    {
        if(puzzle[row][col] != 0)
        {
            if((col+1)<SZ) return fillSudoku(puzzle, row, col+1);
            else if((row+1)<SZ) return fillSudoku(puzzle, row+1, 0);
            else return 1;
        }
        else
        {
            for(i=0; i<SZ; ++i)
            {
                if(isAvailable(puzzle, row, col, i+1))
                {
                    puzzle[row][col] = i+1;
                    if((col+1)<SZ)
                    {
                        if(fillSudoku(puzzle, row, col +1)) return 1;
                        else puzzle[row][col] = 0;
                    }
                    else if((row+1)<SZ)
                    {
                        if(fillSudoku(puzzle, row+1, 0)) return 1;
                        else puzzle[row][col] = 0;
                    }
                    else return 1;
                }
            }
        }
        return 0;
    }
    else return 1;
}

int count = 0;

void copy_matrix(int dest[SZ][SZ], int source[SZ][SZ]) {
    int i, j;

    for (i = 0; i < SZ; i++)
        for (j = 0; j < SZ; j++)
            dest[i][j] = source[i][j];
}

void print_matrix(int puzzle[SZ][SZ]) {
    int i, j, k;
    static int c = 0;
    printf("%d\n", c++);
    printf("\n+-----+-----+-----+\n");
    //for(i=1; i<SZ+1; ++i)
    for(i=1; i<2; ++i)
    {
        for(j=1; j<SZ+1; ++j) printf("|%d", puzzle[i-1][j-1]);
        printf("|\n");
        if (i%3 == 0) printf("+-----+-----+-----+\n");
    }
}


void perm(int puzzle[SIZE][SZ][SZ], int row, int col) {
    int i;
    
    if (col == SZ) {
       copy_matrix(puzzle[count+1], puzzle[count]);
       count++;
    }
    else {
        if(puzzle[count][row][col] != 0)
            perm(puzzle, row, col + 1);
        else {
            for (i = 0; i < SZ; i++)
                if (isAvailable(puzzle[count], row, col, i + 1)) {
                    puzzle[count][row][col] = i + 1;
                    perm(puzzle, row, col + 1);
                }
            puzzle[count][row][col] = 0;
        }
    }

}

int search_zeros(int puzzle[][SZ]) {
    int i, j, line, c, cmax;
    line = c = cmax = 0;

    for (i = 0; i < SZ; i++) {
        c = 0;
        for (j = 0; j < SZ; j++)
            if (puzzle[i][j] == 0)
                c++;
        if (c > cmax && c < MAX_PERM) {
            cmax = c;
            line = i;
        }
    }
    return line;
}


int main()
{
    int i, j, k, line_zero;
	
    int puzzle[SIZE][SZ][SZ] = {
                       {{0, 0, 3, 6, 0, 0, 0, 0, 0, 7, 10, 0},
                        {0, 0, 9, 0, 0, 5, 12, 0, 1, 0, 0, 0},
                        {0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 5},
                        {7, 0, 5, 0, 0, 0, 8, 6, 0, 4, 3, 0},
                        {6, 1, 0, 0, 0, 9, 0, 0, 12, 0, 0, 11},
                        {3, 0, 8, 9, 0, 0, 11, 0, 7, 5, 0, 6},
                        {10, 0, 0, 0, 0, 1, 0, 0, 4, 0, 0, 0},
                        {9, 0, 0, 8, 0, 7, 4, 0, 0, 0, 5, 0},
                        {0, 0, 0, 0, 2, 0, 0, 0, 0, 6, 0, 7},
                        {8, 0, 12, 2, 4, 0, 0, 0, 0, 1, 0, 0},
                        {4, 0, 0, 1, 0, 0, 0 ,2, 11, 12, 9, 0},
                        {0, 3, 10, 0, 12, 0, 9, 0, 8, 2, 0, 0}},
                    };

   /* int puzzle[SIZE][SZ][SZ]={
                        {{8, 0, 0, 0, 9, 7, 0, 0, 10, 0, 6, 1},
                        {0, 0, 0, 7, 11, 0, 8, 5, 0, 4, 0, 3},
                        {0, 11, 10, 0, 0, 0, 0, 0, 0, 0, 8, 0},
                        {0, 8, 0, 0, 0, 0, 12, 0, 0, 0, 0, 2},
                        {6, 9, 0, 0, 0, 4, 0, 0, 0, 7, 0, 0},
                        {0, 2, 0, 0, 5, 0, 0, 11, 4, 12, 0, 0},
                        {0, 0, 8, 1, 2, 0, 0, 4, 0, 0, 5, 0},
                        {0, 0, 3, 0, 0, 0, 9, 0, 0, 0, 10, 11},
                        {11, 0, 0, 0, 0, 1, 0, 0, 0, 0, 12, 0},
                        {0, 3, 0, 0, 0, 0, 0, 0, 0, 2, 7, 0},
                        {12, 0, 4, 0, 3, 5, 0, 6, 1, 0, 0, 0},
                        {9, 1, 0, 6, 0, 0, 11, 7, 0, 0, 0, 10}}
                        };*/

    line_zero = search_zeros(puzzle[0]);
   perm(puzzle, line_zero, 0);
    //for(k = 0; k < count; k++)
    //    print_matrix(puzzle[k]);

    //printf("%d %d\n", count, line_zero);

    omp_set_num_threads(4);
    //count = 1;

    int gata = 0, steps = 0;

	#pragma omp parallel for private(k, i, j, steps)
	for(k = 0; k < count; k++) {
        steps++;
		if(!gata && fillSudoku(puzzle[k], 0, 0))
		{
			printf("COUNT: %d, Thread: %d, Steps: %d\n+-----+-----+-----+\n", k, omp_get_thread_num(), steps
);
			for(i=1; i<SZ+1; ++i)
			{
				for(j=1; j<SZ+1; ++j) printf("|%2d", puzzle[k][i-1][j-1]);
				printf("|\n");
				if (i%3 == 0) printf("+-----+-----+-----+\n");
			}
            //exit(0);
            #pragma omp atomic
            gata++;
		}
	}

    if (!gata)
        printf("No solution!\n");

    return 0;
}
