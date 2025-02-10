#include "tcp_client_server.h"


status TCPClient::Connect(const std::string& dst_addr, const uint16_t dst_port) {
    status st = WSAStartupIfNeeded();
    if (st != status::OK) {
        return st;
    }

    _sock = socket(AF_INET, SOCK_STREAM, 0);
    if (_sock < 0) {
        WSACleanupIfNeeded();
        return status::SOCKET_CREATE_FAILED;
    }

    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(dst_port);
    if (inet_pton(AF_INET, dst_addr.c_str(), &addr.sin_addr) != 1) {
        WSACleanupIfNeeded();
        return status::INVALID_ADDRESS;
    }

    if (connect(_sock, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        WSACleanupIfNeeded();
        return status::SOCKET_CONNECT_FAILED;
    }

    return status::OK;
}

int TCPClient::Send(char* buf, int len, int flags) {
    return send(_sock, buf, len, flags);
}

int TCPClient::Close() {
    return tcpft_closesocket(_sock);
}

tcpft_sock TCPClient::sock() {
    return _sock;
}

status TCPClient::WSAStartupIfNeeded() {
#ifdef _WIN32
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        return status::WSA_STARTUP_FAILED;
    }
#endif
    return status::OK;
}

status TCPClient::WSACleanupIfNeeded() {
#ifdef _WIN32
    if (WSACleanup() != 0) {
        return status::WSA_CLEANUP_FAILED;
    }
#endif
    return status::OK;
}
