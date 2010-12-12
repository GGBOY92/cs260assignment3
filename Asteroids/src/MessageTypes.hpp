/*!
 *   @File   FileTransfer.hpp
 *   @Author Michael Travaglione
 *   @Date   20 Sept 2010
 *   @Brief  Common data storage objects needed by both server and client applications
 */
#pragma once

#include "SocketLibrary.hpp"   // network message types

const unsigned MAX_USERNAME_LEN = 20;
const unsigned KEY_INFO_BUFF_LEN = 32;

enum KeyState { KEY_DOWN, KEY_TRIGGERED };

struct KeyInfo
{
    u32 key_;
    KeyState state_;
};

struct Username 
{
    Username(void) { memset(name_, 0, MAX_USERNAME_LEN); }
    char name_[MAX_USERNAME_LEN];
};

// Message types

struct MsgJoin
{
    struct Data 
    { 
        Username username_;
    };

    Data data_;
    MsgType type_;
};

struct MsgInput 
{
    MsgInput(void) : type_(NetworkMessage::INPUT) { }
    struct Data
    {
        Username username_;
        KeyInfo key_data_[KEY_INFO_BUFF_LEN];
        u32 key_info_count_;
    };

    Data data_;
    MsgType type_;
};

struct MsgPosUpdate
{
    u32 const static buf_size_ = 1600;

    struct Data
    {
        char inst_data_[ buf_size_ ];
        u32 inst_count_;
    };

    Data data_;
    MsgType type_;
};
