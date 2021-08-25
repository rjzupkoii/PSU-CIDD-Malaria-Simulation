/*
 * ParallelJobs.h
 *
 * Define a parallel job processor using the singleton pattern.
 */
#ifndef PARALLELJOBS_H
#define PARALLELJOBS_H

#include <functional>
#include <mutex>
#include <thread>
#include <queue>

class ParallelJobs {
  public:
    struct task {
      std::function<void()> function;
    };

  private:
    // Queue for the jobs
    mutable std::mutex jobs_mutex;
    std::queue<task> jobs;

    // Worker threads
    std::vector<std::thread> workers;

    // Flag to stop the worker thread
    bool stopFlag = false;

    // Constructor
    ParallelJobs();

    // Deconstructor
    ~ParallelJobs();

    // Check the jobs and run them if one exists
    static void do_work();

    bool get_job(task& job);

  public:
    // Not supported by singleton
    ParallelJobs(ParallelJobs const&) = delete;

    // Not supported by singleton
    void operator=(ParallelJobs const&) = delete;

    // Get a reference to the object
    static ParallelJobs& get_instance() {
        static ParallelJobs instance;
        return instance;
    }

    // Starts the parallel jobs thread and allows work to be queued, returns
    // the number of threads that was started
    unsigned long start(unsigned int count);

    // Add a job to the work queue, it will be run in FIFO order
    unsigned long add_job(const task& job);

    // Check to see if there is work pending in the job queue
    bool work_pending();

    // Gracefully stops the parallel job queue
    void stop();
};

#endif