/**************************************************************************************************/
/*!
\file   File.cpp
\author Steven Liss
\author Robert Onulak
\brief
  A simple file object that divides the path, filename, and extension.  The object can also
   be used to check a file's validity through FileOK. Also some shell wrappers are included.

  Copyright (C) 2010 DigiPen Institute of Technology.
  Reproduction or disclosure of this file or its contents without the
  prior written consent of DigiPen Institute of Technology is prohibited.

*/
/**************************************************************************************************/
#pragma once

#include <vector>
#include <string>

enum MakeDirError
{
    MDE_Ok = 0,             ///< Successfully created the directory.
    MDE_DirectoryExists,    ///< Directory already exists.
    MDE_InvalidDir,         ///< Invalid directory name.
    MDE_UnknownError        ///< Unknown error occurred!
};  // enum MkDirErrors

std::string StripExtension( const std::string &filename );

bool DirectoryExists( const char *pDirName );
MakeDirError MakeDirectory( const char *pDirName );

class FileCollector
{
public:
    //void IncludeSubdirectories( bool include );

    void SetDirectory( const char *pDirName );
    bool CollectFiles( void );

    typedef std::vector<std::string> NameVector;
    typedef NameVector::iterator                iterator;
    typedef NameVector::reverse_iterator        reverse_iterator;
    typedef NameVector::const_iterator          const_iterator;
    typedef NameVector::const_reverse_iterator  const_reverse_iterator;

          iterator begin( void )        { return filenames_.begin(); }
    const_iterator begin( void ) const  { return filenames_.begin(); }

          iterator end( void )          { return filenames_.end(); }
    const_iterator end( void ) const    { return filenames_.end(); }

          reverse_iterator rbegin( void )       { return filenames_.rbegin(); }
    const_reverse_iterator rbegin( void ) const { return filenames_.rbegin(); }

          reverse_iterator rend( void )         { return filenames_.rend(); }
    const_reverse_iterator rend( void ) const   { return filenames_.rend(); }

          std::string& operator[]( int index )       { return filenames_[index]; }
    const std::string& operator[]( int index ) const { return filenames_[index]; }

private:
    bool include_;

    std::string directory_;
    NameVector filenames_;


  static char toUnix(char c)
  {
    if(c == '\\')
      return '/';
    else
      return c;
  }

  static char toWindows( char c )
  {
    if ( c == '/' )
      return '\\';
    else
      return c;
  }

  void AllToWindows( void );


};  // FileCollector
