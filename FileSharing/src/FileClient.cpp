/*!
 *   @File   FileClient.cpp
 *   @Author Michael Travaglione
 *   @Date   11/11/2010
 *   @Brief  Implementation of client for file transfer assignment.
 */

#include <iostream>

#include "shared.hpp"
#include "SocketInterface.hpp"
#include "SocketLibrary.hpp"

#include "FileClient.hpp"

void FileClient::Init( void )
{
    // initialize winsock
    try
    {
        StartWinSock();
    }
    catch(WSErr& e)
    {
        throw e;
    }

     // load config.ini file containing port and server ip
    config_.LoadConfigFile();

    GetLocalIP(localIP_);

     // set client port
    clientSock_.SetPortNumber(8012);
    clientSock_.SetIP(localIP_);

     // initialize socket used to connect to the server
    try
    {
        clientSock_.Init();
    }
    catch(iSocket::SockErr& e)
    {
        throw e;
    }

     // specify port and ip of remote connection, which will be the server
    remoteAddr_.SetIP(config_.ip_);
    remoteAddr_.SetPortNumber(config_.serverPort_);
}

///////////////////////////////////////////

void FileClient::ConnectToServer( void )
{
    try
    {
         // continue to try to connect to the server until the user quits
        bool connected = false;
        while(!connected)
        {
            if(clientSock_.Connect(remoteAddr_))
            {
                printf("Connnection to Server esatblished...\n Port: %d\n Server IP: %s\n",
                       config_.serverPort_, config_.ip_.c_str());
                connected = true;
            }
            else
            {
                std::cout << "Unable to establish connection with server.\nRetry? (YES/NO): ";
                std::string retry;
                std::cin >> retry;

                for(size_t i = 0; i < retry.length(); ++i)
                    retry[i] = toupper(retry[i]);

                if(retry == "YES")
                    continue;
                else
                {
                    clientSock_.Close();
                    break;
                }
            }
        }
    }
    catch(iSocket::SockErr e)
    {
        throw e;
    }
}

///////////////////////////////////////////

void FileClient::SendFileList( void )
{
    fileCollector_.SetDirectory(config_.sendPath_.c_str());
    fileCollector_.CollectFiles();

    MsgJoin join;
    FileInfo fInfo;
    unsigned fCount = 0;
    for(FileCollector::iterator it = fileCollector_.begin(); it != fileCollector_.end(); ++it)
    {
        strcpy(fInfo.fileName_, (*it).c_str());
        join.data_.files_[fCount] = fInfo;

        ++fCount;
    }


    join.type_ = NetworkMessage::JOIN;
    join.data_.fileCount_ = fCount;
    join.data_.udpAddr_.SetIP(localIP_);
    join.data_.udpAddr_.SetPortNumber(config_.udpPort_);

    NetworkMessage netMsg;
    netMsg << join;
    clientSock_.Send(netMsg);
}
