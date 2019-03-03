#pragma once

#include <string.h>

// works as usual stack
template<class Data>
struct Stack{
	Data* array; // array that holds data
	int capacity; // array's capacity
	int count; // number of elements in array

	inline Stack(int size);
	inline ~Stack();

	// clears stack
	inline void clear();

	// returns true if the stack is empty, false otherwise
	inline bool empty();

	// returns reference to the element at given position
	inline Data& operator[] (const int index);

	// ensures memory for one more element
	inline void allocate();

	// adds new element on top of the stack
	inline void push(const Data& data);

	// removes element from top of the stack and returns it
	inline Data& pop();
};

template<class Data>
Stack<Data>::Stack(int size){
	array = new Data[size];
	capacity = size;
	count = 0;
}
template<class Data>
Stack<Data>::~Stack(){
	delete[] array;
}

template<class Data>
void Stack<Data>::clear(){
	count = 0;
}
template<class Data>
bool Stack<Data>::empty(){
	return count == 0;
}
template<class Data>
Data& Stack<Data>::operator[] (const int index){
	return array[index];
}

template<class Data>
void Stack<Data>::allocate(){
	if(count == capacity){
		capacity *= 2;
		Data* oldArray = array;
		array = new Data[capacity];
		memcpy(array, oldArray, sizeof(Data)*count);
		delete[] oldArray;
	}
}
template<class Data>
void Stack<Data>::push(const Data& data){
	allocate();
	array[count] = data;
	++count;
}
template<class Data>
Data& Stack<Data>::pop(){
	return array[--count];
}
