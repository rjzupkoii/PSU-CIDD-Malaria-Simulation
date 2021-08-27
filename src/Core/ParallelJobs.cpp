/*
 * ParallelJobs.cpp
 *
 * Implementation of the ParallelJob functions.
 */
#include "ParallelJobs.h"

#include "easylogging++.h"

ParallelJobs::ParallelJobs() = default;

ParallelJobs::~ParallelJobs() = default;

unsigned long ParallelJobs::start(unsigned int count) {

  // Verify that the count makes sense
  if (count == 0) {
    throw std::invalid_argument("Count of threads must be greater than zero.");
  }
  if (count > std::thread::hardware_concurrency()) {
    throw std::invalid_argument("Count of threads should not exceed hardware limits.");
  }

  // Start the threads and return the count as a validation check
  for (auto ndx = 0; ndx < count; ndx++) {
    workers.emplace_back(do_work);
  }
  return workers.size();
}

bool ParallelJobs::work_pending() {
  std::scoped_lock lock(jobs_mutex);
  return !jobs.empty();
}

void ParallelJobs::stop() {
  // Stop the workers
  stopFlag = true;
  for (auto& worker : workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }

  // Delete the workers
  workers.clear();
}

void ParallelJobs::do_work() {
  std::function<void()> job;

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
    if (instance.get_job(job)) {
      job();
    }
  }
}

bool ParallelJobs::get_job(std::function<void()> &job) {
  // Hold the mutex for this function
  std::scoped_lock lock(jobs_mutex);

  // Return if there is nothing to
  if (jobs.empty()) {
    return false;
  }

  // Move the first job to the memory location and return
  job = std::move(jobs.front());
  jobs.pop();
  return true;
}