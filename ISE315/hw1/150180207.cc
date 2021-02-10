#include <vector>
#include <iostream>
#include <fstream>
#include <time.h>
#include <cstdlib>

using namespace std;

inline void swap(vector<int>& arr, int x, int y)
{
	int temp = arr[x];
	arr[x] = arr[y];
	arr[y] = temp;
}

void merge(vector<int>& arr, int p, int q, int r) {
	int i, j;
	int n1 = q - p + 1;
	int n2 = r - q;
	vector<int> L(n1);
	vector<int> R(n2);
	for (i = 0; i < n1; i++)
	{
		L[i] = arr[p + i];
	}
	for (j = 0; j < n2; j++)
	{
		R[j] = arr[q + j + 1];
	}

	i = 0; j = 0;
	int k = p;

	while (i < n1 && j < n2)
	{
		if (L[i] < R[j])
			arr[k++] = L[i++];
		else
			arr[k++] = R[j++];
	}
	while (i < n1) arr[k++] = L[i++];
	while (j < n2)arr[k++] = R[j++];
}

void merge_sort(vector<int>& arr, int p, int r) {
	if (p < r)
	{
		int q = (p + r) / 2;
		merge_sort(arr, p, q);
		merge_sort(arr, q + 1, r);
		merge(arr, p, q, r);
	}
}

void insertion_sort(vector<int>& arr, int N)
{
	int i, j, key;
	for (i = 0; i < N; i++)
	{
		key = arr[i];
		j = i - 1;
		while (i >= 0 && arr[j] > key)
		{
			swap(arr,j+1,j);
			j--;
		}
		arr[j + 1] = key;
	}
}

void bubble_sort(vector<int>& arr, int N)
{
	int i, j;
	for (i = 0; i < N-1; i++)
	{
		for (j = 0; j < N-i-1; j++)
		{
			if (arr[j] > arr[j+1])
			{
				swap(arr,j,j+1);
			}
		}
	}
}

void merge_sort_caller(vector<int>& arr, int N)
{
	merge_sort(arr,0,N-1);
}

typedef void(*Sort_Func)(vector<int>&, int);

int main(int argc,char* argv[])
{
	if (argc < 3)
	{
		cerr << "program requires 2 arguments, " << argc-1 << " given" << endl;
		exit(-1);
	}
	ifstream dataFile("data.txt");
	ofstream outFile("sorted.txt");
	int current,amount,i=0;
	vector<int> numbers;

	amount = atoi(argv[1]);
	while (dataFile >> current && ++i < amount)
		numbers.push_back(current);

	if (string(argv[2]) == "MergeSort")
		merge_sort_caller(numbers,amount);
	else if (string(argv[2]) == "InsertionSort")
		insertion_sort(numbers, amount);
	else if (string(argv[2]) == "BubbleSort")
		bubble_sort(numbers, amount);

	else if (string(argv[2]) == "csv")
	{
		vector<int> copyVector;
		clock_t t;
		ofstream resultFile("results.csv");
		Sort_Func funcs[] = { merge_sort_caller,insertion_sort,bubble_sort };

		resultFile << ",MergeSort,InsertionSort,BubbleSort" << endl;
		for (int i = 5000; i <= amount; i += 5000)
		{
			resultFile << i << " elements";
			for (int v =0; v < 3; v++)
			{
				copyVector = numbers;
				t = clock();
				funcs[v](copyVector,i);
				t = clock() - t;
				resultFile << "," << (float)t/CLOCKS_PER_SEC;
			}
			resultFile << endl;
		}
		return 0;
	}
	else
	{
		cerr << string(argv[2]) << " is not a valid second input, must be MergeSort BubbleSort or InsertionSort" << endl;
		exit(-1);
	}

	for (i = 0;i<amount;i++)
		outFile << numbers[i] << endl;
	return 0;
}
