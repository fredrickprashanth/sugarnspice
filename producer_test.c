#include <stdio.h>

#include <pthread.h>

#define DATA_SZ 204800

int data[DATA_SZ];
int producer_head;

int _d_i_producer;

#define loop(count) do { static int _tmp = 0; \
			 for(_tmp=0; _tmp<count; _tmp++); \
			}while(0)
void*
producer_thread(void *unused_data)
{
	while(1)
	{
		data[producer_head] = _d_i_producer++;
		producer_head = (producer_head + 1)%DATA_SZ;
		loop(99999);
		//sleep(1);
	}
	pthread_exit(0);

}
int consumer_head;

#define CONSUME_RATE 100
void*
consumer_thread(void *unused_data)
{
	int i;
	while(1)
	{
		for(i=0; i<CONSUME_RATE && consumer_head!=producer_head; i++)
		{
			printf("%d\n", data[consumer_head]);
			consumer_head = (consumer_head + 1)%DATA_SZ;
		}
		//sleep(4);
		sleep(1);
	}
	pthread_exit(0);

}	
int main()
{

	void *ret_val;
	pthread_t th_producer, th_consumer;
	pthread_create(&th_producer, NULL, producer_thread, NULL);
	pthread_create(&th_consumer, NULL, consumer_thread, NULL);
	pthread_join(th_producer, &ret_val);
	pthread_join(th_consumer, &ret_val);
	return 0;
}
	
