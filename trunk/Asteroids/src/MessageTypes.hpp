/*!
 *   @File   FileTransfer.hpp
 *   @Author Michael Travaglione
 *   @Date   20 Sept 2010
 *   @Brief  Common data storage objects needed by both server and client applications
 */
#pragma once

#include "SocketLibrary.hpp"   // network message types

const unsigned MAX_USERNAME_LEN = 20;

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
        u32 udpPort_;
    };

    Data data_;
    MsgType type_;
};
struct MsgInput 
{
    enum KeyState { KEY_DOWN, KEY_TRIGGERED };
    struct Data
    {
        u32 input;
        KeyState state;
    };

    Data data_;
    MsgType type_;
};
