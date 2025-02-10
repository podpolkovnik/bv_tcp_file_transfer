#pragma once

/**
 * @brief Abstract worker class.
 *
 * Derived classes must implement the Work() method.
 */
class Worker {
public:
    Worker() = default;
    Worker(const Worker&) = default;
    Worker(Worker&&) = default;
    virtual ~Worker() = default;

    /**
     * @brief Performs the work. Must be overridden.
     */
    virtual void Work() = 0;

    /**
     * @brief Functor operator that calls preparation, work and finish methods.
     */
    void operator()() {
        onPrepareWork();
        Work();
        onFinishWork();
    }

protected:
    /**
     * @brief Called before the main work.
     */
    virtual void onPrepareWork() {}
    /**
     * @brief Called after the main work.
     */
    virtual void onFinishWork() {}
};
