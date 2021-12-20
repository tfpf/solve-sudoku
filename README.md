# Generate and Solve Sudoku Puzzles

## Generate
```
python3 generate_sudoku.py
```
will print a sudoku puzzle to standard output. Copy it to a file, say,
`sudoku.txt`.

## Solve
```
gcc -O2 -std=c11 -Wall -Wextra -o solve_sudoku solve_sudoku.c
./solve_sudoku sudoku.txt
```
will display the solution. The solver is quite primitive; it can only work out
easy sudoku puzzles. I will update it when I can.

