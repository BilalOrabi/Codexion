#define NUM_THREADS 5
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct threads_data
{
	int id;
	int work_ms;

} t_threads_data;

void *name(void *args)
{
	t_threads_data *tdata = (t_threads_data *)args;
    printf("[Thread %d] Started. TID: %lu, Will work for %d ms\n", 
       tdata->id, (unsigned long)pthread_self(), tdata->work_ms);
	int *exit_code = malloc(sizeof(int));
	if (!exit_code)
		return (NULL);

	*exit_code = tdata->id * 10;

	usleep(tdata->work_ms * 1000);
	printf("[Thread %d] Finished.\n", tdata->id);
	printf("work in ms : %d\n", tdata->work_ms);

	return (void *)exit_code;
}

int main()
{
	pthread_t threads[NUM_THREADS];
	t_threads_data tdata[NUM_THREADS];
	void *retval;

	for (int i = 0; i < NUM_THREADS; i++)
	{
		tdata[i].id = i + 1;
		tdata[i].work_ms = i + 10;
		pthread_create(&threads[i], NULL, name, (void *)&tdata[i]);
	}
	for (int i = 0; i < NUM_THREADS; i++)
	{
		pthread_join(threads[i], &retval);
		printf("exit status: %d\n", *((int *)retval));
		free(retval);
	}

	return 0;
}