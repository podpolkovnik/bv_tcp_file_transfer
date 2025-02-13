#include "fsocket.h"
#include "worker.h"
#include "file.h"
#include "log.h"

/**
 * @brief Worker that writes file content from a Pool to an output file.
 */
class FileWriterWorker : public Worker {
public:
    /**
     * @brief Constructs a FileWriterWorker.
     *
     * @param location Path to the output file.
     * @param pool Reference to the Pool to read chunks from.
     */
    explicit FileWriterWorker(const std::string& location, Pool& pool)
        : _location(location), _pool(pool), _is_finished(false), _is_finish_requested(false)
    {}

    void Work() override {
        while (!(_is_finish_requested.load() && _pool.isEmpty())) {
            _pool.waitForNotEmpty();
            Chunk chunk = _pool.Pop();

            while (!chunk.isEmpty()) {
                _fw.Write(chunk.Pop());
            }
        }
    }

    /**
     * @brief Checks whether the worker has finished processing.
     *
     * @return true if finished, false otherwise.
     */
    bool isFinished() {
        return _is_finished.load();
    }

    /**
     * @brief Finish worker.
     *
     */
    void Finish() {
        _is_finish_requested.store(true);
    }

protected:
    void onPrepareWork() override {
        _is_finished.store(false);
        _fw.Open(_location);
    }

    void onFinishWork() override {
        _fw.Close();
        _is_finished.store(true);
    }

private:
    const std::string _location;
    Pool& _pool;
    FileWriter _fw;
    std::atomic<bool> _is_finished;
    std::atomic<bool> _is_finish_requested;
};

status FISocket::Init(const std::string& src_addr, const uint16_t src_port) {
    return _server.Init(src_addr, src_port);
}

void FISocket::Receive(const std::string& location) {
    FileWriterWorker fww(location, pool());
    std::thread fwwt(std::ref(fww));

    tcpft_sock sock = _server.Accept();
    std::string buf(Chunk::size, '\0');
    size_t chunk_cnt = 0;

    tcpft_logInfo("receive ", "\"", location, "\" starting...");

    while (!fww.isFinished()) {
        int nb = _server.Receive(sock, &buf[0], Chunk::size, 0);
        if (nb > 0) {
            if (nb < static_cast<int>(Chunk::size)) {
                buf.resize(nb);
            }
            ++chunk_cnt;
            tcpft_logInfo("receive chunk: ", chunk_cnt, ", size: ", nb);
            pool().Fit(buf);
        }
        else if (nb == 0) {
            fww.Finish();
            break;
        }
    }

    tcpft_logInfo("receive finished");
    fwwt.join();
}

int FISocket::Close() {
    return _server.Close();
}