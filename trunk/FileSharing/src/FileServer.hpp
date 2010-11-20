/*!
 *   @File   FileServer.hpp
 *   @Author Steven Liss
 *   @Date   11 November 2010
 *   @Brief  Inteface for object containing the sever which maintains the file transfer application.
 */

#pragma once

#include <hash_map>
#include <string>
#include <vector>
 
#include "FileTransfer.hpp"
#include "Server.hpp"  

class FileServer 
{
public:
    void Init(void);
    void Run(void);
    void Close(void);

    typedef stdext::hash_map<std::string, FileOwner> FileCont;
    typedef stdext::hash_map<u32, std::vector<std::string> > FileNameCont;

private:
    static unsigned transID_;

    Server server_;
    FileCont masterFileList_;
    FileNameCont fileNames_;

    void ProcMessage(NetworkMessage& msg);
    void SendMasterList(u32 conID);
    void RemoveFiles(u32 conID);
    void PrintMasterList(void);
};
