#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "kernel/panic.h"

template <class T>
class QueueNode {
public:
    T data;
    QueueNode* next;
};

template <class T>
class Queue {
public:
    Queue() {};
    Queue(Queue<T>& other);
    ~Queue();

    void enqueue(T); // Panics if there is no enough memory
    T dequeue();     // The queue must not be empty.
    T top();         // The queue must not be empty.
    int size();

    bool empty();

private:
    QueueNode<T> *head = nullptr, *tail = nullptr;
    int queue_size = 0;
};

template<class T>
Queue<T>::Queue(Queue<T>& other) 
{
    for (int i = 0; i < other.size(); i++)
    {
        T head = other.dequeue();
        enqueue(head);
        other.enqueue(head);
    }
}

template<class T>
Queue<T>::~Queue() 
{
    while (head != nullptr)
    {
        QueueNode<T>* tmp = head->next;
        delete head;
        head = tmp;
    }
}

template<class T>
void Queue<T>::enqueue(T data) 
{
    QueueNode<T>* new_node = new QueueNode<T>;
    if (!new_node)
        panic("Queue -> enqueue(): Not enough memory!\n");
    new_node->data = data;
    new_node->next = nullptr;

    if (!head)
        head = tail = new_node;
    else
    {
        tail->next = new_node;
        tail = new_node;
    }

    queue_size++;
}

template<class T>
T Queue<T>::dequeue() 
{
    T data = head->data;
    if (head == tail)
        tail = nullptr;
    head = head->next;
    
    queue_size--;

    return data;
}

template<class T>
T Queue<T>::top() 
{
    return head->data;
}

template<class T>
bool Queue<T>::empty() 
{
    return head == nullptr;
}

template<class T>
int Queue<T>::size()
{
    return queue_size;
}

#endif // __QUEUE_H__