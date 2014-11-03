// ----------------------------------------------------------------------------
//
//  Sudoku Puzzle Solver
//
//  Developer: Katrina Sitkovits
//  Email: katrina.sitkovits@gmail.com
//  Date: Nov. 3, 2014
//  Programming Langugage: C++
//
//  This Sudoku puzzle solver program accepts a CSV or white-space delimitted
//  puzzle and produces a solved CSV file in the same directory using the 
//  following command line execution:
//
//  $ sudoku_code_executable <input_filename> <output_filename> 
// 
//  The algorithm first simplifies the input puzzle based on trivial 
//  constraints requiring that no row, column, or box should have duplicate
//  values. Once it has exhausted the trivial approach, it performs recursive 
//  back-tracking to iteratively solve the puzzle. This two-phase approach
//  simplifies the computational complexity before performing a brute force 
//  recursion. The program has been tested with  puzzles of easy, medium, and
//  hard difficulties, and has been successful in all cases.
//
// ----------------------------------------------------------------------------


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;


// Import original, unsolved Sudoku puzzle from input file
// Input file may be .CSV or white-space delimited
// Input filename specified as command line argument #1
// e.g. $ sudoku_code_executable <input_filename> <output_filename>
void import_puzzle ( string puzzle_file, vector<vector<int> >& input_puzzle, vector<vector<int> >& solved_puzzle, vector<vector<vector<int> > >& square_list ) {
		
	// Read file
	std::ifstream fp;
	fp.open( puzzle_file.c_str() );
	if (fp.fail()) {
		cerr << "Can't open puzzle file" << endl;
		abort();
	}	
	int fp_value;
	vector<vector<int> > x (9, vector<int> (9,0));
	int row = 0;
	int col = 0;
	while(fp >> fp_value) {
		x[row][col] = fp_value;
		fp.get();
		col++;
		if (col==9) {
			col = 0;
			row++;
		}
	}
	fp.close();

	// Import puzzle into puzzle array & create list of possible choices for each box
	for (int row=0; row<9; row++) {
		for (int col=0; col<9; col++) {
			fp_value = x[row][col];
			if ( fp_value == 0 ) {
				for (int i=1; i<10; i++) {
					square_list[row][col].push_back(i);
				}
			} else {
				solved_puzzle[row][col] = fp_value;
				input_puzzle[row][col] = fp_value;
			}	
		}
	}
	
}

// Saves solved puzzle to file
// Output filename specified by user as command line argumnet #2
// e.g. $ sudoku_code_executable <input_filename> <output_filename>
void save_solved_puzzle ( string solution_file, vector<vector<int> >& final_puzzle )
{
	// Create solution puzzle file
	std::ofstream fp; 
	if (fp.fail()) {
		cerr << "Can't create solution puzzle file" << endl;
		abort();
	}	
	fp.open(solution_file.c_str()); 
	for (int row=0; row<9; row++) {
		for (int col=0; col<9; col++) {
			fp << final_puzzle[row][col];
			if ( col < 8 ) fp << ",";
		}
		fp << endl;
	}
	fp.close();
}

