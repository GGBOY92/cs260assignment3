

#include <stdio.h>
#include <stdlib.h>

#include "splitter.hpp"


iFileInfo::iFileInfo() : m_chunk_size( 0 ), m_curr_seq_number( 0 ), m_p_file( NULL ), m_start_time( 0 )
{}


iFileInfo::~iFileInfo( void )
{}


void iFileInfo::Close( void )
{
    if( m_p_file )
        fclose( m_p_file );
}


void iFileInfo::SetChunkSize( u32 chunk_size )
{
    m_chunk_size = chunk_size;
}


void iFileInfo::SetDirectory( std::string const &directory )
{
    m_directory = directory;
}


void iFileInfo::SetFilename( std::string const &filename )
{
    m_filename = filename;
}


u32 iFileInfo::GetFileSize( void )
{
    return m_file_size;
}


void iFileInfo::CalculateChunkCount( void )
{
    m_chunk_count = m_file_size / m_chunk_size;
    m_last_chunk_size = m_file_size % m_chunk_size;

    if( m_last_chunk_size > 0 )
        ++m_chunk_count;
}

bool iFileInfo::IsGood( void )
{
    if( m_p_file == NULL )
        return false;
    else
        return true;
}

bool FileSplitter::GetNextChunk( iFileInfo::Chunk &chunk )
{
    if( m_p_file == NULL )
        InitChunking();

    if( m_p_file == NULL )
        return false;

    size_t bytes_read = fread( chunk.m_data, 1, m_chunk_size, m_p_file );

    if( ferror( m_p_file ) )
        printf( "error reading file" );

    chunk.m_size = bytes_read;
    chunk.m_file_size = m_file_size;
    chunk.m_seq_number = m_curr_seq_number++;
    chunk.m_chunk_count = m_chunk_count;

    if( bytes_read > 0 )
        return true;
    else
        return false;
}


void FileSplitter::InitChunking( void )
{
    std::string file = m_directory + m_filename;
    m_p_file = fopen( file.c_str(), "rb" );

    if( m_p_file == NULL )
        return;

    CalculateFileSize();
    CalculateChunkCount();
}


void FileSplitter::CalculateFileSize( void )
{
    if( fseek( m_p_file, 0, SEEK_END ) != 0 )
    {
        printf( "Error in FileSplitter::CalculateFileSize.  Error in seek operation.\n" );
        return;
    }

    int file_len = ftell( m_p_file );

    if( file_len == -1 )
    {
        printf( "Error in FileSplitter::CalculateFileSize.  Error in tell operation.\n" );
        return;
    }

    m_file_size = u32( file_len );

    fseek( m_p_file, 0, SEEK_SET );
}

bool FileSplitter::ChunkedAll( void )
{
    return ( m_curr_seq_number == m_chunk_count );
}

bool FileJoiner::IsFileComplete( void )
{
    return CheckChunkStatus();
}


bool FileJoiner::PutChunk( Chunk const &chunk )
{
    m_file_size = chunk.m_file_size;
    if( !m_p_file )
        InitJoining();

    u32 &chunk_status = m_chunk_data[ chunk.m_seq_number ];

    if( chunk_status > 0 )
        return false;



    fseek( m_p_file, chunk.m_seq_number * m_chunk_size, SEEK_SET );
    fwrite( chunk.m_data, 1, chunk.m_size, m_p_file );

    ++chunk_status;
    ++m_curr_chunk_count;

    return CheckChunkStatus();
}


void FileJoiner::InitJoining( void )
{
    std::string file = m_directory + m_filename;
    m_p_file = fopen( file.c_str(), "wb" );

    if( !m_p_file )
        return;

    fseek( m_p_file, m_file_size - 1, SEEK_SET );

    fputc( 0, m_p_file );

    CalculateChunkCount();

    m_chunk_data.resize( m_chunk_count, u32( 0 ) );

    m_curr_chunk_count = 0;
    m_start_time = timeGetTime();
}

void FileJoiner::SetFileSize( u32 file_size )
{
    m_file_size = file_size;
}

bool FileJoiner::CheckChunkStatus()
{
    if( m_curr_chunk_count == m_chunk_count )
        return true;
    else
        return false;
}
