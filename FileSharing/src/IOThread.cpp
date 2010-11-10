
/*!
 *   @File   IOOThread.cpp
 *   @Author Steven Liss
 *   @Date   20 Oct 2010
 *   @Brief  Thread object for printing and taking command line input.
 */

#include "IOThread.hpp"
#include "shared.hpp"

IOObject IOObject::console = IOObject();

void IOObject::InitThread( void )
{

}

void IOObject::Prompt( void )
{
  if( prompting_ )
    return;

  prompt_.Lock();
  bool &prompt = prompt_.Access();
  prompt = true;
  prompt_.Unlock();
}

void IOObject::Run( void )
{
  while( !isDying )
  {
    prompt_.Lock();
  	bool &prompt = prompt_.Access();

    if( prompt )
  	{
      std::string str;
  	  prompting_ = true;
  	  ::Prompt( str, "" );
  	  prompting_ = false;

  	  inQueue_.Lock();
      MessageQueue &inQ = inQueue_.Access();
      inQ.push_back( str );
      inQueue_.Unlock();
  	  prompt = false;
  	}

  	prompt_.Unlock();
  }
}

void IOObject::FlushThread( void )
{
  wait_.Lock();
  bool &wait( wait_.Access() );
  wait = false;
  wait_.Unlock();
}

bool IOObject::GetMessages( std::string &str )
{
  inQueue_.Lock();
  MessageQueue &inQ( inQueue_.Access() );
  if( inQ.size() > 0 )
  {
    str = inQ.front();
    inQ.pop_front();
    inQueue_.Unlock();

    return true;
  }

  inQueue_.Unlock();
  return false;
}

void IOObject::Resume( void )
{
  wait_.Lock();
  bool &wait( wait_.Access() );
  wait = false;
  wait_.Unlock();
}

void IOObject::Print( char const *msg, ... ) const
{
  StartPrint();

  va_list argList;
  va_start(argList, msg);

  vprintf(msg, argList);

  va_end(argList);

  EndPrint();
}

void IOObject::StartPrint( void ) const
{
  if( prompting_ )
  {
    printf( "\n" );
  }
}

void IOObject::EndPrint( void ) const
{
  if( prompting_ )
  {
    printf( "\n >" );
  }
}

/*
void IOObject::Wait( void )
{

  while( true )
  {
    wait_.Lock();
    bool &wait( wait_.Get() );

    if( !wait )
    {
      wait_.UnLock();
      break;
    }

    wait_.UnLock();

  }
}
*/
