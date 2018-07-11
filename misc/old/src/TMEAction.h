/* 
 * File:   TMEAction.h
 * Author: Merlin
 *
 * Created on August 23, 2013, 1:38 PM
 */

#ifndef TMEACTION_H
#define	TMEACTION_H

class TMEAction {
public:
    TMEAction();
    TMEAction(const TMEAction& orig);
    virtual ~TMEAction();

    virtual void perform_weekly_action(const int& location) = 0;

private:

};

typedef TMEAction* (*CreateActionFn)(void);

#endif	/* TMEACTION_H */

