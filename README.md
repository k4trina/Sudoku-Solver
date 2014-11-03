## Sudoku Puzzle Solver

### Developer: Katrina Sitkovits
### Email: katrina.sitkovits@gmail.com
### Date: Nov. 3, 2014

Programming Langugage: C++

This Sudoku puzzle solver program accepts a CSV or white-space delimitted
puzzle and produces a solved CSV file in the same directory using the 
following command line execution:
```
$ sudoku_code_executable <input_puzzle_filename> <output_puzzle_filename> 
```
The algorithm first simplifies the input puzzle based on trivial 
constraints requiring that no row, column, or box should have duplicate
values. Once it has exhausted the trivial approach, it performs recursive 
back-tracking to iteratively solve the puzzle. This two-phase approach
simplifies the computational complexity before performing a brute force 
recursion. The program has been tested with  puzzles of easy, medium, and
hard difficulties, and has been successful in all cases.
