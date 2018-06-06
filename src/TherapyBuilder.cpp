/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TherapyBuilder.cpp
 * Author: Merlin
 * 
 * Created on August 23, 2017, 2:43 PM
 */

#include "TherapyBuilder.h"
#include "Therapy.h"
#include "SCTherapy.h"
#include "MACTherapy.h"

TherapyBuilder::TherapyBuilder() {
}

TherapyBuilder::~TherapyBuilder() {
}

Therapy* TherapyBuilder::build(const YAML::Node& ns, const int& t_id) {
    Therapy* t = NULL;
    if (ns["drug_id"]) {
        t = new SCTherapy();
        t->set_id(t_id);
        for (int i = 0; i < ns["drug_id"].size(); i++) {
            int drug_id = ns["drug_id"][i].as<int>();
            //        std::cout << therapy_id << "-" << drug_id << std::endl;
            ((SCTherapy*) t)->add_drug(drug_id);
        }

        int dosing_days = ns["dosing_days"].as<int>();
        ((SCTherapy*) t)->set_dosing_day(dosing_days);
    } else {
        if (ns["therapy_ids"]) {
            t = new MACTherapy();
            t->set_id(t_id);
            
            for (int i = 0; i < ns["therapy_ids"].size(); i++) {
                int therapy_id = ns["therapy_ids"][i].as<int>();
                //        std::cout << therapy_id << "-" << drug_id << std::endl;
                ((MACTherapy*) t)->add_therapy_id(therapy_id);
            }
            for (int i = 0; i < ns["regimen"].size(); i++) {
                int starting_day = ns["regimen"][i].as<int>();
                //        std::cout << therapy_id << "-" << drug_id << std::endl;
                ((MACTherapy*) t)->add_schedule(starting_day);
            }
        }
    }


    return t;
}
