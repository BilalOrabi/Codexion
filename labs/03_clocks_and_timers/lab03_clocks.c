/*
 * Lab 03: Precise Clocks & Drift-Free Sleep
 * Follow the specifications in docs/03_Clocks_and_Precise_Timers.md
 */
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
long long get_time_ms(void)
{

	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000LL + tv.tv_usec / 1000LL);
}

void precise_sleep(long long duration_ms)
{
	long long start = get_time_ms();

	while ((get_time_ms() - start) < duration_ms)
	{
		usleep(500);
	}
}

int main(void)
{
	long long start_time = get_time_ms();
	int i = 100;
	long long end_time = 0;
	long long elapsed = 0;
	while (i > 0)
	{
		precise_sleep(10);
		i--;
	}
	end_time = get_time_ms();
	elapsed = end_time - start_time;
	printf("Expected time: 1000 ms\n");
	printf("Actual time:   %lld ms\n", elapsed);
	printf("Total drift:   +%lld ms\n", elapsed - 1000LL);
	return (0);
}
