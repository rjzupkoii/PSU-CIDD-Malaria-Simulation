/* 
 * File:   MDAAction.h
 * Author: Merlin
 *
 * Created on August 24, 2013, 3:39 PM
 */

#ifndef MDAACTION_H
#define	MDAACTION_H

#include "TMEAction.h"

class MDAAction : public TMEAction {
public:
    MDAAction();
    MDAAction(const MDAAction& orig);
    virtual ~MDAAction();

    virtual void perform_weekly_action(const int& location);

    static TMEAction* Create() {
        return new MDAAction();
    }
private:

};

#endif	/* MDAACTION_H */

