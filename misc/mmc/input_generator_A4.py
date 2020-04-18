# -*- coding: utf-8 -*-
"""
Created on Tue Aug  7 14:23:33 2018

@author: NguyenTran
"""

import yaml;
import numpy as np;
from math import log;
import copy;

#import inflect;
#p = inflect.engine();

def kFormatter(num):
    return str(num) if num <=999 else str(round(num/1000)) +'k';


stream = open('input_mmc_A4_mu_0p001983.yml', 'r');
data = yaml.load(stream);
stream.close();

data['starting_date'] = '2006/1/1';
data['ending_date'] = '2060/1/1';
data['start_of_comparison_period']= '2020/1/1';

data['seasonal_info']['enable'] = 'false';

#1 location
location_info =  [[0, 0, 0]];
number_of_locations = len(location_info);
data['location_db']['location_info']= location_info;


#population size 
popsize = 70000
data['location_db']['population_size_by_location'] = [popsize];       

plas2_introductions = {
        0.0: '0p00',
        0.01 : '0p01',
        0.05 : '0p05',
        0.1: '0p10',
        0.15: '0p15',
        0.25: '0p25',
        0.5: '0p50'       
        };
        
treatment_coverages = {
        0.2: {
            'f': '0p2',
            'beta_pfpr': {
                    0.05: 'PFPR01',
                    0.08: 'PFPR05',
                    0.14: 'PFPR10',
                    0.32: 'PFPR20',                
                    }
        },
        0.4: {
            'f': '0p4',
            'beta_pfpr': {
                    0.06: 'PFPR01',
                    0.093: 'PFPR05',
                    0.18: 'PFPR10',
                    0.38: 'PFPR20',                
                    }
        },
        0.6: {
            'f': '0p6',
            'beta_pfpr': {
                    0.09: 'PFPR01',
                    0.11: 'PFPR05',
                    0.19: 'PFPR10',
                    0.45: 'PFPR20',                
                    }
        }
};    

compliance = {
        1.0: '1p0'
        };
              
#%%
for plas2, plas2_str in plas2_introductions.items():    
    for comp, comp_str in compliance.items():        
        for tc, tc_map in treatment_coverages.items():            
            for beta, pfpr in tc_map['beta_pfpr'].items():
                new_data = copy.deepcopy(data)
                
#                print(plas2, plas2_str, comp, comp_str, tc, tc_map['f'], beta, pfpr)
                
                ##modify parameters
                for index,event in enumerate(data['events']):
                    if event['name'] == 'introduce_plas2_parasites':
                        new_data['events'][index]['info'][0]['fraction'] = plas2
                
                new_data['p_compliance'] = comp                   
                
                new_data['location_db']['p_treatment_for_less_than_5_by_location'][0] = tc
                new_data['location_db']['p_treatment_for_more_than_5_by_location'][0] = tc
                
                new_data['location_db']['beta_by_location'][0] = beta
                
                ## save to file
                output_filename = 'A4/input_mmc_A4_mu_0p001983_plas2_%s_comp_%s_tc_%s_pfpr_%s.yml'%( plas2_str, comp_str, tc_map['f'], pfpr);
                output_stream = open(output_filename, 'w');
                yaml.dump(new_data, output_stream); 
                output_stream.close();
                