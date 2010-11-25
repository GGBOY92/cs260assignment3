#pragma once

#include "CTypes.hpp"

#include "shared.hpp"

#include <string>
#include <deque>

class iFileInfo
{

public: // classes

    struct Chunk
    {
        u32 const static MAX_CHUNK_SIZE = 900;
        u32 m_chunk_count;
        u32 m_seq_number;
        u32 m_size;
        char m_data[ MAX_CHUNK_SIZE ];
//        DataBuffer m_data;

        bool operator< ( Chunk const &rhs )
        {
            if( m_seq_number < rhs.m_seq_number )
                return true;
            else
                return false;
        }
    };
 
public: // methods

    iFileInfo( void );

    u32 GetFileSize( void );

    void SetDirectory( std::string const &directory );
    void SetFilename( std::string const &file_name );
    void SetChunkSize( u32 chunk_size );
    bool IsGood( void );
    void Close( void );

    ~iFileInfo( void );

protected: // methods

    void CalculateChunkCount( void );

protected: // members

    std::string m_directory;
    std::string m_filename;
    u32 m_chunk_size;

    u32 m_curr_seq_number;
    FILE *m_p_file;
    u32 m_file_size;
    u32 m_chunk_count;
    u32 m_last_chunk_size;

};


class FileJoiner : public iFileInfo
{

private: // classes

    typedef std::deque< u32 > ChunkData;

public: // methods
    
    void SetFileSize( u32 file_size );

    bool PutChunk( Chunk const &chunk );
    bool IsFileComplete( void );

private: // methods

    void InitJoining( void );
    bool CheckChunkStatus( void );

private: // members

    ChunkData m_chunk_data;
    u32 m_curr_chunk_count;

};


class FileSplitter : public iFileInfo
{

public: // methods

    bool GetNextChunk( iFileInfo::Chunk &chunk );
    bool ChunkedAll( void );
    void InitChunking( void );

private: // methods
    
    void CalculateFileSize( void );
};
