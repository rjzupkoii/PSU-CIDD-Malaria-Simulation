/*
 * ParallelJobs.cpp
 *
 * Implementation of the ParallelJob functions.
 */
#include "ParallelJobs.h"

#include "easylogging++.h"

ParallelJobs::ParallelJobs() { }

ParallelJobs::~ParallelJobs() { }

int ParallelJobs::start() {
  worker = std::thread(do_work);
  return 1;
}

int ParallelJobs::add_job(task job) {
  auto size = jobs.size();
  jobs.push(job);
  return (size + 1);
}

void ParallelJobs::stop() {
  stopFlag = true;
  if (worker.joinable()) {
      worker.join();
  }
}

void ParallelJobs::do_work() {
  while(true) {
    auto& instance = get_instance();

    // If we are told to stop, then stop
    if (instance.stopFlag) {
      LOG(INFO) << "Stopping worker thread";
      if (instance.jobs.size() != 0) {
        LOG(INFO) << instance.jobs.size() << " job(s) remain in worker thread";
      }
      break;
    }

    // Otherwise check for work
    if (!instance.jobs.empty()) {
      // Get the task from the queue
      task job = instance.jobs.front();
      instance.jobs.pop();

      // Run the function
      job.function();
    }
  }
}
