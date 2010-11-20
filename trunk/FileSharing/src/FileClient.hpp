/*!
 *   @File   FileClient.hpp
 *   @Author Michael Travaglione
 *   @Date   11/11/2010
 *   @Brief  Interface for client which interacts with file transfer server
 */

#include <string>
#include <vector>
#include <hash_map>

#include "TCPSocket.hpp"
#include "UDPSocket.hpp"
#include "FileTransfer.hpp" // FileInfo

#include "Splitter.hpp"

class FileClient
{
private: // classes

    typedef std::pair< SocketAddress, FileSplitter > SplitPair;
    typedef stdext::hash_map< u32, SplitPair > SplitterMap;
    typedef stdext::hash_map< u32, FileJoiner > JoinerMap;

public:
     // load config file, initialize socket, set remote address
    void Init(void);
     // establish connection to server
    void ConnectToServer(void);
    void SendFileList(void);
    void Run(void);
    void Close(void);

private: // methods
    
    void ProcInput(std::string& input);
    void ProcMessage(NetworkMessage& msg);
    void UpdateTransfers( void );


private: // members
    bool run_;
    bool connectedToServer_;

    TCPSocket clientSock_;
    UDPSocket peerSock_;

    SocketAddress remoteAddr_;
    SocketAddress updAddr_;
    Config config_;
    FileName files_[MAX_FILES];

    char* localIP_;

    SplitterMap outgoingTransfers_;
    JoinerMap incomingTransfers_;

    u32 const static DEFAULT_CHUNK_SIZE = 5000;
    u32 m_curr_update;
    u32 m_update_count;
};
