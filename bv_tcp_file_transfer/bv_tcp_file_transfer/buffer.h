#pragma once

#include <string>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <algorithm>

/**
 * @brief Thread-safe buffer class template.
 *
 * @tparam T Type of elements stored.
 * @tparam Size Maximum number of elements in the buffer.
 */
template <typename T, size_t Size>
class Buffer {
public:
    static const size_t size = Size;
    using value_type = T;

    Buffer(Buffer<T, Size>&& other) = delete;
    Buffer() = default;

    /**
     * @brief Copy constructor.
     */
    Buffer(const Buffer<T, Size>& other) {
        std::lock_guard<std::mutex> lock(mutex);
        _buf = other._buf;
        cv.notify_one();
    }

    /**
     * @brief Copy assignment operator.
     */
    Buffer<T, Size>& operator=(const Buffer<T, Size>& other) {
        std::lock_guard<std::mutex> lock(mutex);
        _buf = other._buf;
        cv.notify_one();
        return *this;
    }

    /**
     * @brief Push an element into the buffer.
     *
     * If the buffer is full, the oldest element is removed.
     *
     * @param value The value to push.
     */
    void Push(const T& value) {
        std::lock_guard<std::mutex> lock(mutex);
        if (_buf.size() == Buffer::size) {
            _buf.pop_front();
        }
        _buf.push_back(value);
        cv.notify_one();
    }

    /**
     * @brief Returns a reference to the first element.
     *
     * @return T& Reference to the front element.
     */
    T& Front() {
        std::lock_guard<std::mutex> lock(mutex);
        return _buf.front();
    }

    /**
     * @brief Returns a reference to the last element.
     *
     * @return T& Reference to the back element.
     */
    T& Back() {
        std::lock_guard<std::mutex> lock(mutex);
        return _buf.back();
    }

    /**
     * @brief Removes and returns the first element.
     *
     * @return T The removed element.
     */
    T Pop() {
        std::lock_guard<std::mutex> lock(mutex);
        T item = _buf.front();
        _buf.pop_front();
        cv.notify_one();
        return item;
    }

    /**
     * @brief Returns the number of elements in the buffer.
     *
     * @return size_t Count of elements.
     */
    size_t Count() const {
        std::lock_guard<std::mutex> lock(mutex);
        return _buf.size();
    }

    // Waiting methods:
    void waitForFull() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return _buf.size() == Buffer::size; });
    }
    void waitForEmpty() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return _buf.empty(); });
    }
    void waitForNotFull() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return _buf.size() < Buffer::size; });
    }
    void waitForNotEmpty() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return !_buf.empty(); });
    }
    void waitForHalf() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return _buf.size() == Buffer::size / 2; });
    }
    void waitForAboveHalf() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return _buf.size() > Buffer::size / 2; });
    }
    void waitForBelowHalf() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this] { return _buf.size() < Buffer::size / 2; });
    }

    /**
     * @brief Checks if the buffer is empty.
     *
     * @return true if empty, false otherwise.
     */
    bool isEmpty() const {
        std::lock_guard<std::mutex> lock(mutex);
        return _buf.empty();
    }

    /**
     * @brief Checks if the buffer is full.
     *
     * @return true if full, false otherwise.
     */
    bool isFull() const {
        std::lock_guard<std::mutex> lock(mutex);
        return _buf.size() == Buffer::size;
    }

    bool isHalf() const {
        std::lock_guard<std::mutex> lock(mutex);
        return _buf.size() == Buffer::size / 2;
    }
    bool isAboveHalf() const {
        std::lock_guard<std::mutex> lock(mutex);
        return _buf.size() > Buffer::size / 2;
    }
    bool isBelowHalf() const {
        std::lock_guard<std::mutex> lock(mutex);
        return _buf.size() < Buffer::size / 2;
    }

protected:
    mutable std::mutex mutex;
    std::condition_variable cv;

private:
    std::deque<T> _buf;
};

/**
 * @brief A Chunk is a buffer of characters.
 */
class Chunk : public Buffer<char, 1024> {};

/**
 * @brief A Pool is a buffer of Chunks.
 *
 * The Fit() method splits an input string into chunks and pushes them into the pool.
 */
class Pool : public Buffer<Chunk, 1024> {
public:
    /**
     * @brief Splits the input string into fixed-size chunks and pushes them into the pool.
     *
     * @param buf The input string to split.
     */
    void Fit(const std::string& buf) {
        size_t nchunks = buf.size() / Chunk::size;
        if (buf.size() % Chunk::size != 0)
            ++nchunks;

        for (size_t nchunk = 0; nchunk < nchunks; ++nchunk) {
            Chunk chunk;
            size_t start_idx = nchunk * Chunk::size;
            size_t finish_idx = std::min(start_idx + Chunk::size, buf.size());
            for (size_t idx = start_idx; idx < finish_idx; ++idx) {
                chunk.Push(buf[idx]);
            }
            Push(chunk);
        }
    }
};
