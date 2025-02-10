#pragma once

#include "status.h"

#include <stdint.h>
#include <string>

#define NOMINMAX

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <netinet/in.h>
#include <unistd.h>
#endif

#ifdef _WIN32
using tcpft_sock = SOCKET;
#define tcpft_closesocket closesocket
#define tcpft_setsockopt(socket, level, optname, optval, optlen) setsockopt(socket, level, optname, (const char*)(optval), optlen)
#else
using tcpft_sock = int;
#define tcpft_closesocket close
#define tcpft_setsockopt(socket, level, optname, optval, optlen) setsockopt(socket, level, optname, optval, optlen)
#endif

/**
 * @brief TCP Server class for accepting incoming connections.
 */
class TCPServer {
public:
    /**
     * @brief Constructs a TCPServer.
     */
    explicit TCPServer() : _sock(-1) {}
    ~TCPServer() { Close(); }

    /**
     * @brief Initializes the server with the source address and port.
     *
     * @param src_addr Source IP address.
     * @param src_port Source port.
     * @return status Error status.
     */
    status Init(const std::string& src_addr, uint16_t src_port);

    /**
     * @brief Accepts an incoming connection.
     *
     * @return tcpft_sock The accepted socket.
     */
    tcpft_sock Accept();

    /**
     * @brief Receives data from a socket.
     *
     * @param sock The socket to receive from.
     * @param buf Buffer to store the received data.
     * @param len Maximum length to receive.
     * @param flags Flags for recv().
     * @return int Number of bytes received.
     */
    int Receive(tcpft_sock sock, char* buf, int len, int flags);

    /**
     * @brief Closes the server socket.
     *
     * @return int Result of closing operation.
     */
    int Close();

    /**
     * @brief Returns the server socket.
     *
     * @return tcpft_sock The server socket.
     */
    tcpft_sock sock();

private:
    tcpft_sock _sock;

private:
    status WSAStartupIfNeeded();
    status WSACleanupIfNeeded();
};

/**
 * @brief TCP Client class for connecting to a remote server.
 */
class TCPClient {
public:
    explicit TCPClient() : _sock(-1) {}
    ~TCPClient() { Close(); }

    /**
     * @brief Connects to the given destination address and port.
     *
     * @param dst_addr Destination IP address.
     * @param dst_port Destination port.
     * @return status Error status.
     */
    status Connect(const std::string& dst_addr, const uint16_t dst_port);

    /**
     * @brief Sends data over the connected socket.
     *
     * @param buf Pointer to data buffer.
     * @param len Length of data.
     * @param flags Flags for send().
     * @return int Number of bytes sent.
     */
    int Send(char* buf, int len, int flags);

    /**
     * @brief Closes the client socket.
     *
     * @return int Result of closing operation.
     */
    int Close();

    /**
     * @brief Returns the client socket.
     *
     * @return tcpft_sock The client socket.
     */
    tcpft_sock sock();

private:
    tcpft_sock _sock;

private:
    status WSAStartupIfNeeded();
    status WSACleanupIfNeeded();
};