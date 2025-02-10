#pragma once

/**
 * @brief Enumeration of status/error codes.
 */
enum class status {
    OK = 0,
    WSA_STARTUP_FAILED = -1,
    WSA_CLEANUP_FAILED = -2,
    SOCKET_CREATE_FAILED = -3,
    INVALID_ADDRESS = -4,
    SOCKET_BIND_FAILED = -5,
    SOCKET_LISTEN_FAILED = -6,
    SOCKET_CONNECT_FAILED = -7
};
