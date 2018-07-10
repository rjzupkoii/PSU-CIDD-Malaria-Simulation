/* 
 * File:   PersonIndexHandler.h
 * Author: nguyentran
 *
 * Created on April 17, 2013, 10:29 AM
 */

#ifndef PERSONINDEXHANDLER_H
#define	PERSONINDEXHANDLER_H
#include "Core/PropertyMacro.h"
class IndexHandler {
    DISALLOW_COPY_AND_ASSIGN(IndexHandler)
    PROPERTY_REF(int,index)
    
public:
    IndexHandler();    
    virtual ~IndexHandler();
private:

};

#endif	/* PERSONINDEXHANDLER_H */

