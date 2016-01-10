#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define LOGE(...) fprintf (stderr, __VA_ARGS__)
#define LOGI(...) fprintf (stdout, __VA_ARGS__)

#define SZ 12
#define ON_ROW 0
#define ON_COL 1
#define BLOCK_ROWS 3
#define BLOCK_COLS 4

pthread_mutex_t **s_mutex;

pthread_mutex_t gmutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
	int s_id;
	int min_row, max_row;
	int min_col, max_col;
} thread_args;

int puzzle[SZ][SZ]=
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
			{0, 3, 10, 0, 12, 0, 9, 0, 8, 2, 0, 0}};


int init_all(pthread_t **threads, thread_args **args)
{
	int i, j;
	int row_ind, col_ind;

	*threads = (pthread_t*)malloc(SZ  * sizeof(pthread_t));
	if (!*threads)
	{
		LOGE("Error allocating thread array!\n");
		return 1;
	}

	*args = (thread_args*)malloc(SZ * sizeof(thread_args));
	if (!*args)
	{
		LOGE("Error allocating args struct array!\n");
		return 1;
	}

	row_ind = col_ind = 0;
	for (i = 0; i < SZ; i++)
	{
		(*args)[i].s_id = i;
		(*args)[i].min_row = row_ind;
		(*args)[i].max_row = row_ind + BLOCK_ROWS;
		(*args)[i].min_col = col_ind;
		(*args)[i].max_col = col_ind + BLOCK_COLS;

		if ( (i+1) % 3 == 0 )
		{
			row_ind += BLOCK_ROWS;
		}

		if ( (i+1) % 3 )
		{
			col_ind += BLOCK_COLS;
		}
		else
		{
			col_ind = 0;
		}
	}

	s_mutex = (pthread_mutex_t**)malloc(2 * sizeof(pthread_mutex_t*));
	if (!s_mutex)
	{
		LOGE("Error allocating mutex array!\n");
		return 1;
	}

	for (i = 0; i < 2; i++)
	{
		s_mutex[i] = (pthread_mutex_t*)malloc(SZ * sizeof(pthread_mutex_t));
		if (!s_mutex[i])
		{
			LOGE("Error allocating mutex array!\n");
			return 1;
		}
	}

	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < SZ; j++)
		{
			if ( pthread_mutex_init(&s_mutex[i][j], NULL) )
			{
				LOGE("Error at pthread_mutex_init!\n");
				return 1;
			}
		}
	}

	return 0;
}

int destroy_all(pthread_t **threads, thread_args **args)
{
	int i, j;

	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < SZ; j++)
		{
			if ( pthread_mutex_destroy(&s_mutex[i][j]) )
			{
				LOGE("Error at pthread_mutex_destroy!\n");
				return 1;
			}
		}
	}

	for (i = 0; i < 2; i++)
	{
		free(s_mutex[i]);
	}

	free(s_mutex);
	free(*threads);
	free(*args);

	return 0;
}

int is_not_finished(int row, int col)
{
	int i, j, sum = 0;

	for (i = 0; i < SZ; i++)
	{
		for (j = 0; j < SZ; j++)
		{
			if (puzzle[i][j] == 0)
				return 1;
		}
	}

	return 0;
}

int is_available(int row, int col, int num)
{
	int rowStart = (row/3) * 3;
	int colStart = (col/4) * 4;
	int i;

	pthread_mutex_lock(&s_mutex[ON_ROW][row]);
	pthread_mutex_lock(&s_mutex[ON_COL][col]);
	for(i=0; i<SZ; ++i)
	{
		if (puzzle[row][i] == num) goto taken;
		if (puzzle[i][col] == num) goto taken;
		if (puzzle[rowStart + (i%3)][colStart + (i/3)] == num)  goto taken;
	}

not_taken:
	pthread_mutex_unlock(&s_mutex[ON_COL][col]);
	pthread_mutex_unlock(&s_mutex[ON_ROW][row]);
	return 1;

taken:
	pthread_mutex_unlock(&s_mutex[ON_COL][col]);
	pthread_mutex_unlock(&s_mutex[ON_ROW][row]);
	return 0;
}

int fill_block(int row, int col, int end_row, int end_col)
{
	int i;
	if (row < end_row && col < end_col)
	{
		if (puzzle[row][col] != 0)
		{
			if ( (col + 1) < end_col )
				return fill_block(row, col + 1, end_row, end_col);
			else if ( (row + 1) < end_row )
				return fill_block(row + 1, end_col - BLOCK_COLS, end_row, end_col);
			else
				return 1;
		}
		else
		{
			for (i = 0; i < SZ; ++i)
			{
				if(is_available(row, col, i+1))
				{
					puzzle[row][col] = i+1;
					if ( (col + 1) < end_col )
					{
						if (fill_block(row, col + 1, end_row, end_col))
							return 1;
						else
							puzzle[row][col] = 0;
					}
					else if ( (row + 1) < end_row )
					{
						if (fill_block(row+1, end_col - BLOCK_COLS, end_row, end_col))
							return 1;
						else
							puzzle[row][col] = 0;
					}
					else return 1;
				}
			}
		}
		return 0;
	}
	else
		return 1;
}

void* solve_sudoku(void *id)
{
	int i = 0;
	for(i = 0; i < 100; i++) {	
	printf("Id = %d, it = %d\n",  ((thread_args*)id)->s_id, i);
	if (is_not_finished( ((thread_args*)id)->min_row, ((thread_args*)id)->min_col))
	{
		fill_block(((thread_args*)id)->min_row,
				   ((thread_args*)id)->min_col,
				   ((thread_args*)id)->max_row,
				   ((thread_args*)id)->max_col);
	}}


	return NULL;
}

int main()
{
	int i, j;
	pthread_t *s_threads = NULL;
	thread_args *s_args = NULL;

	if (init_all(&s_threads, &s_args))
	{
		LOGE("Error during thread array init phase!\n");
		return 1;
	}

	for (i = 0; i < SZ; i++)
	{
		if ( pthread_create(&s_threads[i], NULL, solve_sudoku, &s_args[i]) )
		{

			LOGE("Error creating thread!\n");
			return 1;
		}
	}

	for (i = 0; i < SZ; i++)
	{
		if (pthread_join(s_threads[i], NULL))
		{
			LOGE("Error joining thread!\n");
			return 1;
		}
	}
	printf("+-----+-----+-----+-----+-----+-----+-----+-----+-----+\n");
	for(i=0; i<SZ; i++)
	{
		for(j=0; j<SZ; j++)
		{
			printf(" %d ", puzzle[i][j]);
			if (j%4 == 3)
				printf("\t");
		}

		printf("\n");
		if (i%3 == 2)
			printf("+-----+-----+-----+-----+-----+-----+-----+-----+-----+\n");
	}

	LOGI("Program successfully terminated!\n");
	return 0;
}
