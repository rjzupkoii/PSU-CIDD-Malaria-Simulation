/*
 * ParallelJobs.h
 *
 * Define a parallel job processor using the singleton pattern.
 */
#ifndef PARALLELJOBS_H
#define PARALLELJOBS_H

#include <functional>
#include <future>
#include <mutex>
#include <thread>
#include <queue>

class ParallelJobs {
  private:
    // Queue for the jobs
    mutable std::mutex jobs_mutex;
    std::queue<std::function<void()>> jobs;

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

    // Get the next job from the work queue
    bool get_job(std::function<void()>& job);

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
    template<typename function, typename... Args>
    void submit(function f, Args&&... args) {

      // Wrap the function and arguments we were given as packaged task, this allows us to insert a generic function
      // into the work queue that just calls the function with the parameters we were given
      using wrappedFunction = std::packaged_task<std::invoke_result_t<function, Args...>(Args...)>;
      std::shared_ptr<wrappedFunction> functionPointer = std::make_shared<wrappedFunction>(f);

      // Scope the operation to place the job in the work queue
      {
        std::scoped_lock lock(jobs_mutex);
        jobs.emplace([functionPointer, &args...]() -> void {
          (*functionPointer)(std::forward<Args>(args)...);
          return;
        });
      }
    }

    // Check to see if there is work pending in the job queue
    bool work_pending();

    // Gracefully stops the parallel job queue
    void stop();
};

#endif