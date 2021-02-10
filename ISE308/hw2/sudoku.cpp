#include <iostream>
#include <cstring>
#include <vector>
#include <bits/stdc++.h>
#include <ctime>

#include "input.h"

#define FULL 81

using namespace std;

extern int board[9][9];
extern int blocks[BLOCK_COUNT][6];

struct Sum_Pair
{
	int* positions;
	int len, max, curr;
	int filled;
	Sum_Pair(int m, int l, int pos[4]) : len(l), max(m), positions(pos), filled(0) {}
	Sum_Pair() {}
};

struct State
{
	char numbers[10][9]; //the 10 is to avoid some collusion with an other data (not ideal but it works)
	unsigned short domain_left[9][9]; //bitmap
	char filled;
	Sum_Pair sum_blocks[BLOCK_COUNT];
	State() {}
};

State* copy_state(State& input)
{
	State* cpy_board = new State;
	memcpy(cpy_board, &input, sizeof(State));
	return cpy_board;
}

void print_board(State& input)
{
	int i, v;
	for (i = 0; i < 9; i++) // hoizontal and vertical check
	{
		for (v = 0; v < 9; v++)
		{
			cout << (int)input.numbers[i][v] << ((v+1)%3==0? "  ":" ");
		}
		cout << endl;
		if ((i+1) % 3 == 0) cout << endl ;
	}
}

bool sum_check(State& input)
{
	int i, v,sum;
	for (i = 0; i < BLOCK_COUNT; i++)
	{
		if ((input.sum_blocks[i].curr == input.sum_blocks[i].max) && (input.sum_blocks[i].filled < input.sum_blocks[i].len)) return false;
		if ((input.sum_blocks[i].max- input.sum_blocks[i].curr>9) && (input.sum_blocks[i].filled+1 == input.sum_blocks[i].len)) return false;
		if ((input.sum_blocks[i].curr < input.sum_blocks[i].max) && (input.sum_blocks[i].filled == input.sum_blocks[i].len)) return false;
		if (input.sum_blocks[i].curr > input.sum_blocks[i].max) return false;
	}
	return true;
}

bool is_legal(State& input)
{
	char i, v, k, l;
	bool horizontal_buffer[10], vertical_buffer[10]; //for each num with one extra for empty cells

	for (i = 0; i < 9; i++) // hoizontal and vertical check
	{
		memset(horizontal_buffer, 0, 10 * sizeof(bool));
		memset(vertical_buffer, 0, 10 * sizeof(bool));
		for (v = 0; v < 9; v++)
		{
			//horizontal check
			if (input.numbers[i][v] && horizontal_buffer[input.numbers[i][v]]) return false;
			else horizontal_buffer[input.numbers[i][v]] = true;

			//vertical check
			if (input.numbers[v][i] && vertical_buffer[input.numbers[v][i]]) return false;
			else vertical_buffer[input.numbers[v][i]] = true;
		}
	}

	bool cube_buffer[10];

	for (i = 0; i < 3; i++) // 3x3 check
	{
		for (v = 0; v < 3; v++)
		{
			memset(cube_buffer, 0, 10 * sizeof(bool));
			for (k = 0; k < 3; k++) // 3x3 check
			{
				for (l = 0; l < 3; l++)
				{
					if (input.numbers[i * 3 + k][v * 3 + l] && cube_buffer[input.numbers[i * 3 + k][v * 3 + l]]) return false;
					else cube_buffer[input.numbers[i * 3 + k][v * 3 + l]] = true;
				}
			}
		}
	}

	return sum_check(input);
}

void fill_domain(State& input, int ver, int hor, int num)
{
	int i, v, k, j, veri, hori, diff;
	for (i = 0; i < 9; i++)
	{
		input.domain_left[ver][i] |= 1U << num;
		input.domain_left[i][hor] |= 1U << num;
	}
	veri = (ver / 3) * 3;
	hori = (hor / 3) * 3;
	for (i = 0; i < 3; i++)
	{
		for (v = 0; v < 3; v++)
		{
			input.domain_left[veri + i][hori + v] |= 1U << num;
		}
	}
	//filling the block domain
	int coord = ver * 9 + hor;
	for (i = 0; i < BLOCK_COUNT; i++)
	{
		for (v = 0; v < input.sum_blocks[i].len; v++)
		{
			if (input.sum_blocks[i].positions[v] == coord) //find the sum pair block related to the coord
			{
				input.sum_blocks[i].filled = 0; //resetting the fil value
				for (k = 0; k < input.sum_blocks[i].len; k++)
				{
					if (input.numbers[input.sum_blocks[i].positions[k] / 9][input.sum_blocks[i].positions[k] % 9])
						input.sum_blocks[i].filled += 1;
				}

				input.sum_blocks[i].curr += num;
				diff = input.sum_blocks[i].max - input.sum_blocks[i].curr;

				if ((input.sum_blocks[i].len- input.sum_blocks[i].filled)==1) //only one spot left
				{
					for (k = 0; k < input.sum_blocks[i].len; k++) // for each square in block
					{
						if (!input.numbers[input.sum_blocks[i].positions[k] / 9][input.sum_blocks[i].positions[k] % 9])
						{
							input.domain_left[input.sum_blocks[i].positions[k] / 9][input.sum_blocks[i].positions[k] % 9] = 0x3fe ^ (1U << diff);
						}
					}
					return;
				}

				if (diff < 9) //remove from other nodes domain lists
				{
					for (k = 0; k < input.sum_blocks[i].len; k++) // for each square in block
					{
						for (j = diff; j < 10; j++) // for each square in block
						{
							input.domain_left[input.sum_blocks[i].positions[k]/9][input.sum_blocks[i].positions[k] % 9] |= 1U << j; //set unavaliable to members in block
						}
					}
				}
				return;
			}
		}
	}
}



