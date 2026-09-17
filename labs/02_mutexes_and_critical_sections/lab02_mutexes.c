/*
 * Lab 02: Race Conditions & Critical Sections (pthread_mutex_*)
 * Follow the specifications in docs/02_Mutexes_and_Critical_Sections.md
 */
#include <stdio.h>
#include <pthread.h>
typedef struct s_data
{
	int counter;
} t_data;

void *increment_routine(void *arg)
{
	t_data *data = (t_data *)arg;
	int i = 0;

	while (i < 100000)
	{
		data->counter++;
		i++;
	}
	return NULL;
}

int main(void)
{
	pthread_t threads[4];
	t_data data;

	data.counter = 0;
	int i = 0;

	while (i < 4)
	{
		pthread_create(&threads[i], NULL, increment_routine, (void *)&data);
		i++;
	}
	i = 0;
	while (i < 4)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	printf("Final counter: %d (Expected: 400000)\n", data.counter);
	return (0);
}
