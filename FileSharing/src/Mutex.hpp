/*!
 *   @File   Mutex.hpp
 *   @Author Steven Liss and Westley Hennigh
 *   @Date   25 Feb 2010, 20 Oct 2010
 *   @Brief  Mutex interface, wraps critical section.
 */

#pragma once

class Lock;  // forward declare for mutex

/*
Create a mutex (which will wrap up a critical section).
When you need to use shared data, create a lock with the
mutex you created. The lock will enter the critical section
upon construction and then leave upon destruction (so no
need to remember to unlock).
*/

class Mutex
{
public:
	Mutex () { InitializeCriticalSection (&CriticalSection); }
	~Mutex () { DeleteCriticalSection (&CriticalSection); }

private:
	// Acquire and release will be called by the lock only, there is no
	// external access.
	void Acquire ()
	{
		EnterCriticalSection (&CriticalSection);
	}

	void Release ()
	{
		LeaveCriticalSection (&CriticalSection);
	}

	CRITICAL_SECTION CriticalSection;  // the critical section for this mutex

	friend class Lock;

  template < typename T >
  friend class SecureObject;
};


class Lock
{
public:
	// Acquire the semaphore
	Lock ( Mutex& mutex_ )
		: mutex(mutex_)
	{
		mutex.Acquire();
	}

	// Release the semaphore
	~Lock ()
	{
		mutex.Release();
	}

private:
	Mutex& mutex;

  // DO NOT CALL
  Lock& operator = ( Lock& );
};
