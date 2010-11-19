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
               ProcMessage((*it));
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

/////////////////////////////////////

void FileServer::ProcMessage(NetworkMessage& msg)
{
    switch(msg.type_)
    {
    case NetworkMessage::JOIN:
        {
             // convert network message into known type
            MsgJoin join;
            msg >> join;

            // add the files from the new client to the master list
            u32 numFiles = join.data_.fileCount_;
            for(unsigned i = 0; i < numFiles; ++i)
            {
                masterFileList_.insert(std::make_pair(join.data_.files_[i].fileName_, join.data_.udpAddr_));
                std::cout << join.data_.files_[i].fileName_ << std::endl;
            }

            // need to send the new client an updated list of available files...

            // set up the message to send
            MsgServerFiles serverFiles;
            // number of file names to send
            serverFiles.data_.fileCount_ = masterFileList_.size();
            // type of message
            serverFiles.type_ = NetworkMessage::SERVER_FILES;

            // copy each filename on the master list into the data of the message
            unsigned i = 0;
            for(FileCont::iterator it2 = masterFileList_.begin(); it2 != masterFileList_.end(); ++it2)
            {
                strcpy(serverFiles.data_.files_[i].fileName_, it2->first.c_str());
                ++i;
            }

            // now need to convert the message into a generic network message
            NetworkMessage newMsg;
            // set the connection id so the server knows who to send to
            newMsg.conID_ = msg.conID_;

            // convert to known type
            newMsg << serverFiles;
            // send to the client
            server_.SendMessage(newMsg);
        }
        break;
    case NetworkMessage::REQ_FILES:
        {
            // set up the message to send
            MsgServerFiles serverFiles;
            // number of file names to send
            serverFiles.data_.fileCount_ = masterFileList_.size();
            // type of message
            serverFiles.type_ = NetworkMessage::SERVER_FILES;

            // copy each filename on the master list into the data of the message
            unsigned i = 0;
            for(FileCont::iterator it2 = masterFileList_.begin(); it2 != masterFileList_.end(); ++it2)
            {
                strcpy(serverFiles.data_.files_[i].fileName_, it2->first.c_str());
                ++i;
            }

            // now need to convert the message into a generic network message
            NetworkMessage newMsg;
            // set the connection id so the server knows who to send to
            newMsg.conID_ = msg.conID_;

            // convert to known type
            newMsg << serverFiles;
            // send to the client
            server_.SendMessage(newMsg);
        }
    break;
    }
}
