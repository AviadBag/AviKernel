#ifndef __VECTOR_H__
#define __VECTOR_H__

/* A simple linked list */

#include "kernel/panic.h"

template <class T>
class VectorNode
{
public:
    T data;
    VectorNode *next;
};

template <class T>
class Vector
{
public:
    // ------------------- Constructors and Destructor -------------------
    Vector();
    Vector(Vector<T> &other);
    ~Vector();

    // ------------------- Regular Methods -------------------
    T get(int index) const;    // Panic if index is out of range
    bool empty()     const;
    int  size()      const;
    void append(T);            // Panics if there is no enough memory
    void pop_back();           // Deletes the last element; Panics if empty
    
    // ------------------- Methods with long docs -------------------
    /**
     * Sorts the vector, according to the given function.
     * The given function gets two variables of type T as a parameter.
     * Returns true if the first parameter is bigger than the second.
    */
    void sort(bool (*compare)(T first, T second));

private:
    // ------------------- Regular Methods -------------------
    VectorNode<T>* get_node(int index) const;
    void           append_node(VectorNode<T> *new_node);

    VectorNode<T> *head;
    int vector_size;
};

template <class T>
Vector<T>::Vector()
{
    head = nullptr;
    vector_size = 0;
}

template <class T>
Vector<T>::Vector(Vector<T> &other)
    : Vector()
{
    for (int i = 0; i < other.size(); i++)
        append(other.get(i));
}

template <class T>
Vector<T>::~Vector()
{
    while (head)
    {
        VectorNode<T> *tmp = head->next;
        delete head;
        head = tmp;
    }
}

template <class T>
void Vector<T>::pop_back()
{
    // Panic if empty
    if (empty())
        panic("Vector: Cannot delete last element when empty!");

    // There is only one element - delete it
    else if (size() == 1)
    {
        delete head;
        head = nullptr;
        vector_size = 0;
    }

    // There is more than one element
    else
    {
        VectorNode<T>* ptr = head;
        // advance until one before last
        while (ptr->next->next != nullptr)
            ptr = ptr->next;

        // Delete!
        delete ptr->next;
        ptr->next = nullptr;
        
        vector_size--;
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
    VectorNode<T> *new_node = new VectorNode<T>;
    if (!new_node)
        panic("Vector -> append(): Not enough memory!\n");
    new_node->data = data;
    new_node->next = nullptr;

    append_node(new_node);
}

template <class T>
void Vector<T>::append_node(VectorNode<T> *new_node)
{
    if (!head)
        head = new_node;
    else
    {
        VectorNode<T> *node = head;
        while (node->next != nullptr)
            node = node->next;

        node->next = new_node;
    }

    vector_size++;
}

template <class T>
T Vector<T>::get(int index) const
{
    return get_node(index)->data;
}

template <class T>
VectorNode<T> *Vector<T>::get_node(int index) const
{
    if (index + 1 > size())
        panic("Vector: Index \"%d\" is out of range", index);

    VectorNode<T> *node = head;
    while (index--)
        node = node->next;

    return node;
}

template <class T>
void Vector<T>::sort(bool (*compare)(T first, T second))
{
    // Insertion sort

    VectorNode<T> *iterator = head;
    VectorNode<T> *iterator_prev = nullptr;
    VectorNode<T> *new_iterator;      // The iterator might move; we want to backup is next.
    VectorNode<T> *new_iterator_prev; // The iterator might move; We want to save his new prev.
    int sorted_area_size = 0;

    while (iterator)
    {
        new_iterator = iterator->next;
        new_iterator_prev = iterator;

        VectorNode<T> *insertion_ptr = head;
        VectorNode<T> *insertion_ptr_prev = nullptr;
        for (int i = 0; i < sorted_area_size; i++)
        {
            if (compare(insertion_ptr->data, iterator->data))
            {
                // We should insert <iterator> behind <insertion_ptr>
                if (!iterator_prev)
                    continue; // This is the first item; There is nothing to do.

                new_iterator_prev = iterator_prev; // The operator will move.
                new_iterator = iterator->next;

                iterator_prev->next = iterator->next; // Now <iterator> is not connected to the list anymore
                iterator->next = insertion_ptr;       // Now iterator is right behind <insertion_ptr>
                if (!insertion_ptr_prev)
                    // We are inserting behind the FIRST item - so we should only update <head>
                    head = iterator;
                else
                    // We have to update <insertion_ptr_prev>
                    insertion_ptr_prev->next = iterator;

                break; // We inserted our var in the right place; no need further investigation.
            }

            insertion_ptr_prev = insertion_ptr;
            insertion_ptr = insertion_ptr->next;
        }

        sorted_area_size++;
        iterator_prev = new_iterator_prev;
        iterator = new_iterator;
    }
}

#endif // __VECTOR_H__