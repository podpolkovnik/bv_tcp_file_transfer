#pragma once

#include <string>
#include <fstream>
#include <sstream>

/**
 * @brief Class for writing to a file.
 */
class FileWriter {
public:
    FileWriter() = default;
    ~FileWriter() { Close(); }

    /**
     * @brief Opens the file for writing.
     *
     * @param file_path Path to the output file.
     * @throws std::runtime_error if file cannot be opened.
     */
    void Open(const std::string& file_path);

    /**
     * @brief Writes a string to the file.
     *
     * @param buf String to write.
     */
    void Write(const std::string& buf);

    /**
     * @brief Writes a single character to the file.
     *
     * @param buf Character to write.
     */
    void Write(const char& buf);

    /**
     * @brief Closes the file.
     */
    void Close();

private:
    std::ofstream _file;
};

/**
 * @brief Class for reading a file.
 */
class FileReader {
public:
    FileReader() = default;
    ~FileReader() { Close(); }

    /**
     * @brief Opens the file at the given path.
     *
     * @param file_path Path to the file.
     * @throws std::runtime_error if file cannot be opened.
     */
    void Open(const std::string& file_path);

    /**
     * @brief Reads the entire file content into a string.
     *
     * @param buf String to store file contents.
     */
    void Read(std::string& buf);

    /**
     * @brief Closes the file.
     */
    void Close();

    /**
     * @brief Checks if the end of file has been reached.
     *
     * @return true if EOF, false otherwise.
     */
    bool isEndOfFile();

private:
    std::ifstream _file;
};
