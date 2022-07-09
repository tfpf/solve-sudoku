#define _POSIX_C_SOURCE 199309L

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/******************************************************************************
 * Read a sudoku puzzle into a two-dimensional array. Zeros are used to
 * represent blank cells. The format of the input file is the same as that
 * output by the program `generate_sudoku.py`.
 *
 * @param fname Name of the input file to read the puzzle from. If `NULL`, the
 *     puzzle will be read from standard input.
 * @param table Sudoku table.
 *
 * @return `true` if read successfully, `false` otherwise.
 *****************************************************************************/
bool read_sudoku(char const *fname, int table[][9])
{
    FILE *fptr = (fname == NULL) ? stdin : fopen(fname, "r");
    if(fptr == NULL)
    {
        return false;
    }

    for(int i = 0; i < 9; ++i)
    {
        for(int j = 0; j < 9; ++j)
        {
            char c;
            if(fscanf(fptr, " %c", &c) == 0 || (c != '-' && (c < '1' || c > '9')))
            {
                return false;
            }

            table[i][j] = (c == '-') ? 0 : c - '0';
        }
    }
    fclose(fptr);

    return true;
}

/******************************************************************************
 * Count the empty cells (i.e. the positions filled with zeros) in the sudoku.
 *
 * @param table Sudoku table.
 *
 * @return Number of empty cells.
 *****************************************************************************/
int number_of_empty_cells(int table[][9])
{
    int zeros = 0;
    for(int i = 0; i < 9; ++i)
    {
        for(int j = 0; j < 9; ++j)
        {
            if(table[i][j] == 0)
            {
                ++zeros;
            }
        }
    }

    return zeros;
}

/******************************************************************************
 * Display the sudoku table. Draw adjacent blocks using different background
 * colours if the output is going to a terminal.
 *
 * @param table Sudoku table.
 *****************************************************************************/
void show(int table[][9])
{
    bool stdout_is_terminal = isatty(fileno(stdout));
    for(int i = 0; i < 9; ++i)
    {
        for(int j = 0; j < 9; ++j)
        {
            printf("  ");
            bool colour = !((i / 3 + j / 3) % 2) && stdout_is_terminal;
            if(colour && j % 3 == 0)
            {
                printf("\033[37;100m");
            }
            (table[i][j] == 0) ? printf("-") : printf("%d", table[i][j]);
            if(colour && j % 3 == 2)
            {
                printf("\033[0m");
            }
        }
        printf("\n");
    }
}

/******************************************************************************
 * Check whether the number given may be placed in the given row.
 *
 * @param table Sudoku table.
 * @param row Number from 0 to 8.
 * @param num Number from 1 to 9.
 *
 * @return `true` if `num` isn't in the row indexed `row`, else `false`.
 *****************************************************************************/
bool allowed_in_row(int table[][9], int row, int num)
{
    for(int j = 0; j < 9; ++j)
    {
        if(table[row][j] == num)
        {
            return false;
        }
    }

    return true;
}

/******************************************************************************
 * Check whether the number given may be placed in the given column.
 *
 * @param table Sudoku table.
 * @param col Number from 0 to 8.
 * @param num Number from 1 to 9.
 *
 * @return `true` if `num` isn't in the column indexed `col`, else `false`.
 *****************************************************************************/
bool allowed_in_col(int table[][9], int col, int num)
{
    for(int i = 0; i < 9; ++i)
    {
        if(table[i][col] == num)
        {
            return false;
        }
    }

    return true;
}

/******************************************************************************
 * Check whether the number given may be placed in the indicated block. Scan
 * only those positions whose row and column indices differ from `row` and
 * `col` respectively, since the other positions will be checked by
 * `allowed_in_row` and `allowed_in_col`.
 *
 * @param table Sudoku table.
 * @param row Number from 0 to 8.
 * @param col Number from 0 to 8.
 * @param num Number from 1 to 9.
 *
 * @return `true` if `num` isn't in the block which contains the cell
 *     with row index `row` and column index `col` at a position with row
 *     index different from `row` and column index different from `col`, else
 *     `false`.
 *****************************************************************************/
bool allowed_in_block(int table[][9], int row, int col, int num)
{
    // Find out where the block indicated by `row` and `col` begins.
    int block_row_start = row - row % 3;
    int block_col_start = col - col % 3;

    for(int i = block_row_start; i < block_row_start + 3; ++i)
    {
        for(int j = block_col_start; j < block_col_start + 3; ++j)
        {
            if(i != row && j != col && table[i][j] == num)
            {
                return false;
            }
        }
    }

    return true;
}

