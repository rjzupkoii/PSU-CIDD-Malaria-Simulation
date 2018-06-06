/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   BurninMonthlyReporter.h
 * Author: Merlin
 *
 * Created on April 11, 2017, 12:05 PM
 */

#ifndef BURNINMONTHLYREPORTER_H
#define BURNINMONTHLYREPORTER_H
#include "Reporter.h"
#include <string>

class BurninMonthlyReporter : public Reporter {
    DISALLOW_COPY_AND_ASSIGN_(BurninMonthlyReporter)

    VIRTUAL_PROPERTY_REF(double, last_reported_NTF)
    VIRTUAL_PROPERTY_REF(int, last_reported_clinical_episodes)
    VIRTUAL_PROPERTY_REF(int, last_reported_mutants)


public:
    BurninMonthlyReporter();
    //    MonthlyReporter(const MonthlyReporter& orig);
    virtual ~BurninMonthlyReporter();

    virtual void initialize();
    virtual void before_run();
    virtual void after_run();
    virtual void begin_time_step();
    virtual void after_time_step();

private:

};


#endif /* BURNINMONTHLYREPORTER_H */