State& init_board() //initial board position
{
	int i, v;
	State* default_board = new State;
	memset(&(default_board->numbers), 0, 9 * 9 * sizeof(char)); //initialize to 0
	memset(&(default_board->domain_left), 0, 9 * 9 * sizeof(unsigned short)); //initialize to false
	default_board->filled = 0;

	for (i = 0; i < BLOCK_COUNT; i++)
	{
		default_board->sum_blocks[i]=Sum_Pair(blocks[i][0], blocks[i][1], &blocks[i][2]);
	}

	for (i = 0; i < 9; i++)
	{
		for (v = 0; v < 9; v++)
		{
			if (board[i][v])
			{
				default_board->numbers[i][v] = board[i][v];
				fill_domain(*default_board, i, v, board[i][v]);
				default_board->filled += 1;
			}
		}
	}

	return *default_board;
}

State comp_state;

int count_ones(int x)
{
	int sum = 0;
	while (x)
	{
		sum += x & 1U;
		x >>= 1;
	}
	return sum;
}

int MRV_value(int x)
{
	if (0 != comp_state.numbers[x / 9][x % 9]) return 0;
	return count_ones(comp_state.domain_left[x / 9][x % 9]);
}

int MRV(State& input)
{
	State* temp = copy_state(input);
	comp_state = *temp;
	int i;
	
	int best = 0,best_val=0,current;
	for (i = 0; i < 81; i++)
	{
		current = MRV_value(i);
		if (current > best_val)
		{
			best = i;
			best_val = current;
		}
	}
	
	delete temp;
	return best;
}

int lcv_cords;
bool LCV_compare(int x, int y)
{
	int ver = lcv_cords / 9;
	int hor = lcv_cords % 9;

	int y_const=0, x_const=0; //constraint amount
	int i, v;

	for (i = 0; i < 9; i++)
	{
		y_const += !(comp_state.domain_left[ver][i] & (1U << y));
		y_const += !(comp_state.domain_left[i][hor] & (1U << y));

		x_const += !(comp_state.domain_left[ver][i] & (1U << x));
		x_const += !(comp_state.domain_left[i][hor] & (1U << x));
	}

	int veri = (ver / 3) * 3;
	int hori = (hor / 3) * 3;
	for (i = 0; i < 3; i++)
	{
		for (v = 0; v < 3; v++)
		{
			x_const += !(comp_state.domain_left[veri + i][hori + v] & (1U << x));
			y_const += !(comp_state.domain_left[veri + i][hori + v] & (1U << y));
		}
	}
	return x_const > y_const;
}

vector<int> LCV(State& input, int cords)
{
	lcv_cords = cords;
	State* temp = copy_state(input);
	comp_state = *temp;
	vector<int> return_vector(10);
	int i;
	for (i = 0; i < 10; i++) return_vector[i] = i;
	sort(return_vector.begin(), return_vector.end(), LCV_compare);
	delete temp;
	return return_vector;
}

bool is_goal(State& s)
{
	int i, v;
	for (i = 0; i < 9; i++)
	{
		for (v = 0; v < 9; v++)
		{
			if (s.numbers[i][v] == 0) return false;
		}
	}
	return true;
}

time_t start;
int total_iter = 0;
bool solve(State& input)
{
	if (is_goal(input))
	{
		print_board(input);
		cout << "total iterations: " << total_iter << " total time: " << ((float)clock()-start) / CLOCKS_PER_SEC << " secs" << endl;
		exit(0);
	}
	int next_var = MRV(input);
	int ver = next_var / 9;
	int hor = next_var % 9;
	vector<int> next_dom = LCV(input, next_var);
	int i;

	for (i = 0; i < 10; i++)
	{
		if (next_dom[i] == 0) continue;
		total_iter++;
		input.numbers[ver][hor] = next_dom[i];

		if (is_legal(input))
		{
			State& temp = *copy_state(input);
			fill_domain(temp, ver, hor, next_dom[i]);
			if (solve(temp)) return true;
			delete& temp;
		}
		input.numbers[ver][hor] = 0;
	}
	return false;
}

int main()
{
	start = clock();
	solve(init_board());
	return 0;
}