// Helper function for simplify_puzzle
void macro_box_iterate ( vector<vector<int> >& solved_puzzle, vector<vector<vector<int> > >& square_list, bool& solution_update ) {

	solution_update = false;

	for (int box_y = 0; box_y < 3; box_y++) {
		for (int box_x = 0; box_x < 3; box_x++) {

			// Macro Box
			for (int sub_y = 0; sub_y < 3; sub_y++) {
				for (int sub_x = 0; sub_x < 3; sub_x++) {		

					// Current square coordinates in full puzzle
					int puzzle_row = 3*box_y + sub_y;
					int puzzle_col = 3*box_x + sub_x;

					// Check puzzle row for current square			
					for(int col=0; col<9; col++) {
						for(int i=0; i<square_list[puzzle_row][puzzle_col].size(); i++){
							if ( square_list[puzzle_row][puzzle_col][i] == solved_puzzle[puzzle_row][col] ){
								square_list[puzzle_row][puzzle_col].erase(square_list[puzzle_row][puzzle_col].begin()+i);
							}
						}
					}
					
					// Check puzzle column for current square	
					for(int row=0; row<9; row++) {
						for(int i=0; i<square_list[puzzle_row][puzzle_col].size(); i++){
							if ( square_list[puzzle_row][puzzle_col][i] == solved_puzzle[row][puzzle_col] ){
								square_list[puzzle_row][puzzle_col].erase(square_list[puzzle_row][puzzle_col].begin()+i);
							}
						}
					}
					
					// Check current box for current square
					for(int row=0; row<3; row++){
						for(int col=0; col<3; col++){
							int abs_puzzle_row = 3*box_y + row;				
							int abs_puzzle_col = 3*box_x + col;
							if (abs_puzzle_row == puzzle_row && abs_puzzle_col == puzzle_col) continue;
							for(int i=0; i<square_list[puzzle_row][puzzle_col].size(); i++){
								if ( square_list[puzzle_row][puzzle_col][i] == solved_puzzle[abs_puzzle_row][abs_puzzle_col] ){
									square_list[puzzle_row][puzzle_col].erase(square_list[puzzle_row][puzzle_col].begin()+i);
								}
							}
						}
					}

					// Check for singular solutions for current square, and insert into solved_puzzle
					if ( square_list[puzzle_row][puzzle_col].size() == 1 ) {
						solved_puzzle[puzzle_row][puzzle_col] = square_list[puzzle_row][puzzle_col][0];
						square_list[puzzle_row][puzzle_col].clear();
						solution_update = true;

						// Clean square_lists for this particular box
						for (int search_sub_y = 0; search_sub_y < 3; search_sub_y++) {
							for (int search_sub_x = 0; search_sub_x < 3; search_sub_x++) {				
								int row = 3*box_y + search_sub_y;
								int col = 3*box_x + search_sub_x;
								for(int i=0; i<square_list[row][col].size(); i++){
									if ( solved_puzzle[puzzle_row][puzzle_col] == square_list[row][col][i] ) {
										square_list[row][col].erase(square_list[row][col].begin()+i);
									}
								}
							}
						}

					}			

				}
			}	
			// End Marco Box

			// Reduce lists for squares in current Macro Box, i.e. search for unique values
			for (int num=1; num<10; num++) {

				int count = 0;
				int found_puzzle_row = 0;
				int found_puzzle_col = 0;

				for (int sub_y = 0; sub_y < 3; sub_y++) {
					for (int sub_x = 0; sub_x < 3; sub_x++) {				
						int puzzle_row = 3*box_y + sub_y;
						int puzzle_col = 3*box_x + sub_x;
						for(int i=0; i<square_list[puzzle_row][puzzle_col].size(); i++){
							if ( num == square_list[puzzle_row][puzzle_col][i] ) {
								count++;
								found_puzzle_row = puzzle_row;
								found_puzzle_col = puzzle_col;
							}
						}
					}
				}

				if ( count == 1 ) {
					solved_puzzle[found_puzzle_row][found_puzzle_col] = num;
					square_list[found_puzzle_row][found_puzzle_col].clear();
					solution_update = true;
				}

			}
			// End Macro Box

		}
	}

}

// Reduces puzzle by solving trivial solutions based on row, column, and box constraints
void simplify_puzzle( vector<vector<int> >& solved_puzzle, vector<vector<vector<int> > >& square_list, int parent_iteration ) {

	bool done = false;
	int iteration = 1;
	while ( !done ) {
		bool solution_update;
		macro_box_iterate( solved_puzzle, square_list, solution_update );
		done = !solution_update;
		iteration++;
	}
	//print_combinations( solved_puzzle, square_list );
}

// Print current iteration of puzzle to standard output 
void visualize ( vector<vector<int> >& puzzle ) {
	for(int row=0; row<9; row++){
		for(int col=0; col<9; col++){
			cout << puzzle[row][col] << " ";
			if (col==2 || col==5) cout << "  ";
		}
		cout << endl;
		if (row==2 || row==5) cout << endl;
	}
}

// Print possible combinations in each puzzle square 
// Used for debugging purposes only
void print_combinations( vector<vector<int> >& solved_puzzle, vector<vector<vector<int> > >& square_list ) {

	cout << endl;
	// Print out combinations 
	for (int box_y = 0; box_y < 3; box_y++) {
		for (int box_x = 0; box_x < 3; box_x++) {
			cout << "Box [" << box_y << "][" << box_x << "]" << endl;
			for (int sub_y = 0; sub_y < 3; sub_y++) {
				for (int sub_x = 0; sub_x < 3; sub_x++) {	
						int puzzle_row = 3*box_y + sub_y;
						int puzzle_col = 3*box_x + sub_x;

						if ( solved_puzzle[puzzle_row][puzzle_col] == 0 ) {
							for(int i=0; i<square_list[puzzle_row][puzzle_col].size(); i++) { 
								cout << square_list[puzzle_row][puzzle_col][i];
							}
							cout << endl;
						} else {
							cout << solved_puzzle[puzzle_row][puzzle_col] << endl;	
						}

				}
			}
			cout << endl;
		}
	}

}

