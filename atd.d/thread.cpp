// thread.cpp
#include "thread.h"
//====================================================
//= struct mutex
//====================================================
mutex::mutex()
{
	::pthread_mutex_init(&mx, NULL);
}
mutex::~mutex()
{
	::pthread_mutex_destroy(&mx);
}
void mutex::lock()
{
	::pthread_mutex_lock(&mx);
}
void mutex::unlock()
{
	::pthread_mutex_unlock(&mx);
}
//====================================================
//= struct semaphore
//====================================================
semaphore::semaphore(int max)
{
	::sem_init(&sm, 0, max);
}
semaphore::~semaphore()
{
	::sem_destroy(&sm);
}
void semaphore::post()
{
	::sem_post(&sm);
}
void semaphore::wait()
{
	::sem_wait(&sm);
}
//====================================================
//= struct cond
//====================================================
cond::cond()
{
	::pthread_cond_init(&cd, NULL);
}
cond::~cond()
{
	::pthread_cond_destroy(&cd);
}
void cond::signal()
{
	::pthread_cond_signal(&cd);
}
void cond::wait(mutex &mutex)
{
	::pthread_cond_wait(&cd, &mutex.mx);
}
//====================================================
//= struct thread
//====================================================
thread::thread(const function &func)
: func(func)
{
	struct call
	{
		static void *back(void *data)
		{
			return ((thread *)data)->kernel();
		}
	};
	::pthread_create(&th, NULL, call::back, (void *)this);
}
thread::~thread()
{
}
void *thread::kernel()
{
	func();
	return NULL;
}
void thread::join()
{
	::pthread_join(th, NULL);
}