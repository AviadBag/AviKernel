#ifndef _H_FIXED_QUEUE
#define _H_FIXED_QUEUE

#define QUEUE_SIZE 2048 

template <class T>
class FixedQueue
{
public:
	FixedQueue();
	
	void enqueue(T);
	T dequeue();
	T top(); // If the queue is empty, a garbage value will be returned and the queue will be destroyed.

	bool empty();

private:
	T arr[QUEUE_SIZE];
	
	int head, tail;
	bool is_empty;
};

#endif
