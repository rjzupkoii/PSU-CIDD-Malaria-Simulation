/* 
 * File:   STCAction.h
 * Author: Merlin
 *
 * Created on August 24, 2013, 3:40 PM
 */

#ifndef STCACTION_H
#define	STCACTION_H

#include "TMEAction.h"

class STCAction : public TMEAction {
public:
    STCAction();
    STCAction(const STCAction& orig);
    virtual ~STCAction();

    virtual void perform_weekly_action(const int& location);

    static TMEAction* Create() {
        return new STCAction();
    }
private:

};

#endif	/* STCACTION_H */

