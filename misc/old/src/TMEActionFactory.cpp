/* 
 * File:   TMEActionFactory.cpp
 * Author: Merlin
 * 
 * Created on August 24, 2013, 3:10 PM
 */

#include "TMEActionFactory.h"
#include "MDAAction.h"
#include "STCAction.h"

TMEActionFactory::TMEActionFactory() {
    register_action("A", &MDAAction::Create);
    register_action("B", &STCAction::Create);
}

TMEActionFactory::~TMEActionFactory() {
}

void TMEActionFactory::register_action(const std::string& action_name, CreateActionFn pfnCreate) {
    m_FactoryMap[action_name] = pfnCreate;
}

TMEAction* TMEActionFactory::create_action(const std::string& action_name) {
    FactoryMap::iterator it = m_FactoryMap.find(action_name);
    if (it != m_FactoryMap.end())
        return it->second();
    return nullptr;
}