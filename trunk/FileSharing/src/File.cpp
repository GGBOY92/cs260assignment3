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

#include "File.hpp"

#include <cstring>
#include <algorithm>

#include <sys/stat.h>
#include <errno.h>

#include "Windows.h"
#include <direct.h>

std::string GetExtension( const std::string &filename )
{
    size_t pos = filename.find_last_of( '.' ) + 1;
    if( pos != std::string::npos )
    {
        return filename.substr( pos, filename.size() );
    }
    else
    {
        return std::string();
    }
}

std::string StripExtension( const std::string &filename )
{
    size_t pos = filename.find_last_of( '.' );
    if ( pos != std::string::npos )
    {
        return filename.substr( 0, pos );
    }
    else
    {
        return std::string();
    }
}

bool DirectoryExists( const char *pDirName )
{
    struct _stat64 info;
    int result = _stat64( pDirName, &info );

    if ( result == -1 )     // Problem collecting file information.
    {
        return false;
    }
    else
    {
          // Return if the stats of the file state that this file is a directory.
        return info.st_mode & S_IFDIR ? true : false;
    }
}

MakeDirError MakeDirectory( const char *pDirName )
{
    if ( !pDirName )
    {
        return MDE_InvalidDir;
    }

    if ( _mkdir( pDirName ) == 0 )
    {
        return MDE_Ok;
    }
    else
    {
        int result;
        _get_errno( &result );

        if ( result == EEXIST )
        {
            return MDE_DirectoryExists;
        }
        else
        {
            return MDE_UnknownError;
        }
    }
}

void FileCollector::SetDirectory( const char *pDirName )
{
    directory_.assign( pDirName );

    std::transform( directory_.begin(), directory_.end(), directory_.begin(),
        &toWindows );
}

bool FileCollector::CollectFiles( void )
{
    WIN32_FIND_DATA data;
    HANDLE hFind = FindFirstFile( directory_.c_str(), &data );

    if ( hFind == INVALID_HANDLE_VALUE ) 
    {
        return false;
    } 
    else 
    {
          // Remove the previously saved files.
        filenames_.clear();

        do
        {
              // If the file we are on is not a directory, add it to our container of filenames.
            if ( !( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
            {
              filenames_.push_back( std::string( data.cFileName ) );
            }
        } while ( FindNextFile( hFind, &data ) != 0 );
    
        DWORD dwError = GetLastError();
        FindClose( hFind );

        if ( dwError != ERROR_NO_MORE_FILES ) 
        {
            return false;
        }
        else
        {
            return true;
        }
    }
}
