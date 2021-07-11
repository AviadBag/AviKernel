#include "utils/fixed_queue.h"
#include "drivers/keyboard/extended_char.h"

template class FixedQueue<ExtendedChar>;

template <class T>
FixedQueue<T>::FixedQueue()
{
	head = tail = -1;
	is_empty = true;
}

template <class T>
void FixedQueue<T>::enqueue(T data)
{
	is_empty = false; // The queue will be not empty after this enqueue

	tail++;
	tail %= QUEUE_SIZE;
	arr[tail] = data;

	if (head == -1)
		head = 0; // Initialize head
	else if (head == tail) // We have just overriden the first data
	{
		head++;
		head %= QUEUE_SIZE;
	}
}


template <class T>
T FixedQueue<T>::dequeue()
{
	if (head == tail) // The queue will be empty after this dequeue
		is_empty = true;

	T data = arr[head];
	head++;
	head %= QUEUE_SIZE;

	if (empty())
		head = tail = -1; // Re-initialize the pointers.

	return data;
}

template <class T>
bool FixedQueue<T>::empty()
{
	return is_empty;
}
