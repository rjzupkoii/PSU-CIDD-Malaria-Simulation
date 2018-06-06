/* 
 * File:   DrugDatabase.h
 * Author: nguyentran
 *
 * Created on June 3, 2013, 11:05 AM
 */

#ifndef DRUGDATABASE_H
#define	DRUGDATABASE_H

#include "PropertyMacro.h"
#include "DrugType.h"
#include <map>

typedef std::map<int, DrugType*> DrugTypePtrMap;

class DrugDatabase {
    DISALLOW_COPY_AND_ASSIGN_(DrugDatabase)
    VIRTUAL_PROPERTY_REF(DrugTypePtrMap, drug_db)

public:
    DrugDatabase();
    //    DrugDatabase(const DrugDatabase& orig);
    virtual ~DrugDatabase();

    void add(DrugType* dt);
    DrugType* get(const int& id);
    
private:

};

#endif	/* DRUGDATABASE_H */

