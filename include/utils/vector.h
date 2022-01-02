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

    // ----------------------- Operators -----------------------
    Vector<T> &operator=(const Vector &other);

    // ------------------- Regular Methods -------------------
    T get(int index) const; // Panic if index is out of range
    bool empty() const;
    bool exist(T) const; // Does the given object exist in the list?
    int size() const;
    void remove(int index); // Panics if the index is out of range
    void append(T);         // Panics if there is no enough memory
    void pop_back();        // Deletes the last element; Panics if empty

    // ------------------- Methods with long docs -------------------
    /**
     * Sorts the vector, according to the given function.
     * The given function gets two variables of type T as a parameter.
     * Returns true if the first parameter is bigger than the second.
    */
    void sort(bool (*compare)(T first, T second));

private:
    // ------------------- Regular Methods -------------------
    VectorNode<T> *get_node(int index) const;
    void append_node(VectorNode<T> *new_node);
    void append_vector(const Vector &other);
    void free_data();                             // Frees all of the dynamic allocated data
    void empty_vector();                          // Empties the vector. DOES NOT FREE DATA! Call free_data() for that before.
    void panic_if_illegal_index(int index) const; // Panics if either the index is minus or out of range

    VectorNode<T> *head;
    int vector_size;
};

template <class T>
Vector<T>::Vector()
{
    empty_vector();
}

template <class T>
Vector<T>::Vector(Vector<T> &other)
{
    empty_vector();
    append_vector(other);
}

template <class T>
Vector<T>::~Vector()
{
    free_data();
}

template <class T>
Vector<T> &Vector<T>::operator=(const Vector &other)
{
    // Destroy the previous state
    free_data();
    empty_vector();

    // Copy the new data
    append_vector(other);

    return *this;
}

template <class T>
void Vector<T>::panic_if_illegal_index(int index) const
{
    if (index + 1 > size())
        panic("Vector: Index \"%d\" is out of range", index);
    else if (index < 0)
        panic("Vector: Index \"%d\" is negative!", index);
}

template <class T>
void Vector<T>::remove(int index)
{
    // First - assurance
    panic_if_illegal_index(index);

    // The simplest case - it's the first element
    if (index == 0)
    {
        VectorNode<T> *node = head->next;
        delete head;
        head = node;
    }
    else
    {
        // Get the node before
        VectorNode<T> *before = get_node(index - 1);
        // The next node
        VectorNode<T> *tmp = before->next->next;
        // Delete
        delete before->next;
        // Advance
        before->next = tmp;
    }

    vector_size--;
}

template <class T>
void Vector<T>::append_vector(const Vector &other)
{
    for (int i = 0; i < other.size(); i++)
        append(other.get(i));
}

template <class T>
void Vector<T>::free_data()
{
    while (head)
    {
        VectorNode<T> *tmp = head->next;
        delete head;
        head = tmp;
    }
}

template <class T>
void Vector<T>::empty_vector()
{
    head = nullptr;
    vector_size = 0;
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
        free_data();
        empty_vector();
    }

    // There is more than one element
    else
    {
        VectorNode<T> *ptr = head;
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
bool Vector<T>::exist(T obj) const
{
    for (int i = 0; i < size(); i++)
        if (get(i) == obj)
            return true;

    return false;
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
    panic_if_illegal_index(index);

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
