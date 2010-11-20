/*!
 *   @File   FileTransfer.hpp
 *   @Author Michael Travaglione
 *   @Date   20 Sept 2010
 *   @Brief  Common data storage object needed by both server and client applications
 */
#pragma once

#include "SocketLibrary.hpp"   // network message types
#include <vector>

#include "Splitter.hpp"

const unsigned MAX_FILENAME_LENGTH = 100;
const unsigned MAX_IP_LENGTH = 20;
const unsigned MAX_FILES = 30;

struct FileName 
{
    FileName(void)
    {
        memset(fileName_, 0, MAX_FILENAME_LENGTH);
    }

    char fileName_[MAX_FILENAME_LENGTH];
};

struct FileOwner
{
    SocketAddress sockAddr_;
    u32 conID_;
};

typedef std::vector<FileName> FileNameCont;

// Message types

struct MsgJoin
{
    struct Data 
    { 
        u32 fileCount_;
        FileName files_[MAX_FILES];
        SocketAddress udpAddr_;
    };

    Data data_;
    MsgType type_;
};

struct MsgGet
{
    struct Data
    {
        FileName name_;
        SocketAddress recvAddr_;
    };

    Data data_;
    MsgType type_;
};

// struct MsgQuit
// {
//     MsgQuit(void) : type_(NetworkMessage::QUIT) { }
//     struct Data { };
// 
//     Data data_;
//     MsgType type_;
// };
 
struct MsgTransfer
{
  MsgTransfer(void) : type_(NetworkMessage::TRANSFER) { }
  struct Data
  {
    iFileInfo::Chunk chunk_;
    u32 transferID_;
  };
 
  Data data_;
  MsgType type_;
};
 

struct MsgInformSender
{
  MsgInformSender(void) : type_(NetworkMessage::INFORM_SENDER) { }
  struct Data
  {
    u32 transferID_;
    char fileName_[MAX_FILENAME_LENGTH];
    SocketAddress recipient_;
  };

  Data data_;
  MsgType type_;
};
 

 struct MsgInformReceiver
 {
     MsgInformReceiver(void) : type_(NetworkMessage::INFORM_RECEIVER) { }
     struct Data
     {
        u32 transferID_;
        char fileName_[MAX_FILENAME_LENGTH];
        SocketAddress sender_;
     };
 
     Data data_;
     MsgType type_;
 };

struct MsgServerFiles
{
    struct Data 
    { 
        u32 fileCount_;
        FileName files_[MAX_FILES];
    };

    Data data_;
    MsgType type_;
};
