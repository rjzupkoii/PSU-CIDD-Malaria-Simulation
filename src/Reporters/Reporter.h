/* 
 * File:   Reporter.h
 * Author: Merlin
 *
 * Created on August 1, 2013, 12:05 PM
 */

#ifndef REPORTER_H
#define REPORTER_H

#include "../PropertyMacro.h"

class Model;

class Reporter {

    DISALLOW_COPY_AND_ASSIGN_(Reporter)
    POINTER_PROPERTY(Model, model)

    enum ReportType {
        CONSOLE = 0,
        GUI,
        AMU,
        FARM,
        MULTIPLE_LOCATION,
        AGE_GROUP_2_TO_10,
        YEARLY_REPORTER_V1,
        MONTHLY_REPORTER,
        BURNIN_MONTHLY_REPORTER,
        BURNIN_FARM_REPORTER,
        BFREPORTER,
        BFFARM_REPORTER
    };


public:
    Reporter();
    //    Reporter(const Reporter& orig);
    virtual ~Reporter();

    virtual void initialize() = 0;
    virtual void before_run() = 0;
    virtual void after_run() = 0;
    virtual void begin_time_step() = 0;
    virtual void after_time_step() = 0;

    static Reporter* MakeReport(ReportType report_type);


private:

};

#endif /* REPORTER_H */

