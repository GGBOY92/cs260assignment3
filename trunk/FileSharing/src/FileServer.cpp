/*!
 *   @File   FileServer.cpp
 *   @Author Michael Travaglione
 *   @Date   11 November 2010
 *   @Brief  Implementation of object containing the sever which maintains the file transfer application 
 */

#include "FileServer.hpp"
#include "shared.hpp"   // BaseErrExcep

#include <iostream>

void FileServer::Init( void )
{
    try
    {
        server_.Init();
    }
    catch(BaseErrExcep& e)
    {
        throw e;
    }
}

/////////////////////////////////////

void FileServer::Run(void)
{
    bool run = true;
    try
    {
        while(run)
        {
            server_.Update();
            std::deque<NetworkMessage> currentMessages;
            server_.GetMessages(currentMessages);

            for(std::deque<NetworkMessage>::iterator it = currentMessages.begin(); 
                it != currentMessages.end(); ++it)
            {
                switch(it->type_)
                {
                case NetworkMessage::JOIN:
                    {
                        MsgJoin* pJoin = reinterpret_cast<MsgJoin*>(it->msg_.Bytes());
                  
                        u32 numFiles = pJoin->data_.fileCount_;
                        for(unsigned i = 0; i < numFiles; ++i)
                        {
                            masterFileList_.insert(std::make_pair(pJoin->data_.files_[i].fileName_, pJoin->data_.udpAddr_));
                            std::cout << pJoin->data_.files_[i].fileName_ << std::endl;
                        }

                        return;

                        //NetworkMessage msg;
                        //msg.conID_ = it->conID_;



                        //server_.SendMessage(msg);
                    }
                break;
                }
            }
        }
    }
    catch(BaseErrExcep& e)
    {
        throw e;
    }
}

/////////////////////////////////////

void FileServer::Close(void)
{
    try
    {
        server_.Close();
    }
    catch(BaseErrExcep& e)
    {
        throw e;
    }
}