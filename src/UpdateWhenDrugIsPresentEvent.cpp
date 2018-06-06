/* 
 * File:   UpdateByHavingDrugEvent.cpp
 * Author: Merlin
 * 
 * Created on July 31, 2013, 12:16 PM
 */

#include "UpdateWhenDrugIsPresentEvent.h"

#include "Person.h"
#include "ClonalParasitePopulation.h"
#include "Scheduler.h"
#include "DrugsInBlood.h"
#include "SingleHostClonalParasitePopulations.h"
#include "Config.h"
#include "Model.h"

OBJECTPOOL_IMPL(UpdateWhenDrugIsPresentEvent)

UpdateWhenDrugIsPresentEvent::UpdateWhenDrugIsPresentEvent() {
}

UpdateWhenDrugIsPresentEvent::~UpdateWhenDrugIsPresentEvent() {
}

void UpdateWhenDrugIsPresentEvent::schedule_event(Scheduler* scheduler, Person* p, ClonalParasitePopulation* clinical_caused_parasite, const int& time) {
    if (scheduler != nullptr) {

        UpdateWhenDrugIsPresentEvent* e = new UpdateWhenDrugIsPresentEvent();
        e->set_dispatcher(p);
        e->set_clinical_caused_parasite(clinical_caused_parasite);
        e->set_executable(true);
        e->set_time(time);

        p->add(e);
        scheduler->schedule(e);
    }
}

void UpdateWhenDrugIsPresentEvent::execute() {
    Person* person = (Person*) dispatcher();
    if (person->drugs_in_blood()->size() > 0) {
        if (person->all_clonal_parasite_populations()->contain(clinical_caused_parasite_) && person->host_state() == Person::CLINICAL) {
            if (clinical_caused_parasite_->last_update_log10_parasite_density() <= Model::CONFIG->log_parasite_density_level().log_parasite_density_asymptomatic) {
                person->set_host_state(Person::ASYMPTOMATIC);
            }
        }
        person->schedule_update_by_drug_event(clinical_caused_parasite_);
    } else {
        //        no drug in blood, reset the status of drug Update parasite
        // the drug update parasite is inserted into blood when  there is still drug in blood and also the clinical cause parasite

        for (int i = 0; i < person->all_clonal_parasite_populations()->size(); i++) {
            ClonalParasitePopulation* bloodParasite = person->all_clonal_parasite_populations()->parasites()->at(i);
            if (bloodParasite->update_function() == Model::MODEL->having_drug_update_function()) {
                person->determine_relapse_or_not(bloodParasite);

            }

        }


        //        map<int, BloodParasite*>::iterator pos;
        //        for (pos = person->parasitePopulation->parasitePop.begin(); pos != person->parasitePopulation->parasitePop.end(); ++pos) {
        //            BloodParasite* bloodParasite = pos->second;
        //            int pID = pos->first;
        //
        //            if (bloodParasite->updateFunction == BloodParasite::drugUpdateFunction) {
        //
        //                double PClinical = person->getClinicalProbability();
        //                double P = Random::randFlat(0.0, 1.0);
        //                if (!Global::AllowNewCoinfectionToCauseSymptoms && person->parasitePopulation->size > 1) {
        //                    PClinical = -1;
        //                }
        //
        //                if (P < PClinical) {
        //                    //progress to clinical after several days
        //                    person->parasitePopulation->parasitePop[pID]->updateFunction = BloodParasite::clinicalUpdateFunction;
        //                    person->scheduleProgressToClinicalEvent(pID);
        //
        //                } else {
        //                    //only progress to clearance by Immune system
        //                    //progress to clearance
        //                    person->parasitePopulation->parasitePop[pID]->updateFunction = BloodParasite::clearanceUpdateFunction;
        //
        //                    //        person->scheduleClearParasiteByImmneSystemEvent(pID);
        //                    //        if (person->drugsInBlood->getSize() == 0) {
        //                    //            person->cancelAllEventsExceptID(Events::UpdateEveryKDays, id);
        //                    //            person->scheduleUpdateEveryKDays();
        //                    //        }
        //                }
        //
        //            }
        //        }

    }
}

