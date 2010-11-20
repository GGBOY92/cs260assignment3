
/*!
 *   @File   IOOThread.hpp
 *   @Author Steven Liss
 *   @Date   20 Oct 2010
 *   @Brief  Thread object for printing and taking command line input.
 */

#pragma once

#include <string>
#include <deque>

#include "ActiveObject.hpp"
#include "SecureObject.hpp"
#include "LimitedQueue.hpp"

class IOObject : public ActiveObject
{
public:

  typedef std::deque< std::string > MessageQueue; 
  bool IsPrompting( void ) { return prompting_; }

private:

  SecureObject< MessageQueue > inQueue_;
  SecureObject< bool > wait_;
  SecureObject< bool > prompt_;

  bool prompting_;

public:

  IOObject() :
      ActiveObject(), inQueue_( MessageQueue() ), wait_( false ), prompt_( false ), prompting_( false )
  {}

  virtual void InitThread( void );
  virtual void Run( void );
  virtual void FlushThread( void );

  void Resume( void );
  void Prompt( void );

  void StartPrint( void ) const;
  void EndPrint( void ) const;
  
  void Print( char const *msg, ... ) const;

  bool GetMessages( std::string &str );
  void CheckForInput( void );
  void TranslateInputs( void );
  void ProcInput( void );
  void ProcInputRecord( INPUT_RECORD const &in );
  IOObject static console;

  u32 m_history_count;

  std::string m_input_string;
  LimitedQueue< std::string > m_message_history;

  // windows

  INPUT_RECORD inputs[ 1000 ];

  HANDLE outBuffer;
  HANDLE inBuffer;
  DWORD m_input_count;
};
