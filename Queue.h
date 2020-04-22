/*
 * Queue.h
 *
 *  Created on: 24 Nov 2019
 *      Author: Lina
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <iostream>
#include <cstdio>
#include <cstdlib>

using std::cout;
using std::endl;
using std::cerr;
using std::endl;
using std::cerr;
enum QUEUE_TYPE { PRODUCER,CONSUMER };

template<class T>
class Queue {
	QUEUE_TYPE q_type;
	//request or response. it shoud have uint64_t status element. (first 64 bits)
	T *arr;
	//maximum number of requests i in the queue
	int qsize;	
	// shared indexes
	uint64_t* consumer_idx; 
	uint64_t* producer_idx; 
	//internal checking
	void verify_index(int index);
	void verify_index_producer_consumer(int index);
	bool verify_to_dequeue();
	bool verify_to_enqueue();

	public:
	Queue(T* base_addr,int qsize,uint64_t* _producer_idx,uint64_t* _consumer_idx, QUEUE_TYPE _q_type);//size_t _HEADER_SIZE,size_t _MSG_SIZE);
	Queue(const Queue& q);
	~Queue();
	T dequeue();
	void enqueue(T item);
	int length();
	int size();
	bool isEmpty();
	bool isFull();
	Queue& operator=(const Queue& q);
	const T& operator[](int index) const;
	const T& operator[](int index);

};

void error(const char*str) {
	cerr << "Error: " << str << endl;
	exit(0);

}

/*
 * verifying index for the operator []
 */
template<class T>
void Queue<T>::verify_index(int index){
	if(index>=qsize || index<0 ){
		error("Bad index");
		return;
	}
	if(*producer_idx==*consumer_idx){
		uint64_t * ptr_status=reinterpret_cast<uint64_t*>(arr+(*producer_idx));
		if(*ptr_status==1){	
			return;
		}
	}
	if(*producer_idx>*consumer_idx && index>=*consumer_idx && index<*producer_idx){
		return;
	}
	if(*producer_idx<*consumer_idx && (index>= *consumer_idx || index<*producer_idx )){
		return;
	}
	error("Bad index");
}

template<class T>
void Queue<T>::verify_index_producer_consumer(int index){
	if(index>=qsize || index<0 ){
		error("Bad index");
		return;
	}

}
template<class T>
bool Queue<T>::verify_to_enqueue(){
	uint64_t * ptr_status=reinterpret_cast<uint64_t*>(arr+(*producer_idx));
	while(*ptr_status==1); //loop while the header say it is ready
	return true;
}
template<class T>
bool Queue<T>::verify_to_dequeue(){
	uint64_t * ptr_status=reinterpret_cast<uint64_t*>(arr+(*consumer_idx));
	while(*ptr_status==0); //loop while the header say it is not ready
	return true;
}

template<class T>
/*
 *base_addr - base address of the queue (VA),qsize number of requests in there queue (slotsnum) 
 */
Queue<T>::Queue(T* base_addr,int qsize,uint64_t* _producer_idx,uint64_t* _consumer_idx, QUEUE_TYPE _q_type){//,size_t _HEADER_SIZE,size_t _MSG_SIZE) {
	std::cout << qsize << std::endl;
	this->arr = base_addr;
	this->qsize = qsize;
	this->consumer_idx=_consumer_idx;
	this->producer_idx=_producer_idx;
	q_type=_q_type;
	if(q_type==CONSUMER){
		*(this->consumer_idx)=0;
	}else if(q_type==CONSUMER){
		*(this->producer_idx)=0;
	}
	//this->HEADER_SIZE=_HEADER_SIZE;
	//this->MSG_SIZE=_MSG_SIZE;
}

template<class T>
Queue<T>::Queue(const Queue& q) {
	this->qsize = q.qsize;
	this->consumer_idx=consumer_idx;
	this->producer_idx=producer_idx;
	this->arr = q.arr;
}

template<class T>
Queue<T>::~Queue() {
	//delete[] arr; the allocated memory is not freed here (by host).
}

template<class T>
int Queue<T>::length(){
	if(*producer_idx==*consumer_idx){
		uint64_t * ptr_status=reinterpret_cast<uint64_t*>(arr+(*producer_idx));
		if(*ptr_status==0){
			return 0;
		}else{
			return qsize;
		}
	}else if(*producer_idx>*consumer_idx){
		return *producer_idx-*consumer_idx; //it is the first empty availble idx to write to
	}else{ //*producer_idx<*consumer_idx
		return qsize-*consumer_idx+*producer_idx;
	}
}

template<class T>
int Queue<T>::size(){
	//number of request in the queue (MEM_SIZE)/sizeof(request)
	return this->qsize;
}

template<class T>
Queue<T>& Queue<T>::operator=(const Queue<T>& q) {
	if (this == &q) {
		return *this;
	}
	this->qsize = q.qsize;
	this->arr=q.arr;
	this->consumer_idx=consumer_idx;
	this->producer_idx=producer_idx;
	return *this;
}

template<class T>
const T& Queue<T>::operator[](int index) const {
	verify_index(index);
	return arr[index];
}

template<class T>
const T& Queue<T>::operator[](int index) {
	verify_index(index);
	return arr[index];
}

template<class T>
bool Queue<T>::isEmpty() {
	return (this->length() == 0);
}

template<class T>
bool Queue<T>::isFull() {
	return (this->length() == qsize);

}

template<class T>
void Queue<T>::enqueue(T item) {
	std::cout << "producer and consumer idxes" << std::endl;
	std::cout << *producer_idx << std::endl;
	std::cout << *consumer_idx << std::endl;

	assert(q_type==PRODUCER);
	while(isFull()); //busy wait when it full, in this case *producer_idx==*consumer_idx and the data is valid
	std::cout << "is not full, please push" <<std::endl;
	while(!verify_to_enqueue());	 
	uint64_t * ptr_status=reinterpret_cast<uint64_t*>(arr+(*producer_idx));
	arr[*producer_idx]=item;
	*producer_idx=(*producer_idx+1)%qsize;
	std::cout << "producer new idx" << std::endl;
	std::cout << *producer_idx << std::endl;
	verify_index_producer_consumer(*(producer_idx));
	*ptr_status=1; //the consumer allowed to use it.
}


template<class T>
T Queue<T>::dequeue() {
	assert(q_type==CONSUMER);
	while(isEmpty()); //busy wait when it empty
	std::cout << "is not empty, please pull" <<std::endl;
	while(!verify_to_dequeue());	
	uint64_t * ptr_status=reinterpret_cast<uint64_t*>(arr+(*consumer_idx));
	T data = arr[*(consumer_idx)];
	*consumer_idx=(*consumer_idx+1)%qsize;
	std::cout << " the new idx of the consumer is " << std::endl;
	std::cout << *consumer_idx << std::endl;
	verify_index_producer_consumer(*(consumer_idx));
	*ptr_status=0; //the producer allowed to use it.
	return data;
}

#endif /* QUEUE_H_ */
