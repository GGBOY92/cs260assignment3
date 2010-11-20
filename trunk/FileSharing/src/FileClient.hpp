/*!
 *   @File   FileClient.hpp
 *   @Author Michael Travaglione
 *   @Date   11/11/2010
 *   @Brief  Interface for client which interacts with file transfer server
 */

#include <string>
#include <vector>

#include "TCPSocket.hpp"
#include "FileTransfer.hpp" // FileName

class FileClient
{
public:
     // load config file, initialize socket, set remote address
    void Init(void);
     // establish connection to server
    void ConnectToServer(void);
    void SendFileList(void);
    void Run(void);
    void Close(void);

private:
    bool run_;
    bool connectedToServer_;

    TCPSocket clientSock_;
    SocketAddress remoteAddr_;
    SocketAddress updAddr_;
    Config config_;

    void ProcInput(std::string& input);
    void ProcMessage(NetworkMessage& msg);

    char* localIP_;
    
};
