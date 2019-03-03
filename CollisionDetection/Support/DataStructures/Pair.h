#pragma once

// holds a pair
template<class T>
struct Pair{
	T a;
	T b;
	Pair();
	Pair(T a, T b);
};

template<class T>
Pair<T>::Pair(){

}

template<class T>
Pair<T>::Pair(T a, T b): a(a), b(b){

}
