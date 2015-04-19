// thread.h
#ifndef __thread_h__
#define __thread_h__
#include "atd.h"
using namespace atd;
#include <pthread.h>
#include <semaphore.h>
//====================================================
//= struct mutex
//====================================================
struct mutex : public object
{
	pthread_mutex_t mx;
	mutex();
	~mutex();
	void lock();
	void unlock();
};
//====================================================
//= struct semaphore
//====================================================
struct semaphore : public object
{
	sem_t sm;
	semaphore(int max = 0);
	~semaphore();
	void post();
	void wait();
};
//====================================================
//= struct cond
//====================================================
struct cond : public object
{
	pthread_cond_t cd;
	cond();
	~cond();
	void signal();
	void wait(mutex &mutex);
};
//====================================================
//= struct thread
//====================================================
struct thread : public object
{
	typedef generic::function<void, void> function;	
	function func;
	pthread_t th;
	thread(const function &func);
	~thread();
	void start();
	void *kernel();
	void join();

	template <typename T>
	thread(void (T::*method)(), T *offset);
	thread(function::func_type func);
};
template <typename T>
thread::thread(void (T::*method)(), T *offset) : func(method, offset)
{
	start();
}
#endif//__thread_h__



