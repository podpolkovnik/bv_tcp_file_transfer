#pragma once

#include "buffer.h"
#include "tcp_client_server.h"

#include <stdint.h>
#include <string>

/**
 * @brief Socket-based file receiver/transmitter base class.
 *
 */
class FSocket {
public:
    virtual ~FSocket() = default;

    /**
     * @brief Closes the socket.
     *
     * @return int Result of closing.
     */
    virtual int Close() = 0;

protected:
    Pool& pool() { return _pool; }

private:
    Pool _pool;
};

/**
 * @brief Socket-based file receiver.
 *
 * Receives data over TCP and writes it to a file using a FileWriterWorker.
 */
class FISocket : public FSocket {
public:
    FISocket() = default;
    ~FISocket() { Close(); }

    /**
     * @brief Initializes the server socket.
     *
     * @param src_addr Source IP address.
     * @param src_port Source port.
     * @return status Error status.
     */
    status Init(const std::string& src_addr, const uint16_t src_port);

    /**
     * @brief Receives a file and writes it to the specified location.
     *
     * @param location Path to the output file.
     */
    void Receive(const std::string& location);

    /**
     * @brief Closes the server socket.
     *
     * @return int Result of closing.
     */
    int Close() override;

private:
    TCPServer _server;
};

/**
 * @brief Socket-based file transmitter.
 *
 * Reads a file and sends its data over TCP using a FileReaderWorker.
 */
class FOSocket : public FSocket {
public:
    FOSocket() = default;
    ~FOSocket() { Close(); }

    /**
     * @brief Connects to the destination server.
     *
     * @param dst_addr Destination IP address.
     * @param dst_port Destination port.
     * @return status Error status.
     */
    status Connect(const std::string& dst_addr, const uint16_t dst_port);

    /**
     * @brief Transmits the file located at the given path.
     *
     * @param location Path to the input file.
     */
    void Transmit(const std::string& location);

    /**
     * @brief Closes the client socket.
     *
     * @return int Result of closing.
     */
    int Close() override;

private:
    TCPClient _client;
};
