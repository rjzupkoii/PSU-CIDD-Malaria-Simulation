/*
 * ParallelJobs.cpp
 *
 * Implementation of the ParallelJob functions.
 */
#include "ParallelJobs.h"

#include <cmath>

#include "easylogging++.h"

ParallelJobs::ParallelJobs() { }

ParallelJobs::~ParallelJobs() { }

unsigned long ParallelJobs::start() {

  // Start floor(n / 2) threads
  auto processors = std::thread::hardware_concurrency();
  for (auto ndx = 0; ndx < (int)floor(processors / 2.0); ndx++) {
    workers.emplace_back(do_work);
  }

  // Return the number of workers
  return workers.size();
}

unsigned long ParallelJobs::add_job(const task& job) {
  auto size = jobs.size();
  jobs.push(job);
  return (size + 1);
}

void ParallelJobs::stop() {
  stopFlag = true;
  for (auto& worker : workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

void ParallelJobs::do_work() {
  while(true) {
    auto& instance = get_instance();

    // If we are told to stop, then stop
    if (instance.stopFlag) {
      LOG(INFO) << "Stopping worker thread";
      if (!instance.jobs.empty()) {
        LOG(INFO) << instance.jobs.size() << " job(s) remain in worker thread";
      }
      break;
    }

    // Prepare the jobs variable since we run after unlocking
    task job;

    // Otherwise, check for work
    instance.jobs_lock.lock();
    if (!instance.jobs.empty()) {
      // Get the task from the queue
      job = instance.jobs.front();
      instance.jobs.pop();
    }
    instance.jobs_lock.unlock();

    // Run the function
    if (job.function != nullptr) {
      job.function();
    }
  }
}
