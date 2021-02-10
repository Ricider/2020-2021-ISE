#include <iostream>
#include <string>
#include <vector>
#include <string.h>

using namespace std;

int max(int f, int s) {return f>s?f:s;}

int totalCalc=0;

int lcs(char* f, char* s)
{
    if (*f==0 || *s==0) return 0;
	totalCalc++;
    if (*f==*s) return 1 + lcs(f+1,s+1);
    return max(lcs(f+1,s),lcs(f,s+1));
}

int dyn_lcs(string f, string s)
{
	int f_len = f.length(), s_len = s.length();
	vector<vector<int>> L(f_len + 1, vector<int>(s_len + 1));

	for (int i = 0; i <= f_len; i++)
	{
		for (int v = 0; v <= s_len; v++)
		{
			totalCalc++;
			if (i == 0 || v == 0)
				L[i][v] = 0;
			else if (f[i - 1] == s[v - 1])
				L[i][v] = L[i - 1][v - 1] + 1;
			else
				L[i][v] = max(L[i - 1][v], L[i][v - 1]);
		}
	}
	return L[f_len][s_len];
}

int lcs_calc(vector<vector<int>>& L,char* f, char* s, int fi, int si)
{
	if (*f == 0 || *s == 0) return L[fi][si] = 0;
	if (L[fi][si] != -1) return L[fi][si];
	totalCalc++;
	if (*f == *s) return L[fi][si] = 1 + lcs_calc(L, f + 1, s + 1, fi + 1, si + 1);
	return L[fi][si] = max(lcs_calc(L, f + 1, s, fi + 1, si), lcs_calc(L, f, s + 1, fi, si + 1));
}

int open_lcs(char* f, char* s)
{
	int f_len = strlen(f), s_len = strlen(s);
	vector<vector<int>> L(f_len + 1, vector<int>(s_len + 1,-1));
	return lcs_calc(L, f, s, 0, 0);
}

int main(int argc, char* argv[])
{
	if (argc==4)
	{

		string X = argv[1];
		string Y = argv[2];
		cout << "dynamic programming: " << dyn_lcs(X, Y) << endl;
		cout << "iterations " << totalCalc << endl;
	}
	else if (argc==5)
	{
		cout << "open recursion: " << open_lcs(argv[1], argv[2]) << endl;
		cout << "iterations " << totalCalc << endl;
	}
	else 
	{
		cout << "naive recursion: " << lcs(argv[1], argv[2]) << endl;
		cout << "iterations " << totalCalc << endl;
	}
}
