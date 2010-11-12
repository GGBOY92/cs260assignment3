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

namespace SD
{

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
        &FileStat::toWindows );
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
                filenames_.push_back( data.cFileName );
            }
        } while ( FindNextFile( hFind, &data ) != 0 );
    
        DWORD dwError = GetLastError();
        FindClose( hFind );

        if ( dwError != ERROR_NO_MORE_FILES ) 
        {
            DebugPrint( "FindNextFile error. Error is %u.\n", dwError );

            return false;
        }
        else
        {
            return true;
        }
    }
}

void FileStatCollector::SetDirectory( const FileStat &fileStat )
{
    directory_ = fileStat.Full();

    directory_.AllToWindows();
}

bool FileStatCollector::CollectFiles( void )
{
    WIN32_FIND_DATA data;
    HANDLE hFind = FindFirstFile( directory_.Full(), &data );

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
                std::string filename = data.cFileName;
                std::string fileExt  = GetExtension( filename );
                filename = StripExtension( filename );
                FileStat fileStat( directory_.Path(), filename.c_str(), fileExt.c_str() );
                filenames_.push_back( fileStat );
            }
        } while ( FindNextFile( hFind, &data ) != 0 );
    
        DWORD dwError = GetLastError();
        FindClose( hFind );

        if ( dwError != ERROR_NO_MORE_FILES ) 
        {
            DebugPrint( "FindNextFile error. Error is %u.\n", dwError );

            return false;
        }
        else
        {
            return true;
        }
    }
}


/*!
 *  @brief  Construct a FileStat object from a single string.
 *
 *  @note  Use the other constructor if possible it's much faster
 *
 *  @param full  the full path, filename, extension string.  path/path/filename.extension
 */
FileStat::FileStat(char const *full)
{
  char const *pathEnd = full;
  char const *filenameBegin = full;
  char const *filenameEnd = full;
  char const *extensionBegin = full;
  char const *end = strrchr(full, 0);

  //copy the full path including the NULL terminator
  std::transform(full, end + 1, full_, &toUnix );

  //find the end of the path portion
  pathEnd = strrchr(full, '/' );
  if( ! pathEnd )
  { 
    pathEnd = strrchr(full, '\\' );
    if( ! pathEnd )
    {
      //if there is no path set pathEnd to beginning of the string
      pathEnd = full;
    }
    else
      filenameBegin = pathEnd + 1;
  }
  else
    filenameBegin = pathEnd + 1;

  //copy the path portion into path_ and NULL terminate it
  std::transform(full, pathEnd, path_, &toUnix );
  path_[pathEnd - full] = 0;

  //find the extension
  filenameEnd = strrchr(pathEnd, '.');
  if( ! filenameEnd )
  {
    //if there isn't one NULL terminate it
    filenameEnd = end;
    ext_[0] = 0;
  }
  else
  {
    //if there is one copy it and then NULL terminate it
    extensionBegin = filenameEnd + 1;
    strncpy(ext_, extensionBegin, end - extensionBegin );
    ext_[end - extensionBegin] = 0;
  }
  
  //copy the filename and NULL terminate it
  strncpy(file_, filenameBegin, filenameEnd - filenameBegin);
  file_[filenameEnd - filenameBegin] = 0;
}


/*!
 *  @brief  Construct a FileStat object from seperate path, filename, and extension
 *
 *  @param path  the path to the file
 *  @param file  the file's name
 *  @param ext  the file's extension
 */
FileStat::FileStat(char const * const &path, char const * const &file, char const * const &ext )
{
  strcpy(path_, path );
  std::transform(path, path + strlen(path ) + 1, path_, &toUnix );
  strcpy(file_, file );
  strcpy(ext_, ext );

  strcpy(full_, path_ );
  strcat(full_, "/");
  strcat(full_, file_ );
  strcat(full_, ".");
  strcat(full_, ext_ );
}

/*!
 *  @brief  Check to see if a file path is valid.
 *
 *  @return  True if the file is valid false otherwise.
 */
bool FileStat::FileOK() const
{
  FILE *f = fopen(full_, "r");

  if(f)
  {
    fclose(f);
    return true;
  }
  else
    return false;
}

void FileStat::AllToWindows()
{
  std::transform( full_, full_ + MAX_FULL_LEN, full_,
      &FileStat::toWindows );

  std::transform( path_, path_ + MAX_PATH_LEN, path_,
      &FileStat::toWindows );

  std::transform( file_, file_ + MAX_FILE_LEN, file_,
      &FileStat::toWindows );

  std::transform( ext_, ext_ + MAX_EXT_LEN, ext_,
      &FileStat::toWindows );
}

}
