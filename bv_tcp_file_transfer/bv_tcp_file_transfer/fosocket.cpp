#include "fsocket.h"
#include "worker.h"
#include "file.h"
#include "log.h"

/**
 * @brief Worker that reads a file and fills a Pool with its content.
 */
class FileReaderWorker : public Worker {
public:
    /**
     * @brief Constructs a FileReaderWorker.
     *
     * @param location Path to the input file.
     * @param pool Reference to the Pool to fill.
     */
    explicit FileReaderWorker(const std::string& location, Pool& pool)
        : _location(location), _pool(pool), _is_finished(false)
    {}

    void Work() override {
        // Create a terminator chunk with a '\0' character.
        std::string buf;
        _fr.Read(buf);
        _pool.Fit(buf);
    }

    /**
     * @brief Checks whether the worker has finished processing.
     *
     * @return true if finished, false otherwise.
     */
    bool isFinished() {
        return _is_finished.load();
    }

protected:
    void onPrepareWork() override {
        _is_finished.store(false);
        _fr.Open(_location);
    }

    void onFinishWork() override {
        _fr.Close();
        _is_finished.store(true);
    }

private:
    const std::string _location;
    Pool& _pool;
    FileReader _fr;
    std::atomic<bool> _is_finished;
};

status FOSocket::Connect(const std::string& dst_addr, const uint16_t dst_port) {
    return _client.Connect(dst_addr, dst_port);
}

void FOSocket::Transmit(const std::string& location) {
    FileReaderWorker frw(location, pool());
    std::thread frwt(std::ref(frw));
    size_t chunk_cnt = 0;

    tcpft_logInfo("transmit ", "\"", location, "\" starting...");

    while (!(frw.isFinished() && pool().isEmpty())) {
        pool().waitForNotEmpty();
        Chunk chunk = pool().Pop();

        std::string buf;
        while (!chunk.isEmpty()) {
            buf.push_back(chunk.Pop());
        }

        ++chunk_cnt;
        tcpft_logInfo("send chunk: ", chunk_cnt, ", size: ", buf.size());
        _client.Send(&buf[0], buf.size(), 0);
    }

    tcpft_logInfo("transmit finished");
    frwt.join();
    _client.Close();
}

int FOSocket::Close() {
    return _client.Close();
}