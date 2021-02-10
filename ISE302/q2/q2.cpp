#include <unistd.h>

using namespace std;

pthread_mutex_t m1, m2;

void randezvous()
{
	printf("rand\n");
}
void critical()
{
	int i;
	for (i = 0; i < 10; i++)
		printf("crit%d", i);
	printf("\n");
}

int count = 0;
void* thread_func(void* t)
{
	sleep(1);
	randezvous();
	pthread_mutex_lock(&m1);
	count++;
	if (count == 5) pthread_mutex_unlock(&m2);
	pthread_mutex_unlock(&m1);

	pthread_mutex_lock(&m2);
	critical();
	pthread_mutex_unlock(&m2);

	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{

	pthread_mutex_init(&m1, NULL);
	pthread_mutex_init(&m2, NULL);
	pthread_t threads[5];

	for (int i = 0; i < 5; i++)
	{
		int return_code = pthread_create(&threads[i], NULL, thread_func, NULL);
	}
	pthread_mutex_lock(&m2);
	sleep(5);
	return 0;
}