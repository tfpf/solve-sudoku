# Generate and Solve Sudoku Puzzles
I just wanted to rewrite an old C program of mine using a better style, and
then verify that it worked correctly.

## Compile
```
gcc -O2 -std=c11 -Wall -Wextra -o solve_sudoku solve_sudoku.c -lm
```

## Generate
```
./solve_sudoku 12
```
will print a sudoku puzzle of difficulty level 12 to standard output. Copy it
to a file, say, `sudoku.txt`. Alternatively, just redirect the output to
`sudoku.txt`.
```
./solve_sudoku 12 > sudoku.txt
```
A puzzle of difficulty level 0 (the minimum) is a solved puzzle, while one of
difficulty level 20 (the maximum) is an empty puzzle.

## Solve
```
./solve_sudoku sudoku.txt
```
will display the solution.

The solver will read from standard input if you don't provide any command line
arguments. Interestingly, this means you can pipe the output of the generator
to the solver
```
./solve_sudoku 12 | ./solve_sudoku
```
though that is probably of no use, since it doesn't show the puzzle first.

The solver is rather primitive. If it cannot fill any cell in the puzzle after
multiple attempts, it makes a random guess. If, as a result of this, the puzzle
becomes unsolvable, it bins everything and starts a fresh attempt. This has a
rather hilarious consequence: the solver can solve even an empty puzzle!
Indeed, that's how the generator works: it solves an empty puzzle and then
removes some of the numbers!
