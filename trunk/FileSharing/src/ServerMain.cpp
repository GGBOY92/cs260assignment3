/*!
 *   @File   Main.cpp
 *   @Author Steven Liss
 *   @Date   30 Sept 2010
 *   @Brief  chess server main, controls the flow of the server application.
 */

#include "shared.hpp"

//#include "GameControl.hpp"

int main( int argc, char *argv[] )
{
  /*
  GameControl controller;

  try
  {
    controller.Init();

    try
    {
      controller.Run();
    }
    catch( BaseErrExcep e )
    {
      e.Print();
    }

    controller.Close();
  }
  catch( BaseErrExcep e )
  {
    e.Print();
  }
  */
   
  printf( "\n Server is closing \n" );

  return WaitForInput();
}
