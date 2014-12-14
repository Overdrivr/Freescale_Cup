#ifndef BUFFERED_ARRAY_H
#define BUFFERED_ARRAY_H

#include <array>

template<typename T, unsigned int size>
class buffered_array
{
    public:
        buffered_array();
        ~buffered_array();

        bool swap();

        T* get();

        T& operator[](unsigned int i);

    private:
        T* buffers[2];
        unsigned int current;
};

template<typename T, unsigned int size>
buffered_array<T,size>::buffered_array()
{
    this->buffers[0] = new T[size];
    this->buffers[1] = new T[size];
    this->current = 0;
}

template<typename T, unsigned int size>
buffered_array<T,size>::~buffered_array()
{
    delete[] this->buffers[0];
    delete[] this->buffers[1];
}

template<typename T, unsigned int size>
bool buffered_array<T,size>::swap()
{
    if(this->current == 0)
        this->current = 1;
    else
        this->current = 0;
    return true;
}

template<typename T, unsigned int size>
T& buffered_array<T,size>::operator[](unsigned int i)
{
    return this->buffers[this->current][i];
}

template<typename T,unsigned int size>
T* buffered_array<T,size>::get()
{
    return this->buffers[current];
}
#endif // BUFFERED_ARRAY_H
