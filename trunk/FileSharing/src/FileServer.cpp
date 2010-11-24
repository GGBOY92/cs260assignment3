/*!
 *   @File   FileServer.cpp
 *   @Author Michael Travaglione
 *   @Date   11 November 2010
 *   @Brief  Implementation of object containing the sever which maintains the file transfer application 
 */

#include "shared.hpp"   // BaseErrExcep

#include "FileServer.hpp"

unsigned FileServer::transID_ = 0;

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

            for(std::deque<NetworkMessage>::iterator it = currentMessages.begin(); it != currentMessages.end(); ++it)
               ProcMessage((*it));
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

             // add the files from the new client to the master list - *assumes no duplicates*
            u32 numFiles = join.data_.fileCount_;
            FileOwner owner;
            owner.conID_ = msg.conID_;
            owner.sockAddr_ = join.data_.udpAddr_;
            for(unsigned i = 0; i < numFiles; ++i)
            {
                masterFileList_.insert(std::make_pair(join.data_.files_[i].fileName_, owner));
                fileNames_[msg.conID_].push_back(join.data_.files_[i].fileName_);
            }

            printf("\nNew user connected. %d files added to server.\n\n", numFiles);
            printf("Available files from all clients:\n");
            printf("=================================\n");
            PrintMasterList();

            SendMasterList(msg.conID_);
        }
        break;
    case NetworkMessage::REQ_FILES: 
        printf("\nClient requesting file list...\n\n");
        SendMasterList(msg.conID_);
    break;
    case NetworkMessage::QUIT:
            printf("\nClient is quitting server...\n\n", fileNames_[msg.conID_].size());
    break;
    case NetworkMessage::DISCON:
        RemoveFiles(msg.conID_);

        printf("\nLost connection to client. %d files removed.\n\n", fileNames_[msg.conID_].size());
        printf("Updated list of files: \n");
        printf("====================== \n\n");
        PrintMasterList();
    break;
    case NetworkMessage::GET:
        {
            MsgGet get;
            msg >> get;
       
            FileCont::iterator it = masterFileList_.find(get.data_.name_.fileName_);
            if(it != masterFileList_.end())
            {            
                FileOwner fileSender;
                fileSender = it->second;
                
                MsgInformReceiver informRecv;
                informRecv.data_.sender_ = fileSender.sockAddr_;
                informRecv.data_.transferID_ = transID_;
                informRecv.type_ = NetworkMessage::INFORM_RECEIVER;

                NetworkMessage recvMsg;
                recvMsg.conID_ = msg.conID_;
                recvMsg << informRecv;
                server_.SendMessage(recvMsg);

                MsgInformSender informSender;
                strcpy(informSender.data_.fileName_, get.data_.name_.fileName_);
                informSender.data_.recipient_ = get.data_.recvAddr_;
                informSender.data_.transferID_ = transID_;
                informSender.type_ = NetworkMessage::INFORM_SENDER;

                NetworkMessage sendMsg;
                sendMsg.conID_ = fileSender.conID_;
                sendMsg << informSender;
                server_.SendMessage(sendMsg);

                ++transID_;
            }



        }
    break;
    }
}

/////////////////////////////////////

void FileServer::SendMasterList(u32 conID)
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
    newMsg.conID_ = conID;

     // convert to known type
    newMsg << serverFiles;
     // send to the client
    server_.SendMessage(newMsg);
}

/////////////////////////////////////

void FileServer::RemoveFiles(u32 conID)
{
    FileNameCont::iterator it = fileNames_.find(conID);
    if(it != fileNames_.end())
    {
        std::vector<std::string>& names = it->second;
        for(std::vector<std::string>::iterator it = names.begin(); it != names.end(); ++it)
            masterFileList_.erase((*it));
    }
}

/////////////////////////////////////

void FileServer::PrintMasterList(void)
{
    for(FileCont::iterator it = masterFileList_.begin(); it != masterFileList_.end(); ++it)
        printf("%s\n", it->first.c_str());

    printf("\n");
}
