#include <stdio.h>
#include <omp.h>

#define SZ 12
#define size 2

int isAvailable(int puzzle[][SZ], int row, int col, int num)
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

int main()
{
    int i, j, k;
	

	int puzzle[5][SZ][SZ]={
					{{0, 0, 3, 6, 0, 0, 0, 0, 0, 7, 10, 0},
    					{0, 0, 9, 0, 0, 5, 12, 0, 1, 0, 0, 0},
    					{0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 5},
    					{7, 0, 5, 0, 0, 0, 8, 6, 0, 4, 3, 0},
    					{6, 1, 0, 0, 0, 9, 0, 0, 12, 0, 0, 11},
    					{3, 0, 8, 9, 0, 0, 11, 0, 7, 5, 0, 6},
    					{10, 0, 0, 0, 0, 1, 0, 0, 4, 0, 0, 0},
    					{9, 0, 0, 8, 0, 7, 4, 0, 0, 0, 5, 0},
    					{0, 12, 0, 0, 2, 0, 0, 0, 0, 6, 0, 7},
    					{8, 0, 12, 2, 4, 0, 0, 0, 0, 1, 0, 20},
    					{4, 0, 0, 1, 0, 0, 0 ,2, 11, 12, 9, 0},
    					{0, 3, 10, 0, 12, 0, 9, 0, 8, 2, 0, 0}},

					{{0, 0, 3, 6, 0, 0, 0, 0, 0, 7, 10, 0},
    					{0, 0, 9, 0, 0, 5, 12, 0, 1, 0, 0, 0},
    					{0, 0, 0, 0, 0, 0, 0, 10, 0, 0, 0, 5},
    					{7, 0, 5, 0, 0, 0, 8, 6, 0, 4, 3, 0},
    					{6, 1, 0, 0, 0, 9, 0, 0, 12, 0, 0, 11},
    					{3, 0, 8, 9, 0, 0, 11, 0, 7, 5, 0, 6},
    					{10, 0, 0, 0, 0, 1, 0, 0, 4, 0, 0, 0},
    					{9, 0, 0, 8, 0, 7, 4, 0, 0, 0, 5, 0},
    					{0, 12, 0, 0, 2, 0, 0, 0, 0, 6, 0, 7},
    					{8, 0, 12, 2, 4, 0, 0, 0, 0, 1, 0, 0},
    					{4, 0, 0, 1, 0, 0, 0 ,2, 11, 12, 9, 0},
    					{0, 3, 10, 0, 12, 0, 9, 0, 8, 2, 0, 0}}
						
						};

	#pragma omp parallel num_threads(2)
	{
		#pragma omp single private(k)
		{
			for(k = 0; k < 2; k++) 
			{
				#pragma omp task
				{

					if(fillSudoku(puzzle[k], 0, 0))
					{
						printf("\n+-----+-----+-----+\n");
						for(i=1; i<SZ+1; ++i)
						{
							for(j=1; j<SZ+1; ++j) printf("|%d", puzzle[k][i-1][j-1]);
							printf("|\n");
							if (i%3 == 0) printf("+-----+-----+-----+\n");
						}
					}
					else printf("\n\nNO SOLUTION\n\n");
				}
			}
		}
		
	}

    return 0;
}
