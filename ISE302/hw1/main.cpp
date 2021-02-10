#include <pthread.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <math.h>
#include <vector>

/*
Developement environment: Ubuntu 18.04 on WSL (works on itu ssh servers)
To compile: g++ main.cpp -lpthread -o main
To run: ./main interval_min interval_max np nt
Example: ./main 101 200 2 2
*/

using namespace std;

pthread_mutex_t cout_mutex;

typedef struct
{
	int start, end;
	int thread_num, process_num;
}Bound;


bool is_prime(int n)
{
	int i, ceiling = sqrt(n)+1;
	for (i = 2; i < ceiling; i++)
		if (n % i == 0) return false;
	return true;
}


void* calculate_primes(void* t)
{
	Bound* bound = (Bound*)t;

	pthread_mutex_lock(&cout_mutex);
	cout << "Thread " << bound->process_num << "." << bound->thread_num << ": searching in " << bound->start << "-" << bound->end-1 << endl;;
	pthread_mutex_unlock(&cout_mutex);

	vector<int>* primes= new vector<int>();
	int i;
	for (i = bound->start; i < bound->end; i++)
		if (is_prime(i)) primes->push_back(i);
	pthread_exit((void*) primes);
}

int main(int argc, char* argv[])
{
	if (argc < 5)
	{
		cerr << "insufficent amount of arguments" << endl;
		exit(1);
	}
	int start_index = atoi(argv[1]);
	int end_index = atoi(argv[2]);
	int process_amount = atoi(argv[3]);
	int threads_per_process = atoi(argv[4]);
	int nums_per_thread = (end_index - start_index) / (process_amount * threads_per_process)+1;
	int processes_left = process_amount;
	if (pthread_mutex_init(&cout_mutex, NULL))
	{
		cout << "Mutex couldn't be initialized" << endl;
		exit(1);
	}
	cout << "Master: Started" << endl;
	while (process_amount --> 0)
	{
		int f = fork();
		if (f == -1)
		{
			cerr << "fork failed" << endl;
			exit(1);
		}
		else if (f == 0)
		{
			pthread_mutex_lock(&cout_mutex);
			cout << "Slave " << process_amount+1 << ": Started Interval " << start_index  << "-" << start_index+nums_per_thread * threads_per_process-1 << endl;
			pthread_mutex_unlock(&cout_mutex);

			void* status;
			int i, return_code;
			Bound bounds[threads_per_process];
			pthread_t thread[threads_per_process];
			pthread_attr_t attr;
			pthread_attr_init(&attr);
			pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

			for (i = 0; i < threads_per_process; i++)
			{
				bounds[i].start = start_index + nums_per_thread * i;
				bounds[i].end = start_index + nums_per_thread * (i + 1);
				bounds[i].process_num = process_amount + 1;
				bounds[i].thread_num = i + 1;

				if (bounds[i].end > end_index) bounds[i].end = end_index + 1;

				return_code = pthread_create(&thread[i], &attr, calculate_primes, (void*)&bounds[i]);
				if (return_code)
				{
					cerr << "error creating thread" << endl;
					exit(1);
				}
			}

			pthread_attr_destroy(&attr);
			vector<int> primes;
			for (i = 0; i < threads_per_process; i++)
			{
				return_code = pthread_join(thread[i],&status);
				if (return_code)
				{
					cerr << "error joining thread" << endl;
					exit(1);
				}

				primes.insert(primes.end(), ((vector<int>*)status)->begin(), ((vector<int>*)status)->end());
				delete ((vector<int>*)status);
			}

			pthread_mutex_lock(&cout_mutex);
			cout << "Slave " << process_amount+1 << " Done. Prime numbers are: ";
			for (i = 0; i < primes.size(); i++)
				cout << primes[i] << ",";
			cout << endl;
			pthread_mutex_unlock(&cout_mutex);
			exit(0);
		}
		else start_index += nums_per_thread*threads_per_process;
	}

	int stats;
	while (processes_left-- > 0)
		wait(&stats);

	return 0;
}