#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <mutex>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <atomic>
#include <thread>
#include <chrono>
#include <deque>

#include "tcpft.h"

/**
 * @brief Sender function.
 *
 * Reads a file and transmits it over TCP.
 *
 * @param file_path Path to the input file.
 */
void sender(const std::string& file_path) {
    try {
        FOSocket sock;
        // Connect to server at 127.0.0.1:55055
        sock.Connect("127.0.0.1", 55055);
        sock.Transmit(file_path);
        sock.Close();
    }
    catch (const std::runtime_error& e) {
        tcpft_logFatal(e.what());
    }
}

/**
 * @brief Receiver function.
 *
 * Accepts a TCP connection and receives a file.
 *
 * @param file_path Path to the output file.
 */
void receiver(const std::string& file_path) {
    try {
        FISocket sock;
        // Initialize server on 127.0.0.1:55055
        sock.Init("127.0.0.1", 55055);
        sock.Receive(file_path);
        sock.Close();
    }
    catch (const std::runtime_error& e) {
        tcpft_logFatal(e.what());
    }
}

/**
 * @brief Compares two files for equality.
 *
 * Opens both files in binary mode, compares their sizes and content in blocks.
 *
 * @param file1 Path to the first file.
 * @param file2 Path to the second file.
 * @return true if files are identical, false otherwise.
 */
bool compareFiles(const std::string& file1, const std::string& file2) {
    std::ifstream f1(file1, std::ios::binary | std::ios::ate);
    std::ifstream f2(file2, std::ios::binary | std::ios::ate);

    if (!f1.is_open() || !f2.is_open()) {
        return false;
    }

    std::ifstream::pos_type size1 = f1.tellg();
    std::ifstream::pos_type size2 = f2.tellg();
    if (size1 != size2) {
        return false;
    }

    f1.seekg(0, std::ios::beg);
    f2.seekg(0, std::ios::beg);

    const std::size_t buffer_size = 4096;
    std::vector<char> buffer1(buffer_size);
    std::vector<char> buffer2(buffer_size);

    while (f1 && f2) {
        f1.read(buffer1.data(), buffer_size);
        f2.read(buffer2.data(), buffer_size);

        std::streamsize bytes_read_1 = f1.gcount();
        std::streamsize bytes_read_2 = f2.gcount();

        if (bytes_read_1 != bytes_read_2) {
            return false;
        }
        if (std::memcmp(buffer1.data(), buffer2.data(), static_cast<std::size_t>(bytes_read_1)) != 0) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Main function.
 *
 * Starts the sender and receiver threads, then compares the input and output files.
 */
int main() {
    // Define input and output file paths.
    std::string in_path = ".\\test_in.txt";
    std::string out_path = ".\\test_out.txt";

    // Sender transmits the source file, receiver writes to the output file.
    std::thread st(sender, in_path);        // sender reads test_in.txt
    std::thread rt(receiver, out_path);     // receiver writes to test_out.txt

    st.join();
    rt.join();

    std::cout << "compareFiles: " << compareFiles(in_path, out_path) << std::endl;
    return 0;
}
