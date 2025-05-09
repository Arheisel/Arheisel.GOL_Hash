#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <math.h>

unsigned long key_size = 256;
unsigned long grid_size = 16;
unsigned long column_size = 2;
unsigned long str_lenght = 32;

void fill_string(const char * source, char * dest ){ 
	//fills the string repeating itself up to 32 chars and adds a 0xDD as delimiter preventing 'a' and 'aa' to be the same

	int len = strlen(source) + 1;
	std::strcpy(dest, source);
	dest[len - 1] = 0xDD;
	for(int i = len; i < str_lenght; i++){
		dest[i] = dest[i - len];
	}
	//std::cout << dest << std::endl;
}

void fill_array(bool *lifeboard, char * str ){ 
	//travels trough the string converting every bit 
	//into a alive/dead state of every cell into the board

	for(int i = 0; i < str_lenght; i++){
		int mod = i%column_size;
		int x = (i - mod)/column_size;
		int y = mod * 8;
		for(int j = 0; j<8; j++){
			lifeboard[x*grid_size + y+j] = (str[i] >> j) & 1;
		}
	}
}

int neighbors(bool * lifeboard, int board, int x, int y){
	//gets all the neighbours of a cell counting the alive cells in its 3x3 square not counting itself

	int count = 0;
	for(int i = x-1; i <= x+1; i++){
		if( i >= 0 && i < grid_size){
			for(int j = y-1; j <= y+1; j++){
				if(j >= 0 && j < grid_size && (i != x || j != y) && lifeboard[board*key_size + i*grid_size + j]) count++;
			}
		}
	}
	return count;
}

void pass(bool * lifeboard, int &board){
	//applies the Conway's rules to every cell in the matrix.
	//it copies all the new states to a new matrix preventing unwanted results

	for(int i = 0; i < grid_size; i++){
		for(int j = 0; j < grid_size; j++){
			int nb = neighbors(lifeboard, board, i, j); //get the ammount of neighbours around a cell

			if(nb == 3){
				lifeboard[(!board)*key_size + i*grid_size + j] = true; //if it has 3 neighbors it lives
			}
			else if(nb < 2 || nb > 3){
				lifeboard[(!board)*key_size + i*grid_size + j] = false; //too many or too few it dies
			}
			else lifeboard[(!board)*key_size + i*grid_size + j] = lifeboard[board*key_size + i*grid_size + j]; //if not just copy the state into the new board
		}
	}
	board = !board; //changes the current matrix to the newly filled one
}

void scan(bool *lifeboard, int board, char * dest){ //gets every cell and makes a string out of it (reverse fill)
	for(int x = 0; x < grid_size; x++){
		for(int y = 0; y < column_size; y++){
			char out = 0;
			for(int z = 0; z < 8; z++){
				out = out << 1;
				if(lifeboard[board*key_size + x*grid_size + y*8+z]) out = out | 1;
			}
			dest[column_size*x + y] = out;
		}
	}
}

void xor_scan(char * source, char * dest){ //bitwise xor of the whole string
	for(int i = 0; i < str_lenght; i++){
		dest[i] = source[i] ^ dest[i];
	}
}

void print_hex(const char *s)
{
  for(int i = 0; i < str_lenght; i++)
    printf("%02x", (unsigned char) *s++);
  printf("\n");
}

void bad_arg(){
	std::cout << "Usage: gol_hash [options] string_to_hash" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "\t-h\tHelp (displays this text)" << std::endl;
	std::cout << "\t-n\tNumber of passes for the Conway's algorithm" << std::endl;
	std::cout << "\t-k\tKey strngth in bits (NOTE: only values that has a round SQRT are permitted.)" << std::endl;
	exit(1);
}

int main(int argc, char* argv[]){

	if(argc < 2){
		bad_arg();
	}

	int str_num = 0; //the number in argv where the input string is located
	int pass_num = 250; //the number of passes for the conway's algorithm

	for(int i = 1; i < argc; i++){
		if(strcmp(argv[i], "-n") == 0){
			pass_num = atoi(argv[++i]);	//gets the pass_num var and increments i by one
			continue;
		} 	
		else if(strcmp(argv[i], "-h") == 0){ 
			bad_arg(); 					//Help
			continue;
		}
		else if(strcmp(argv[i], "-k") == 0){
			unsigned long key = atoi(argv[++i]);	//Key Strength
			double square = sqrt(key);
			if(floor(square) != square){
				std::cout << "ERROR: Key Strength ins invalid (NOTE: only values that has a round SQRT are permitted.)" << std::endl;
				bad_arg();
			}
			key_size = key;
			grid_size = square;
			column_size = grid_size / 8;
			str_lenght = key / 8;
			continue;
		}
		else if(str_num == 0){
			str_num = i;							//the input string is the parameter with no options.
			continue;
		}
		else{
			std::cout << "ERROR: Too many arguments." << std::endl;
			bad_arg();
		}
	}

	if(str_num == 0){
		std::cout << "ERROR: No string provided." << std::endl;
		bad_arg();
	}

	//if(strlen(argv[str_num]) > str_lenght) std::cout << "WARNING: Max string length is " << str_lenght - 1 << " chars, the rest will be ignored." << std::endl;

	//std::string input;
	char * filled_str = (char*)malloc(sizeof(char) * str_lenght); //the input string already filled with 32 chars
	bool * lifeboard = (bool*)malloc(sizeof(bool) * 2 * grid_size * grid_size); //the board, consisting of 2 16x16 matrix that are swapped at every "pass"
	int current_board = 0; //the matrix that holds the newest values
	char * output = (char*)malloc(sizeof(char) * str_lenght); //the output string

	/*std::cout << "Enter string:";
	std::cin >> input;*/

	fill_string(argv[str_num], filled_str); //fills the string to 32 chars

	fill_array(lifeboard, filled_str); //puts the string into the first matrix
	scan(lifeboard, current_board, output); //fills the output string with the input. Due to the fill_array way of working, it gets mirrored.

	for(int i = 0; i < pass_num; i++){
		char * pass_output = (char*)malloc(sizeof(char) * str_lenght);

		pass(lifeboard, current_board);
		scan(lifeboard, current_board, pass_output);
		xor_scan(pass_output, output);
	}

	print_hex(output);

	return 0;
}