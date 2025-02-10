#include "file.h"

void FileReader::Open(const std::string& file_path) {
    _file.open(file_path, std::ios::binary);
    if (!_file.is_open()) {
        throw std::runtime_error("file not open");
    }
}
    
void FileReader::Read(std::string& buf) {
    std::stringstream sstr;
    sstr << _file.rdbuf();
    buf = sstr.str();
}

void FileReader::Close() {
    if (_file.is_open())
        _file.close();
}

bool FileReader::isEndOfFile() {
    return _file.eof();
}

void FileWriter::Open(const std::string& file_path) {
    _file.open(file_path, std::ios::binary);
    if (!_file.is_open()) {
        throw std::runtime_error("file not open");
    }
}

void FileWriter::Write(const std::string& buf) {
    _file << buf;
}

void FileWriter::Write(const char& buf) {
    _file << buf;
}

void FileWriter::Close() {
    if (_file.is_open())
        _file.close();
}
