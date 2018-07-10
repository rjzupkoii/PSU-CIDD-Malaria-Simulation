/* 
 * File:   TMEScheduler.h
 * Author: Merlin
 *
 * Created on August 23, 2013, 1:38 PM
 */

#ifndef TMESCHEDULER_H
#define	TMESCHEDULER_H

#include <string>

#include "Core/PropertyMacro.h"
#include "Core/TypeDef.h"

class Model;

class TMEScheduler {
    DISALLOW_COPY_AND_ASSIGN(TMEScheduler)

    POINTER_PROPERTY(Model, model)
    PROPERTY_REF(StringVector2, schedule)
    PROPERTY_REF(std::string, tme_file_name)

public:
    TMEScheduler(Model* model = nullptr);
    //    TMEScheduler(const TMEScheduler& orig);
    virtual ~TMEScheduler();

    virtual void initialize();

    void read_csv(const std::string& file_name);

    void print();
    
    void check_and_perform_TME_Actions();

private:

};

#endif	/* TMESCHEDULER_H */

