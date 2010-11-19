/*!
 *   @File   FileServer.hpp
 *   @Author Steven Liss
 *   @Date   11 November 2010
 *   @Brief  Inteface for object containing the sever which maintains the file transfer application.
 */

#pragma once

#include <hash_map>
 
#include "FileTransfer.hpp"
#include "Server.hpp"  

class FileServer 
{
public:
    void Init(void);
    void Run(void);
    void Close(void);

    typedef stdext::hash_map<std::string, SocketAddress> FileContainer;

private:
    Server server_;
    FileContainer masterFileList_;
};
