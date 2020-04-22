
#pragma once
#include <stdint.h>
#include <cstdlib> 
#include "Queue.h" 

using namespace std;
//request and response
///*****************************************************************
// ******************************************************************/
struct request{
	uint64_t status_header; // 0 or 1 we have 64bits we can divide it later for status indication.
	uint64_t id; 	
};

struct response{
	uint64_t status_header; // 0 or 1 we have 64bits we can divide it later for status indication.
	uint64_t id;
	uint64_t* imm_data;
};

///*****************************************************************
// ******************************************************************/
