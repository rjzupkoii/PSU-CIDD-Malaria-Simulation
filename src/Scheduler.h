/* 
 * File:   Scheduler.h
 * Author: nguyentran
 *
 * Created on March 22, 2013, 2:27 PM
 */

#ifndef SCHEDULER_H
#define	SCHEDULER_H
#include "PropertyMacro.h"
#include "TypeDef.h"

class Model;

class Scheduler {
    DISALLOW_COPY_AND_ASSIGN_(Scheduler)
    PROPERTY_REF(int, current_time)
    PROPERTY_HEADER(int, total_time)

    POINTER_PROPERTY(Model, model);

    PROPERTY_REF(EventPtrVector2, timed_events_list);
    PROPERTY_REF(bool, is_force_stop);

public:
    Scheduler(Model* model = nullptr);
    virtual ~Scheduler();

    void clear_all_events();

    virtual void schedule(Event* event);
    virtual void cancel(Event* event);

    void initialize(const int& total_time);

    void run();

    void begin_time_step();

    void end_time_step();

    bool can_stop();

    void update_end_of_time_step();

    void update_force_of_infection();

    void report_end_of_time_step();

    int current_day_in_year();
    
    void perform_monthly_update();

private:

    void update_treatment_coverage();
};

#endif	/* SCHEDULER_H */

