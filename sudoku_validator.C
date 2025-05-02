#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9
#define SUBGRID_SIZE 3

int sudoku[SIZE][SIZE] = {
    {6, 2, 4, 5, 3, 9, 1, 8, 7},
    {5, 1, 9, 7, 2, 8, 6, 3, 4},
    {8, 3, 7, 6, 1, 4, 2, 9, 5},
    {1, 4, 3, 8, 6, 5, 7, 2, 9},
    {9, 5, 8, 2, 4, 7, 3, 6, 1},
    {7, 6, 2, 3, 9, 1, 4, 5, 8},
    {3, 7, 1, 9, 5, 6, 8, 4, 2},
    {4, 9, 6, 1, 8, 2, 5, 7, 3},
    {2, 8, 5, 4, 7, 3, 9, 1, 6}};

int valid[SIZE * 3] = {0};
typedef struct
{
    int row;
    int col;
} parameters;

void *check_row(void *param)
{
    int row = *((int *)param);
    int found[SIZE + 1] = {0};

    for (int j = 0; j < SIZE; j++)
    {
        int num = sudoku[row][j];
        if (num < 1 || num > SIZE || found[num])
        {
            valid[row] = 0;
            pthread_exit(NULL);
        }
        found[num] = 1;
    }

    valid[row] = 1;
    pthread_exit(NULL);
}

void *check_col(void *param)
{
    int col = *((int *)param);
    int found[SIZE + 1] = {0};

    for (int i = 0; i < SIZE; i++)
    {
        int num = sudoku[i][col];
        if (num < 1 || num > SIZE || found[num])
        {
            valid[SIZE + col] = 0;
            pthread_exit(NULL);
        }
        found[num] = 1;
    }

    valid[SIZE + col] = 1;
    pthread_exit(NULL);
}

void *check_subgrid(void *param)
{
    parameters *params = (parameters *)param;
    int row = params->row;
    int col = params->col;
    int found[SIZE + 1] = {0};

    for (int i = row; i < row + SUBGRID_SIZE; i++)
    {
        for (int j = col; j < col + SUBGRID_SIZE; j++)
        {
            int num = sudoku[i][j];
            if (num < 1 || num > SIZE || found[num])
            {
                valid[2 * SIZE + (row / SUBGRID_SIZE) * SUBGRID_SIZE + (col / SUBGRID_SIZE)] = 0;
                pthread_exit(NULL);
            }
            found[num] = 1;
        }
    }

    valid[2 * SIZE + (row / SUBGRID_SIZE) * SUBGRID_SIZE + (col / SUBGRID_SIZE)] = 1;
    pthread_exit(NULL);
}

int main()
{
    pthread_t threads[SIZE * 3];
    int thread_index = 0;

    // ایجاد تردهای بررسی سطرها
    for (int i = 0; i < SIZE; i++)
    {
        int *row = malloc(sizeof(int));
        *row = i;
        pthread_create(&threads[thread_index++], NULL, check_row, row);
    }

    // ایجاد تردهای بررسی ستون‌ها
    for (int j = 0; j < SIZE; j++)
    {
        int *col = malloc(sizeof(int));
        *col = j;
        pthread_create(&threads[thread_index++], NULL, check_col, col);
    }

    // ایجاد تردهای بررسی زیرشبکه‌ها
    for (int i = 0; i < SIZE; i += SUBGRID_SIZE)
    {
        for (int j = 0; j < SIZE; j += SUBGRID_SIZE)
        {
            parameters *params = malloc(sizeof(parameters));
            params->row = i;
            params->col = j;
            pthread_create(&threads[thread_index++], NULL, check_subgrid, params);
        }
    }

    // انتظار برای پایان تمام تردها
    for (int i = 0; i < SIZE * 3; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // بررسی نتایج
    int is_valid = 1;
    for (int i = 0; i < SIZE * 3; i++)
    {
        if (!valid[i])
        {
            is_valid = 0;
            break;
        }
    }

    if (is_valid)
    {
        printf("yes\n");
    }
    else
    {
        printf("no\n");
    }

    return 0;
}