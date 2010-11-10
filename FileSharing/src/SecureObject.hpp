
/*!
 *   @File   SecureObject.hpp
 *   @Author Steven Liss
 *   @Date   20 Oct 2010
 *   @Brief  Secure object, wraps mutex and data the mutex protects together.
 */

#include "Mutex.hpp"

template < typename T >
class SecureObject
{
  T object_;
  Mutex mutex_;
public:

  SecureObject( T const &object ) : object_( object ) {}

  /*
   *  You must lock the object before accessing it
   */
  void Lock() { mutex_.Acquire(); }

  /*
   *  You must unlock the object after locking it, not doing this may
   *  cause a deadlock.
   */
  void Unlock() { mutex_.Release(); }

  
  /*
   *  use Access in conjunction with lock and unlock for extended data
   *  manipulation.  Get and Set will handle locking the mutex on their
   *  own and are ideal for brief use of the object.
   *
   *  Use Access when you want to collect the data, operate on it, and
   *  then replace it.
   *
   *  Example:
   *    Imagine a program that uses a thread to manipulate a string while the
   *    main program capitalizes whatever is in that string.
   *
   *    SecureObject< std::string > globalSecureStr( "you should try the language lisp" );
   *
   *    main
   *      
   *      start( string thread );
   *      // capitalize the string
   *      while( true )
   *        std::string str = globalSecureStr.Get();
   *        ToUpper( str );
   *        globalSecureStr.Set( str );
   *
   *
   *    string thread
   *    
   *        std::string newStr = "you should try the language python";
   *        globalSecureStr.Set( newStr );
   *        
   *
   *    what we would like is for the string to change like this
   *
   *    "you should try the language lisp"
   *    "YOU SHOULD TRY THE LANGUAGE LISP"
   *    "you should try the language python"
   *    "YOU SHOULD TRY THE LANGUAGE PYTHON"
   *
   *    but it's possible for the following to happen
   *
   *    "you should try the language lisp"
   *    "you should try the language python"
   *    "YOU SHOULD TRY THE LANGUAGE LISP"
   *
   *    because it may happen that the main thread collects the string
   *    while it still has the initial value, then the thread sets the
   *    secure object, and then the main thread puts its modified value
   *    back in over the top of the new string clobbering the string
   *    thread's data
   *
   *    main should look like this
   *
   *      while( true )
   *        globalSecureStr.Lock();
   *        std::string &str = globalSecureStr.Access();
   *        ToUpper( str );
   *        globalSecureStr.Unlock();
   *
   */

  T &Access( void )
  {
    return object_;
  }

  /*
   *  Use Get and set for single line data manipulation.  It is very important
   *  that when doing extended data manipulation that depends on the crrent
   *  state of the data that you use Lock, Access, Unlock.
   */
  T Get( void )
  {
    mutex_.Acquire();
    T object = object_;
    mutex_.Release();
    
    return object;
  }

  void Set( T const &object )
  {
    mutex_.Acquire();
    object_ = object;
    mutex_.Release();
  }

  ~SecureObject() {}
};
