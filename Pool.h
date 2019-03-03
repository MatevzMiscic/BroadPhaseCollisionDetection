#pragma once

#include <string.h>

static constexpr int nullEntry = -1;

// element of a pool
// it can either contain data or index of the next element in the free list
template<class Data>
union Entry{
	Data data;
	int next;
	Entry(){

	}
};

// dynamic array in which data never changes position, even if element is removed from the middle.
// instead removed element is added to the free list to be used again
template<class Data>
struct Pool{
private:
	
public:
	Entry<Data>* array; // array of entries
	int capacity; // array's capacity
	int count; // number of used entries
	int free; // first element in the free list

	Pool(int size);
	~Pool();

	// returns reference to data at given position
	inline Data& operator[] (int index);

	// ensures memory for one more element and returns index of its position
	// references may become invalid
	int allocate();

	// pushes element at given position to the free list
	void deallocate(int index);
};

template<class Data>
Pool<Data>::Pool(int size){
	array = new Entry<Data>[size];
	capacity = size;
	count = 0;
	free = 0;
	for(int i = 0; i < capacity-1; ++i){
		array[i].next = i+1;
	}
	array[size-1].next = nullEntry;
}

template<class Data>
Pool<Data>::~Pool(){
	delete[] array;
}

template<class Data>
Data& Pool<Data>::operator[] (int index){
	return array[index].data;
}

template<class Data>

int Pool<Data>::allocate(){
	if(count == capacity){
		free = capacity;
		capacity *= 2;
		Entry<Data>* oldArray = array;
		array = new Entry<Data>[capacity];
		memcpy(array, oldArray, sizeof(Entry<Data>)*count);
		delete[] oldArray;
		for(int i = free; i < capacity-1; ++i){
			array[i].next = i+1;
		}
		array[capacity-1].next = nullEntry;
	}
	++count;
	int index = free;
	free = array[free].next;

	if(free >= capacity){
		int a = 0;
	}

	return index;
}

template<class Data>
void Pool<Data>::deallocate(int index){
	--count;
	array[index].next = free;
	free = index;
}