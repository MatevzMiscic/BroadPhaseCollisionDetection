#pragma once

#include <string.h>
#include "Settings.h"

// works as usual bitset
struct Bitset{
	uint* bits;
	uint size;

	inline Bitset(const uint size);
	inline ~Bitset();

	inline void clear();

	inline bool operator[] (const uint index);
	inline void set(const uint index, const bool value);
	inline void set(const uint index);
	inline void reset(const uint index);
};

Bitset::Bitset(const uint bitCount){
	size = (bitCount+31)/32;
	bits = new uint[size];
}
Bitset::~Bitset(){
	delete[] bits;
}

void Bitset::clear(){
	memset(bits, 0, sizeof(int)*size);
}

bool Bitset::operator[] (const uint index){
	uint mask = 1 << (index % 32);
	return (bits[index/32] & mask) == mask;
}
void Bitset::set(const uint index, const bool value){
	uint mask = 1 << (index % 32);
	if(value){
		bits[index/32] |= mask;
	}else{
		bits[index/32] &= ~mask;
	}
}
void Bitset::set(const uint index){
	uint mask = 1 << (index % 32);
	bits[index/32] |= mask;
}
void Bitset::reset(const uint index){
	uint mask = 1 << (index % 32);
	bits[index/32] &= ~mask;
}
