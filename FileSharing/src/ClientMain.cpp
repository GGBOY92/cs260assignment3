/*!
 *   @File   Main.cpp
 *   @Author Steven Liss
 *   @Date   21 Sept 2010
 *   @Brief  chess client main, controls the flow of the client application.
 */

//#include "GameClient.hpp"
#include <iostream>

u32 const static READ_BUFFER_SIZE = 256;

int main( int argc, char *argv[] )
{
  /*
  GameClient client;

  try
  {
    client.Init();

    try
    {
      try
      {
        client.Run();
      }
      catch( BaseErrExcep e )
      {
        e.Print();
      }
    }
    catch( BaseErrExcep e )
    {
      e.Print();
    }

    client.Close();
  }
  catch( BaseErrExcep e )
  {
    e.Print();
  }
  */

  return WaitForInput();
}