/******************************************************************************
 * Check whether the number given may be placed at the given position. At the
 * time of calling this function, it must be true that `table[row][col] == 0`.
 *
 * @param table Sudoku table.
 * @param row Number from 0 to 8.
 * @param col Number from 0 to 8.
 * @param num Number from 1 to 9.
 *
 * @return `true` if `num` may appear at row index `row` and column index
 *     `col`, else `false`.
 *****************************************************************************/
bool allowed_at_position(int table[][9], int row, int col, int num)
{
    return allowed_in_row(table, row, num)
           && allowed_in_col(table, col, num)
           && allowed_in_block(table, row, col, num);
}

/******************************************************************************
 * Check how many numbers are allowed at the given position. If only a single
 * number is allowed, assign it at that position. Otherwise, do nothing.
 * (Alternatively, assign a permitted number at that position randomly, if told
 * to do so.) At the time of calling this function, it must be true that
 * `table[row][col] == 0`.
 *
 * @param table Sudoku table.
 * @param row Number from 0 to 8.
 * @param col Number from 0 to 8.
 * @param assign_random Whether to assign `table[row][col]` randomly or not.
 *****************************************************************************/
void select_allowed(int table[][9], int row, int col, bool assign_random)
{
    int allowed;
    int count_allowed = 0;
    for(int num = 1; num <= 9; ++num)
    {
        if(allowed_at_position(table, row, col, num))
        {
            allowed = num;
            ++count_allowed;
        }
    }

    if(count_allowed == 1 || (count_allowed >= 1 && assign_random))
    {
        // GCC emits a superfluous warning for this line when compiling with
        // optimisations and warnings enabled.
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
        table[row][col] = allowed;
        #pragma GCC diagnostic pop
    }
}

/******************************************************************************
 * Check how many positions the given number can be placed at in the given row.
 * If there is only one position, assign it there. Otherwise, do nothing.
 *
 * @param table Sudoku table.
 * @param row Number from 0 to 8.
 * @param num Number from 1 to 9.
 *****************************************************************************/
void select_possible_in_row(int table[][9], int row, int num)
{
    if(!allowed_in_row(table, row, num))
    {
        return;
    }

    int possible_col;
    int count_possible = 0;
    for(int j = 0; j < 9; ++j)
    {
        if(table[row][j] == 0 && allowed_in_col(table, j, num) && allowed_in_block(table, row, j, num))
        {
            possible_col = j;
            ++count_possible;
        }
    }

    if(count_possible == 1)
    {
        table[row][possible_col] = num;
    }
}

/******************************************************************************
 * Check how many positions the given number can be placed at in the given
 * column. If there is only one position, assign it there. Otherwise, do
 * nothing.
 *
 * @param table Sudoku table.
 * @param col Number from 0 to 8.
 * @param num Number from 1 to 9.
 *****************************************************************************/
void select_possible_in_col(int table[][9], int col, int num)
{
    if(!allowed_in_col(table, col, num))
    {
        return;
    }

    int possible_row;
    int count_possible = 0;
    for(int i = 0; i < 9; ++i)
    {
        if(table[i][col] == 0 && allowed_in_row(table, i, num) && allowed_in_block(table, i, col, num))
        {
            possible_row = i;
            ++count_possible;
        }
    }

    if(count_possible == 1)
    {
        table[possible_row][col] = num;
    }
}

/******************************************************************************
 * Check how many positions the given number can be placed at in the indicated
 * block. If there is only one position, assign it at there. Otherwise, do
 * nothing.
 *
 * @param table Sudoku table.
 * @param row Row index of the first cell of a block. One of 0, 3 and 6.
 * @param col Column index of the first cell of a block. One of 0, 3 and 6.
 * @param num Number from 1 to 9.
 *****************************************************************************/
void select_possible_in_block(int table[][9], int row, int col, int num)
{
    if(!allowed_in_block(table, row, col, num))
    {
        return;
    }

    int possible_row, possible_col;
    int count_possible = 0;
    for(int i = row; i < row + 3; ++i)
    {
        for(int j = col; j < col + 3; ++j)
        {
            if(table[i][j] == 0 && allowed_at_position(table, i, j, num))
            {
                possible_row = i;
                possible_col = j;
                ++count_possible;
            }
        }
    }

    if(count_possible == 1)
    {
        table[possible_row][possible_col] = num;
    }
}

