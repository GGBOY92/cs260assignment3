
/*!
 *   @File   SecureObject.hpp
 *   @Author Steven Liss
 *   @Date   20 Oct 2010
 *   @Brief  Secure object, wraps mutex and data it protects together.
 */

#include "Mutex.hpp"

template < typename T >
class SecureObject
{
  T object_;
  Mutex mutex_;
public:

  SecureObject( T const &object ) : object_( object ) {}

  void Lock() { mutex_.Acquire(); }
  void UnLock() { mutex_.Release(); }

  T &Get( void )
  {
    return object_;
  }

  ~SecureObject() {}
};
