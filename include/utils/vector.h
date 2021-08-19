#ifndef __VECTOR_H__
#define __VECTOR_H__

/* A simple linked list */

#include "kernel/panic.h"

template <class T>
class VectorNode {
public:
    T data;
    VectorNode* next;
};

template <class T>
class Vector {
public:
    Vector();
    Vector(Vector<T>& other);
    ~Vector();

    bool empty() const;
    int size() const;
    void append(T); // Panics if there is no enough memory
    T get(int index); // Panic if index is out of range

private:
    VectorNode<T>* head;
    int vector_size;
};

template <class T>
Vector<T>::Vector()
{
    head = nullptr;
    vector_size = 0;
}

template<class T>
Vector<T>::Vector(Vector<T>& other) : Vector()
{
    for (int i = 0; i < other.size(); i++)
        append(other.get(i));
}

template <class T>
Vector<T>::~Vector()
{
    while (head) {
        VectorNode<T>* tmp = head->next;
        delete head;
        head = tmp;
    }
}

template <class T>
bool Vector<T>::empty() const
{
    return vector_size == 0;
}

template <class T>
int Vector<T>::size() const
{
    return vector_size;
}

template <class T>
void Vector<T>::append(T data)
{
    VectorNode<T>* new_node = new VectorNode<T>;
    if (!new_node)
        panic("Vector -> append(): Not enough memory!\n");
    new_node->data = data;
    new_node->next = nullptr;

    if (!head)
        head = new_node;
    else {
        VectorNode<T>* node = head;
        while (node->next != nullptr)
            node = node->next;

        node->next = new_node;
    }

    vector_size++;
}

template <class T>
T Vector<T>::get(int index)
{
    if (index + 1 > size())
        panic("Vector: Index \"%d\" is out of range", index);

    VectorNode<T>* node = head;
    while (index--)
        node = node->next;

    return node->data;
}

#endif // __VECTOR_H__