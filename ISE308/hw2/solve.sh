cp $1 input.h
g++ -O3 sudoku.cpp -o solve
./solve
rm input.h solve