/* 
 * File:   TMEActionFactory.h
 * Author: Merlin
 *
 * Created on August 24, 2013, 3:10 PM
 */

#ifndef TMEACTIONFACTORY_H
#define	TMEACTIONFACTORY_H
#include "TMEAction.h"
#include <map>
#include <string>

class TMEActionFactory {
public:
    virtual ~TMEActionFactory();

    static TMEActionFactory *Get() {
        static TMEActionFactory instance;
        return &instance;
    }

    void register_action(const std::string &action_name, CreateActionFn pfnCreate);
    TMEAction* create_action(const std::string& action_name);

private:
    TMEActionFactory();
    TMEActionFactory(const TMEActionFactory& orig);
    
    typedef std::map<std::string, CreateActionFn> FactoryMap;
    FactoryMap m_FactoryMap;
};

#endif	/* TMEACTIONFACTORY_H */

