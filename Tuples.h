/*
 * Tuples.h
 *
 *  Created on: Aug 30, 2017
 *      Author: kevywilly
 */

#ifndef TUPLES_H_
#define TUPLES_H_

template <typename T>
struct tuple2{
	T _1;
	T _2;

	tuple2<T>(T a, T b) {
		_1 = a;
		_2 = b;
	}
};

template <typename T>
struct tuple3{
	T _1;
	T _2;
	T _3;

	tuple3<T>(T a, T b, T c) {
		_1 = a;
		_2 = b;
		_3 = c;
	}
};

template <typename T>
struct tuple4{
	T _1;
	T _2;
	T _3;
	T _4;

	tuple4<T>(T a, T b, T c, T d) {
		_1 = a;
		_2 = b;
		_3 = c;
		_4 = d;
	}
};




#endif /* TUPLES_H_ */
