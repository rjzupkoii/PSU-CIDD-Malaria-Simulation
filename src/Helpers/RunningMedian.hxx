/**
 * RunningMedian.hxx
 *
 * This template implements a running median using two priority queues. When calculating the median for a large block of
 * values (e.g., across the population of individuals) this should offer the fastest approach.
 *
 * Code adopted from: https://stackoverflow.com/a/28846596/1185
 */
#ifndef RUNNINGMEDIAN_HXX
#define RUNNINGMEDIAN_HXX

#include <limits>
#include <queue>
#include <vector>

template<class T>
class RunningMedian {
private:
    //Values greater than the median sorted so that smallest is on top
    std::priority_queue<T, std::vector<T>, std::greater<T> > upper;

    //Values smaller than the median sorted so that greatest is on top
    std::priority_queue<T, std::vector<T>, std::less<T> > lower;

public:
    RunningMedian() {
      //Seed the queues
      upper.push(std::numeric_limits<T>::max());
      lower.push (std::numeric_limits<T>::min());
    }

    // Add a number to the property queue
    void push(T value){
      // If number is greater than the smallest upper value, then it goes with the upper numbers;
      // otherwise it goes with the lower numbers
      if(value >= upper.top()) {
        upper.push(value);
      }
      else {
        lower.push(value);
      }

      // We need to rebalance if the queues sizes differ by 2, so that they differ by 1.
      if(upper.size() - lower.size() == 2) {
        lower.push(upper.top());
        upper.pop();
      } else if(lower.size() - upper.size() == 2){
        upper.push(lower.top());
        lower.pop();
      }
    }

    T getMedian() const {
      // If the queues are the same size, the median is the average of the smallest upper value, and largest lower value
      if(upper.size() == lower.size()) {
        return (upper.top() + lower.top()) / ((T) 2.0);
      }

      // Upper size is greater, so return it
      if(upper.size() > lower.size()) {
        return upper.top();
      }

      // The lower size must be greater, return it
      return lower.top();
    }
};

#endif