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
#include "IOThread.hpp"

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
    clientSock_.SetPortNumber(0);
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
        connectedToServer_ = false;
        while(!connectedToServer_)
        {
            if(clientSock_.Connect(remoteAddr_))
            {
                IOObject::console.Wake();
                IOObject::console.Print("Connnection to Server established...\n Port: %d\n Server IP: %s\n",
                                         config_.serverPort_, config_.ip_.c_str());
                IOObject::console.Print("\nType '/help' for a list of available comands.\n\n");
                IOObject::console.Prompt();
                connectedToServer_ = true;
            }
            else
            {
                if(PromptYesNo("Unable to establish connection to server. Would you like to retry?"))
                    continue;
                else
                    break;

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
        if(fCount >= MAX_FILES)
            break;
    }

    if(fCount)
    {
        join.type_ = NetworkMessage::JOIN;
        join.data_.fileCount_ = fCount;
        join.data_.udpAddr_.SetIP(localIP_);
        join.data_.udpAddr_.SetPortNumber(config_.udpPort_);

        NetworkMessage netMsg;
        netMsg << join;
        clientSock_.Send(netMsg);
    }
}

///////////////////////////////////////////

void FileClient::Run( void )
{
    run_ = true;
    while(run_)
    {
        try
        {
            std::string input;
            if(IOObject::console.GetMessages(input))
            {
                ProcInput(input);
                IOObject::console.Prompt();
            }

            if(connectedToServer_)
            {
                NetworkMessage netMessage;
                if(clientSock_.Receive(netMessage))
                {
                    ProcMessage(netMessage);
                    IOObject::console.Prompt();
                }
            }
        }
        catch( iSocket::SockErr e )
        {
            if(e.eCode_ == WSAECONNRESET)
            {
                IOObject::console.StartPrint();
                IOObject::console.Print("===========================\n");
                IOObject::console.Print("Connection to server lost.\n");
                IOObject::console.Print("===========================\n");
                IOObject::console.EndPrint();
                connectedToServer_ = false;
            }
        }
    }
}

///////////////////////////////////////////

void FileClient::Close(void)
{
    IOObject::console.Print("\nExiting server...\n\n");
    clientSock_.Close();
}

///////////////////////////////////////////

void FileClient::ProcInput( std::string& input )
{
    if(input == "/show")
    {
        if(connectedToServer_)
        {
            NetworkMessage request;
            request.type_ = NetworkMessage::REQ_FILES;

            clientSock_.Send(request);
        }
        else
            IOObject::console.Print("\nConnection to server lost - no additionl transfers possible.\n\n");
    }
    else if(input == "/help")
    {
        IOObject::console.StartPrint();

        IOObject::console.Print("\n/get [filename]  Request to transfer a file. Myst be a valid file on the server.");
        IOObject::console.Print("\n/help            Displays list of all valid commands and descriptions.");
        IOObject::console.Print("\n/quit            Exit server session.");
        IOObject::console.Print("\n/show            Request an updated list of files currently available from the server.\n\n");

        IOObject::console.EndPrint();
    }
    else if(input == "/quit")
        run_ = false;
    else
        IOObject::console.Print("\nUnknown command - type '/help' for valid commands.\n\n");

    //input.substr();
}

///////////////////////////////////////////

void FileClient::ProcMessage( NetworkMessage& msg )
{
    switch(msg.type_)
    {
    case NetworkMessage::SERVER_FILES:
        {            
            IOObject::console.StartPrint();

            IOObject::console.Print("\nAvailable files on server: \n");
            IOObject::console.Print("========================== \n");
            MsgServerFiles fileMsg;
            msg >> fileMsg;

            unsigned numFiles = fileMsg.data_.fileCount_;
            for(unsigned i = 0; i < numFiles; ++i)
                IOObject::console.Print("%s\n", fileMsg.data_.files_[i].fileName_);

            IOObject::console.Print("\n");
            IOObject::console.EndPrint();
        }
    break;
    }
}


