/*!
 *   @File   FileClient.hpp
 *   @Author Michael Travaglione
 *   @Date   11/11/2010
 *   @Brief  Interface for client which interacts with file transfer server
 */

#include <vector>

#include "File.hpp"         // File collector
#include "TCPSocket.hpp"
#include "FileTransfer.hpp" // FileInfo

class FileClient
{
public:
     // load config file, initialize socket, set remote address
    void Init(void);
     // establish connection to server
    void ConnectToServer(void);
    void SendFileList(void);
    void Run(void);

private:
    TCPSocket clientSock_;
    SocketAddress remoteAddr_;
    SocketAddress updAddr_;
    FileCollector fileCollector_;
    Config config_;

    char* localIP_;
    
};
