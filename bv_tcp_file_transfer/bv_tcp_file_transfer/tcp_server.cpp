#include "tcp_client_server.h"
#include "log.h"

status TCPServer::Init(const std::string& src_addr, uint16_t src_port) {
    tcpft_logInfo("starting tcp server...");
    status st = WSAStartupIfNeeded();
    if (st != status::OK) {
        return st;
    }

    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock < 0) {
        WSACleanupIfNeeded();
        return status::SOCKET_CREATE_FAILED;
    }

    // Set receive timeout
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    tcpft_setsockopt(_sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(src_port);
    if (inet_pton(AF_INET, src_addr.c_str(), &addr.sin_addr) != 1) {
        WSACleanupIfNeeded();
        return status::INVALID_ADDRESS;
    }

    if (bind(_sock, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        WSACleanupIfNeeded();
        return status::SOCKET_BIND_FAILED;
    }

    if (listen(_sock, 1) < 0) {
        WSACleanupIfNeeded();
        return status::SOCKET_LISTEN_FAILED;
    }

    tcpft_logInfo("tcp server started successfully");
    return status::OK;
}

tcpft_sock TCPServer::Accept() {
    return accept(_sock, nullptr, nullptr);
}

int TCPServer::Receive(tcpft_sock sock, char* buf, int len, int flags) {
    return recv(sock, buf, len, flags);
}

int TCPServer::Close() {
    return tcpft_closesocket(_sock);
}

tcpft_sock TCPServer::sock() {
    return _sock;
}

status TCPServer::WSAStartupIfNeeded() {
#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return status::WSA_STARTUP_FAILED;
    }
#endif
    return status::OK;
}

status TCPServer::WSACleanupIfNeeded() {
#ifdef _WIN32
    if (WSACleanup() != 0) {
        return status::WSA_CLEANUP_FAILED;
    }
#endif
    return status::OK;
}
