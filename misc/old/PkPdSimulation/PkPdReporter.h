/* 
 * File:   PkPdReporter.h
 * Author: Merlin
 *
 * Created on October 29, 2014, 12:56 PM
 */

#ifndef PKPDREPORTER_H
#define	PKPDREPORTER_H
#include "Reporter.h"
#include "Core/TypeDef.h"


class PkPdReporter : public Reporter {
    DISALLOW_COPY_AND_ASSIGN_(PkPdReporter)
    PROPERTY_REF(DoubleVector, yesterday_density)
public:
    PkPdReporter();
//    PkPdReporter(const PkPdReporter& orig);
    virtual ~PkPdReporter();
    
    
    
    virtual void initialize();
    virtual void before_run();
    virtual void after_run();
    virtual void begin_time_step();
    virtual void after_time_step();
    
private:

};

#endif	/* PKPDREPORTER_H */

