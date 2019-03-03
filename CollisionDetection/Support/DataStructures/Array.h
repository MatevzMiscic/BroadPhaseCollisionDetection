#pragma once

#include <string.h>

// dynamic array in which elements are allways written consecutively in memory which allows efficient iterating through them
template<class Data>
struct Array{
	Data* array; // array that holds data
	int capacity; // array's capacity
	int count; // number of elements in array

	inline Array(const int size);
	inline ~Array();

	// ensures memory for one more element and returns index of its position
	// references may become invalid
	inline int allocate(); 

	// moves last element into given position
	// Returns true if last element has changed position, false otherwise.
	inline bool deallocate(const int id);

	// returns reference to the element at given position
	inline Data& operator[] (const int index);
};

template<class Data>
Array<Data>::Array(const int size){
	array = new Data[size];
	capacity = size;
	count = 0;
}

template<class Data>
Array<Data>::~Array(){
	delete[] array;
}

template<class Data>
int Array<Data>::allocate(){
	if(count == capacity){
		capacity *= 2;
		Data* oldArray = array;
		array = new Data[capacity];
		memcpy(array, oldArray, sizeof(Data)*count);
		delete[] oldArray;
	}
	return count++;
}

template<class Data>
bool Array<Data>::deallocate(const int id){
	--count;
	if(id != count){
		array[id] = array[count];
		return true;
	}
	return false;
}

template<class Data>
Data& Array<Data>::operator[] (const int index){
	return array[index];
}