/******************************************************************************
 * Check how many possible positions the given number may be placed at. If only
 * a single position is present in a row or column or block, assign it at
 * that/those position/positions. Otherwise, do nothing.
 *
 * @param table Sudoku table.
 * @param num Number from 1 to 9.
 *****************************************************************************/
void select_possible(int table[][9], int num)
{
    for(int i = 0; i < 9; ++i)
    {
        select_possible_in_row(table, i, num);
    }

    for(int j = 0; j < 9; ++j)
    {
        select_possible_in_col(table, j, num);
    }

    for(int i = 0; i < 9; i += 3)
    {
        for(int j = 0; j < 9; j += 3)
        {
            select_possible_in_block(table, i, j, num);
        }
    }
}

/******************************************************************************
 * Do one pass of the table, filling cells wherever possible.
 *
 * @param table Sudoku table.
 * @param assign_random Whether to fill a cell randomly. At most one cell shall
 *     be filled randomly per function call.
 *****************************************************************************/
void single_pass(int table[][9], bool assign_random)
{
    for(int i = 0; i < 9; ++i)
    {
        for(int j = 0; j < 9; ++j)
        {
            if(table[i][j] == 0)
            {
                select_allowed(table, i, j, assign_random);
                assign_random = false;
            }
        }
    }

    for(int num = 1; num <= 9; ++num)
    {
        select_possible(table, num);
    }
}

/******************************************************************************
 * Solve the sudoku puzzle.
 *
 * @param table Sudoku table.
 *****************************************************************************/
void solve(int table[][9])
{
    int prev_zeros = 81;
    bool assign_random = false;
    while(true)
    {
        int zeros = number_of_empty_cells(table);
        if(zeros == 0)
        {
            break;
        }

        // If no cells could be filled in an iteration, ask for a number to
        // be filled in randomly. If that didn't work, give up.
        if(zeros == prev_zeros)
        {
            if(assign_random)
            {
                break;
            }
            assign_random = true;
        }
        single_pass(table, assign_random);
        prev_zeros = zeros;
    }
}

/******************************************************************************
 * Check whether the cells in the sudoku table are filled correctly.
 *
 * @param table Sudoku table.
 *****************************************************************************/
bool valid(int table[][9])
{
    for(int i = 0; i < 9; ++i)
    {
        for(int j = 0; j < 9; ++j)
        {
            if(table[i][j] < 1 || table[i][j] > 9)
            {
                return false;
            }
        }
    }

    int expected_frequency[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    for(int i = 0; i < 9; ++i)
    {
        int frequency[9] = {0};
        for(int j = 0; j < 9; ++j)
        {
            ++frequency[table[i][j] - 1];
        }
        if(memcmp(frequency, expected_frequency, sizeof frequency))
        {
            return false;
        }
    }
    for(int j = 0; j < 9; ++j)
    {
        int frequency[9] = {0};
        for(int i = 0; i < 9; ++i)
        {
            ++frequency[table[i][j] - 1];
        }
        if(memcmp(frequency, expected_frequency, sizeof frequency))
        {
            return false;
        }
    }
    for(int i = 0; i < 9; i += 3)
    {
        for(int j = 0; j < 9; j += 3)
        {
            int frequency[9] = {0};
            for(int k = i; k < i + 3; ++k)
            {
                for(int l = j; l < j + 3; ++l)
                {
                    ++frequency[table[k][l] - 1];
                }
            }
            if(memcmp(frequency, expected_frequency, sizeof frequency))
            {
                return false;
            }
        }
    }

    return true;
}

/******************************************************************************
 * Main function.
 *****************************************************************************/
int main(int const argc, char const *argv[])
{
    int table[9][9];
    char const *fname = (argc < 2) ? NULL : argv[1];
    if(!read_sudoku(fname, table))
    {
        fprintf(stderr, "Could not read the puzzle.\n");
        return EXIT_FAILURE;
    }

    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);
    solve(table);
    clock_gettime(CLOCK_REALTIME, &end);
    int long delay_micro = (int long)(end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_nsec - begin.tv_nsec) / 1000;

    show(table);
    if(!valid(table))
    {
        fprintf(stderr, "Could not solve.\n");
        return EXIT_FAILURE;
    }

    printf("Solved in %ld μs (real time).\n", delay_micro);

    return EXIT_SUCCESS;
}
