/*!
 *   @File   shared.cpp
 *   @Author Steven Liss
 *   @Date   20 Sept 2010
 *   @Brief  A collection of routines and data shared by bothe the client and server application.
 */

#include "shared.hpp"
#include "SocketLibrary.hpp"

#include <algorithm>
#include <iostream>

void WaitForInput( void )
{
  char in = 0;
  printf( "enter anything to terminate " );
  scanf("%c", &in );
}

void Config::LoadConfigFile(void)
{
  FILE *f = fopen( "../config.ini", "rt" );

  if( f )
  {
      // port an ip
    char buffer[20] = { 0 };
    fscanf( f, "<server port> %d\n", &serverPort_ );
    fscanf( f, "<server IP> %s\n", buffer );
    ip_.assign(buffer);

    fscanf( f, "<client UDP port> %d\n", &udpPort_ );

     // read in path information
    char pathBuffer[MAX_PATH] = { 0 };
    fscanf(f, "<send path> %s\n", pathBuffer);
    sendPath_.assign(pathBuffer);
    memset(pathBuffer, 0, sizeof(char) * MAX_PATH);
    fscanf(f, "<receive path> %s", pathBuffer);
    recvPath_.assign(pathBuffer);

    fclose( f );
  }
  else
      printf("config.ini not found.\n");
}

void Prompt( std::string &inBuffer, char const *msg )
{
  printf( msg );
  printf( " >" );
  getline( std::cin, inBuffer );
}

bool PromptYesNo( char const *msg )
{
  std::string prompt;
  while( true )
  { 
    prompt = "";
    printf( msg );
    printf( " (y,n) " );
    getline( std::cin, prompt );

    std::transform(prompt.begin(), prompt.end(), prompt.begin(), tolower );
    
    if( prompt == "n" )
      return false;
    else if( prompt == "y" )
      return true;

  }
}

DataBuffer::DataBuffer()
{
  Zero();
}

DataBuffer::DataBuffer( char const *bytes, u32 len )
{
  Assign( bytes, len );
}

u32 DataBuffer::Assign( char const *bytes, u32 count )
{
  if( count < MAX_BUFFER_LEN )
  {
    memcpy( bytes_, bytes, count );
    len_ = count;

    // there was no overflow, all bytes were assigned
    return 0;
  }
  else
  {
    u32 overFlow = count - MAX_BUFFER_LEN;
    memcpy( bytes_, bytes, MAX_BUFFER_LEN );
    len_ = MAX_BUFFER_LEN;

#ifdef _DEBUG
    printf( "Data truncated by %d in DataBuffer::Assign\n", overFlow );
#endif

    return overFlow;
  }
}

u32 DataBuffer::Append( char const *bytes, u32 count )
{
  if( len_ + count < MAX_BUFFER_LEN )
  {
    memcpy( bytes_ + len_, bytes, count );
    len_ += count;

    // there was no overflow, all bytes were appended
    return 0;
  }
  else
  {
    count = MAX_BUFFER_LEN - len_;
    memcpy( bytes_ + len_, bytes, count );
    len_ += count;

#ifdef _DEBUG
    printf( "Data truncated by %d bytes in DataBuffer::Append\n", count );
#endif
  
    return count;
  }
}

void DataBuffer::Zero( void )
{
  memset( bytes_, 0, MAX_BUFFER_LEN );
  len_ = 0;
}

void DataBuffer::operator <<( NetworkMessage const &netMsg )
{
  u32 typeSize = sizeof( MsgType );
  Assign( (char *) &netMsg.type_, typeSize );
  Append( netMsg.msg_.Bytes(), netMsg.msg_.Size() );
}

void DataBuffer::Allign( u32 pos )
{
  if( pos >= len_ )
    // to do: error, attempt to copy unassigned memory
    return;

  memmove( bytes_, bytes_ + pos, len_ - pos );
}

