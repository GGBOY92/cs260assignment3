/*!
 *   @File   FileTransfer.hpp
 *   @Author Michael Travaglione
 *   @Date   20 Sept 2010
 *   @Brief  Common data storage object needed by both server and client applications
 */
#pragma once

#include "SocketLibrary.hpp"   // network message types

struct FileInfo
{
    std::string fileName_;
    u32 conID_;
    int port_;
    std::string ip_;
};

// Message types

struct MsgJoin
{
    MsgJoin(void) : type_(NetworkMessage::JOIN) { }
    struct Data { };

    Data data_;
    MsgType type_;
};

struct MsgQuit
{
    MsgQuit(void) : type_(NetworkMessage::QUIT) { }
    struct Data { };

    Data data_;
    MsgType type_;
};

struct MsgTransfer
{
    MsgTransfer(void) : type_(NetworkMessage::TRANSFER) { }
    struct Data { };

    Data data_;
    MsgType type_;
};

struct MsgInformSender
{
    MsgInformSender(void) : type_(NetworkMessage::INFORM_SENDER) { }
    struct Data { };

    Data data_;
    MsgType type_;
};

struct MsgInformReceiver
{
    MsgInformReceiver(void) : type_(NetworkMessage::INFORM_RECEIVER) { }
    struct Data { };

    Data data_;
    MsgType type_;
};