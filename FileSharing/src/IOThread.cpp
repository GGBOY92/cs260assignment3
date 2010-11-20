
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

/*
  m_history_count = 10;
  m_message_history.SetCapacity( m_history_count );
  AllocConsole();

  inBuffer = GetStdHandle( STD_INPUT_HANDLE );
  outBuffer = GetStdHandle( STD_OUTPUT_HANDLE );
  
  m_input_count = 0;
  ReadConsoleInput( inBuffer, inputs, 100, &m_input_count );
*/

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
   //   CheckForInput();
   //   ProcInput();
   //   Sleep( 100 );


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
  //StartPrint();

  va_list argList;
  va_start(argList, msg);

  vprintf(msg, argList);

  va_end(argList);

  //EndPrint();
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


void IOObject::CheckForInput( void )
{
    /*
    int isInput = kbhit();

    if( isInput > 0 )
    {
        getline( , )
    }
    */

    m_input_count = 0;
    PeekConsoleInput( inBuffer, inputs, 100, &m_input_count );

    if( m_input_count > 0 )
    {
        ReadConsoleInput( inBuffer, inputs, 100, &m_input_count );
        TranslateInputs();
    }
}


void IOObject::TranslateInputs( void )
{
    for( DWORD i = 0; i < m_input_count; ++i )
    {
        INPUT_RECORD in = inputs[ i ];

        ProcInputRecord( in );
    }

    m_input_count = 0;
}


void IOObject::ProcInput( void )
{
    size_t pos = 0;
    while( ( pos = m_input_string.find_first_of( '\n' ) ) > 0 )
    {
        std::string inMessage;
        inMessage.assign( m_input_string, 0, pos );
        inMessage.erase( 0, pos );
        m_message_history.push_front( inMessage );
    }
}

void IOObject::ProcInputRecord( INPUT_RECORD const &in )
{
    switch( in.EventType )
    {
    case KEY_EVENT:
    {
        KEY_EVENT_RECORD key_info = in.Event.KeyEvent;

        if( key_info.bKeyDown == TRUE )
        {
            m_input_string.append( key_info.uChar.AsciiChar, key_info.wRepeatCount );
        }

        break;
    }
    default:
        break;
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
