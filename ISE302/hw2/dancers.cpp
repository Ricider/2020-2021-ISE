#include <pthread.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/sem.h>
#include <vector>
#include <cstdio>

using namespace std;

/*********************************************************************************
Developement environment: Ubuntu 18.04 on WSL (works on itu ssh servers)
To compile: g++ dancers.cpp -lpthread -o dancers
To run: ./dancers leader_count follower_count
Example: ./dancers 2 2
*********************************************************************************/

#define STAGE_SEMKEY 2435788345
int stage_sem_id;

int followers_waiting = 0;
int leaders_waiting = 0;

pthread_mutex_t f_counter_mutex, l_counter_mutex;

void sem_signal(int semid, int sem_num, int val)
{
	struct sembuf semaphore;
	semaphore.sem_num = sem_num;
	semaphore.sem_op = val;
	semaphore.sem_flg = 1;
	semop(semid, &semaphore, 1);
}

void sem_wait(int semid, int sem_num, int val)
{
	struct sembuf semaphore;
	semaphore.sem_num = sem_num;
	semaphore.sem_op = -1 * val;
	semaphore.sem_flg = 1;
	semop(semid, &semaphore, 1);
}

void* leader_thread(void* t)
{
	long id = (long)t;
	usleep(rand() % 2000000);

	pthread_mutex_lock(&l_counter_mutex);
	leaders_waiting++;
	pthread_mutex_unlock(&l_counter_mutex);

	sem_signal(stage_sem_id, 0, 1);

	pthread_mutex_lock(&f_counter_mutex);
	if (followers_waiting) cout << "Leader " << id << ": " << followers_waiting << " followers are waiting, so I signal the next follower in the queue\n";
	else cout << "Leader " << id << ": No available follower, so I wait. There are other " << leaders_waiting - 1 << " leaders waiting.\n";
	pthread_mutex_unlock(&f_counter_mutex);

	sem_wait(stage_sem_id, 1, 1);
	leaders_waiting--;
	printf("Leader %ld: We are dancing together now.\n",id);
	sem_signal(stage_sem_id, 2, 1);
	sem_wait(stage_sem_id, 3, 1);

	printf("Leader %ld: I leave the stage now.\n", id); //cout was not working atomicly for me so instead of using a mutex I just did this
	pthread_exit(NULL);
}

void* follower_thread(void* t)
{
	long id = (long)t;
	usleep(rand() % 2000000);

	pthread_mutex_lock(&f_counter_mutex);
	followers_waiting++;
	pthread_mutex_unlock(&f_counter_mutex);

	sem_signal(stage_sem_id, 1, 1);

	pthread_mutex_lock(&l_counter_mutex);
	if (leaders_waiting) cout << "Follower " << id << ": " << leaders_waiting << " leaders are waiting, so I signal the next leader in the queue\n";
	else cout << "Follower " << id << ": No available leader, so I wait. There are other " << followers_waiting - 1 << " followers waiting.\n";
	pthread_mutex_unlock(&l_counter_mutex);

	sem_wait(stage_sem_id, 0, 1);
	followers_waiting--;
	printf("Follower %ld: We are dancing together now.\n", id);
	sem_signal(stage_sem_id, 3, 1);
	sem_wait(stage_sem_id, 2, 1);

	printf( "Follower %ld: I leave the stage now.\n",id);
	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	int leader_num = atoi(argv[1]);
	int follower_num = atoi(argv[2]);
	long return_code, i;

	srand((unsigned int)time(0)); //randomize random number
	pthread_mutex_init(&f_counter_mutex, NULL);
	pthread_mutex_init(&l_counter_mutex, NULL);

	stage_sem_id = semget(STAGE_SEMKEY, 4, IPC_CREAT | 0700);
	for (i = 0; i < 4; i++) semctl(stage_sem_id, i, SETVAL, 0);

	pthread_t leader_threads[leader_num];
	pthread_t follower_threads[follower_num];

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	for (i = 0; i < leader_num; i++)
	{
		return_code = pthread_create(&leader_threads[i], &attr, leader_thread, (void*)i);
		if (return_code)
		{
			cerr << "error creating leader thread\n";
			exit(1);
		}
	}
	for (i = 0; i < follower_num; i++)
	{
		return_code = pthread_create(&follower_threads[i], &attr, follower_thread, (void*)i);
		if (return_code)
		{
			cerr << "error creating follower thread\n";
			exit(1);
		}
	}

	pthread_attr_destroy(&attr);

	for (i = 0; i < leader_num; i++)
	{
		return_code = pthread_join(leader_threads[i], NULL);
		if (return_code)
		{
			cerr << "error joining leader thread\n";
			exit(1);
		}
	}
	for (i = 0; i < follower_num; i++)
	{
		return_code = pthread_join(follower_threads[i], NULL);
		if (return_code)
		{
			cerr << "error joining follower thread\n";
			exit(1);
		}
	}

	for (i = 0; i < 4; i++) semctl(stage_sem_id, i, IPC_RMID, 0);

	return 0;
}