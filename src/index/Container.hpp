/*
 * Created by yuan pingzhou on 8/11/17.
 */

#ifndef REVERSEDINDEXSERVICE_CONTAINER_HPP
#define REVERSEDINDEXSERVICE_CONTAINER_HPP

#include <iostream>

typedef struct{
	std::string id;
	int size;
} DOC;

typedef struct{
	long doc_belong;
	int size;
} CONJUNCTION;

typedef struct{
	long conj_belong;
	std::string name;
	std::string value;
	int relation;
	int conj_size;
} ASSIGNMENT;

#endif //REVERSEDINDEXSERVICE_CONTAINER_HPP
