//
// Created by Nguyen Tran on 3/5/2018.
//

#ifndef POMS_BFFARMREPORTER_H
#define POMS_BFFARMREPORTER_H

#include "Reporter.h"

class BFFarmReporter: public Reporter {
    DISALLOW_COPY_AND_ASSIGN_(BFFarmReporter)

public:
    BFFarmReporter();
    virtual ~BFFarmReporter();

    void initialize() override;

    void before_run() override;

    void after_run() override;

    void begin_time_step() override;

    void after_time_step() override;

    void outout_parameters();
};


#endif //POMS_BFFARMREPORTER_H