// Helper function for recursive_solve - checks for unsolved entries in puzzle
bool solve_check( vector<vector<int> >& solved_puzzle ) {
	for(int row=0; row<9; row++){
		for(int col=0; col<9; col++){
			if ( solved_puzzle[row][col] == 0 ) return false;
		}
	}
	return true;
}

// Helper function for recursive_solve - finds next unsolved square in puzzle
void find_empty_square( vector<vector<int> >& solved_puzzle, int& row, int& col ) {
	for(row=0; row<9; row++){
		for(col=0; col<9; col++){
			if ( solved_puzzle[row][col] == 0 ) return;
		}
	}
}

// Recursive back-tracking solver
// Explores every square option based on simplified puzzle values
// Back tracks when solution does not satisfy row, column, and box constraints
void recursive_solve ( vector<vector<int> >& final_puzzle, vector<vector<int> >& solved_puzzle, vector<vector<vector<int> > >& square_list, int parent_iteration ) {

	if ( solve_check(solved_puzzle) ) {
		// SOLVED!
		for(int row=0; row<9; row++){
			for(int col=0; col<9; col++){
				final_puzzle[row][col] = solved_puzzle[row][col];
			}
		}
		return;
	}

	else {

		// Go to next unsolved square in solved_puzzle
		int row_start, col_start;
		find_empty_square( solved_puzzle, row_start, col_start );

		// Copy puzzle for backtracking
		vector<vector<int> > temp_puzzle (9, vector<int> (9,0)); 
		for(int row=0; row<9; row++){
			for(int col=0; col<9; col++){
				temp_puzzle[row][col] = solved_puzzle[row][col];
			}
		}

		// Recurse through possible options in current square
		for(int i=0; i<square_list[row_start][col_start].size(); i++){

			temp_puzzle[row_start][col_start] = square_list[row_start][col_start][i];

			// Check constraints (row, column, box)
			bool constraint_pass = true;

			// Check temp_puzzle row
			for (int col=0; col<9; col++){
				if ( col == col_start ) continue;
				if ( temp_puzzle[row_start][col_start] == temp_puzzle[row_start][col] ) {
					constraint_pass = false;
				}
			}

			// Check temp_puzzle col
			for (int row=0; row<9; row++){
				if ( row == row_start ) continue;
				if ( temp_puzzle[row_start][col_start] == temp_puzzle[row][col_start] ) {
					constraint_pass = false;
				}
			}

			// Check current box
			int box_y = row_start/3;
			int box_x = col_start/3;
			for(int row=0; row<3; row++){
				for(int col=0; col<3; col++){
					int abs_puzzle_row = 3*box_y + row;				
					int abs_puzzle_col = 3*box_x + col;
					if (abs_puzzle_row == row_start && abs_puzzle_col == col_start) continue;
					else if ( temp_puzzle[row_start][col_start] == temp_puzzle[abs_puzzle_row][abs_puzzle_col] ) {
						constraint_pass = false;
					}
				}
			}

			if ( constraint_pass ){
				recursive_solve( final_puzzle, temp_puzzle, square_list, parent_iteration+1 );
			}
				
		}
			

	}
	

}


int main( int argc, char* argv[] ) {
	
	// Puzzle arrays
	vector<vector<int> > final_puzzle (9, vector<int> (9,0));  
	vector<vector<int> > input_puzzle (9, vector<int> (9,0));  
	vector<vector<int> > solved_puzzle (9, vector<int> (9,0));

	// Array for each puzzle square to track possible choices
	vector<vector<vector<int> > > square_list (9, vector<vector<int > >(9, vector<int> () )); 

	// Puzzle input file and output file defined by command line arguments
	// e.g. $ sudoku_code_executable <input_filename> <output_filename>
	string puzzle_file = argv[1];
	string solution_file = argv[2];

	// Read in puzzle from input file
	import_puzzle( puzzle_file, input_puzzle, solved_puzzle, square_list );

	// Simplify puzzle (until all trivial solutions are resolved)
	simplify_puzzle( solved_puzzle, square_list, 0 );

	// Recursively solve through back-tracking
	recursive_solve( final_puzzle, solved_puzzle, square_list, 1 );

	// Display input puzzle and solved puzzle
	cout << "\n\nInput Puzzle: \n\n";
	visualize( input_puzzle );
	cout << "\n\nSolved Puzzle: \n\n";
	visualize( final_puzzle );
	cout << "\n\n";

	// Save solved puzzle to output file
	save_solved_puzzle ( solution_file, final_puzzle );

	return 0;
}
