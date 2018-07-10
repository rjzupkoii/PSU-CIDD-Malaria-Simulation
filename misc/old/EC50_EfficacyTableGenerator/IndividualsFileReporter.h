/* 
 * File:   IndividualsFileReporter.h
 * Author: Merlin
 *
 * Created on November 7, 2014, 11:06 AM
 */

#ifndef INDIVIDUALSFILEREPORTER_H
#define	INDIVIDUALSFILEREPORTER_H
#include "Core/PropertyMacro.h"
#include "Reporter.h"
#include <fstream>
#include <string>
class IndividualsFileReporter : public Reporter {
    DISALLOW_COPY_AND_ASSIGN_(IndividualsFileReporter)

    std::fstream fs_;
    std::string file_name_;
    
public:
    IndividualsFileReporter(const std::string& file_name);
    //    IndividualsFileReporter(const IndividualsFileReporter& orig);
    virtual ~IndividualsFileReporter();
private:
    virtual void initialize();
    virtual void before_run();
    virtual void after_run();
    virtual void begin_time_step();
    virtual void after_time_step();
};

#endif	/* INDIVIDUALSFILEREPORTER_H */

