# Arheisel.GOL_Hash
Hashing function using Conway's game of life I made back in 2016. I ran multiple 'brute force' tests and was never able to find collisions (depending on settings, I recommend leaving the defaults).

Compile on Linux using gcc

`gcc gol_hash.cpp -Werror -o gol_hash`

## Usage
`bash> ./gol_hash [options] input_string`

Options:

    -h  Help 
    
    -n  Number of passes for the Conway's algorithm (Default: 250)
    
    -k  Key strngth in bits (NOTE: only values that have a round SQRT are allowed)(Default: 256)
