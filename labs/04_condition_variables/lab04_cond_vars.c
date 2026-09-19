/*
 * Lab 04: Condition Variables & Event-Driven Signaling
 * Follow the specifications in docs/04_Condition_Variables.md
 *
 * Requirements:
 * 1. A shared struct holding a flag/counter, a mutex, and a condition variable.
 * 2. Worker thread that sleeps on pthread_cond_wait until the condition is met.
 * 3. Main thread that prepares work, sets state, and signals the worker.
 * 4. Zero spinlocks, zero data races (-fsanitize=thread), 42 Norm compliance.
 */
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct s_hub
{
	int ready;
	pthread_mutex_t lock;
	pthread_cond_t cond;

} t_hub;

void *worker_routine(void *arg)
{
	t_hub *hub;

	hub = (t_hub *)arg;
	pthread_mutex_lock(&hub->lock);
	printf("[Worker] Waiting for signal (0 CPU)...\n");

	while (hub->ready == 0)
	{
		pthread_cond_wait(&hub->cond, &hub->lock);
	}
	printf("[Worker] Woke up! Work completed.\n");
	pthread_mutex_unlock(&hub->lock);
	return (NULL);
}

int main(void)
{
	t_hub hub;
	pthread_t thread;
	hub.ready = 0;
	pthread_mutex_init(&hub.lock, NULL);
	pthread_cond_init(&hub.cond, NULL);
	pthread_create(&thread, NULL, worker_routine, (void *)&hub);
	sleep(1);
	pthread_mutex_lock(&hub.lock);
	hub.ready = 1;
	printf("work is ready!\n");
	pthread_cond_signal(&hub.cond);
	pthread_mutex_unlock(&hub.lock);
	pthread_join(thread, NULL);
    pthread_mutex_destroy(&hub.lock);
    pthread_cond_destroy(&hub.cond);
	return (0);
}
