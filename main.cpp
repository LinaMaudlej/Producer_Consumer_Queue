#include <assert.h>
#include <iostream>
#include <cmath>
#include <cstdio>
#include <unistd.h>
#include <assert.h>
#include <iostream> 
#include "common.h"
#include <sys/mman.h>
#include <thread> 
#define REQS_COUNT 5
	
	request* send_base_addr=new request[5];
	int send_qsize= 5;
	uint64_t* send_producer_qnex=new uint64_t;
	uint64_t* send_consumer_qnex=new uint64_t;	
	QUEUE_TYPE _q_type=PRODUCER;
      	Queue<request>* send_q=new Queue<request>(send_base_addr,send_qsize,send_producer_qnex,send_consumer_qnex,_q_type);//,REQ_HEADER_SIZE,REQ_MSG_SIZE);
	Queue<request>* recv_q=new Queue<request>(send_base_addr,send_qsize,send_producer_qnex,send_consumer_qnex,CONSUMER);//,REQ_HEADER_SIZE,REQ_MSG_SIZE);

	request reqs[REQS_COUNT];
	

void thread1(){
	std::cout << "thread1 running" << std::endl;
	std::cout << "first enq" << std::endl;
	send_q->enqueue(reqs[0]);
        std::cout << "2 enq" << std::endl;
	send_q->enqueue(reqs[1]);
	std::cout << "3 enq" << std::endl;
        send_q->enqueue(reqs[1]);
        std::cout << "4 enq" << std::endl;
	send_q->enqueue(reqs[3]);
        std::cout << "5 enq" << std::endl;
        send_q->enqueue(reqs[4]);
	send_q->enqueue(reqs[0]);

	send_q->enqueue(reqs[0]);

	std::cout << " blocking" <<std::endl;
	
}
void thread2(){

	recv_q->length();
	request req=recv_q->dequeue();
       	std::cout << "length is" << std::endl;
        std::cout << send_q->length() << std::endl;
        std::cout << "length is" << std::endl;
        std::cout << recv_q->length() << std::endl;
        recv_q->dequeue();
	std::cout << "unblocking" <<std::endl;

}
int main(int argc,char* argv[]){


	thread t1(thread1);
	thread t2(thread2);
	t1.join();
	t2.join();

	return 0;	
}	
