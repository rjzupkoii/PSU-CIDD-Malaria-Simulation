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

first_line = {
    'DHA-PPQ' : 'input_mmc_S_A4_mu_0p001983.yml',
    'ASAQ': 'input_mmc_A5b_mu_0p001983.yml',
    'AL': 'input_mmc_A6b_mu_0p001983.yml'
    };

mutant_introductions = {
    0.0: '0p00',
    0.01: '0p01',
    0.1: '0p10',
    0.25: '0p25',
    0.5: '0p50'  
};
            
treatment_coverages = {
        0.4: {
            'f': '0p4',
            'beta_pfpr': {
                    0.16: 'PFPR10',               
                    }
        },
};    

compliance = {
        1.0: '1p0',        
        };

ec50_factors= {
    '0p85': 0.85,
    '0p90': 0.9,
    '0p95': 0.95,
    '1p00': 1.0,
    '1p05': 1.05,
    '1p10': 1.1,
    '1p15': 1.15
    }

#%%
for fl, input_fn in first_line.items():            
    print(fl, input_fn);
    
    stream = open(input_fn, 'r');
    
    
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
    
    for ec50_factor_str, ec50_factor in ec50_factors.items():
        for mu, mu_str in mutant_introductions.items():    
            for comp, comp_str in compliance.items():        
                for tc, tc_map in treatment_coverages.items():            
                    for beta, pfpr in tc_map['beta_pfpr'].items():
                        new_data = copy.deepcopy(data)
       
                        for index,event in enumerate(data['events']):
                            if event['name'] == 'introduce_lumefantrine_mutant_parasites' or event['name'] == 'introduce_aq_mutant_parasites' or event['name'] == 'introduce_plas2_parasites':
                                new_data['events'][index]['info'][0]['fraction'] = mu
                        
                        # loop all drugs and adjust EC50
                        for drug_index in data['drug_db']:
                            drug = data['drug_db'][drug_index]
                            for ec50_index in drug['EC50']:
                                original_ec50 = drug['EC50'][ec50_index]
                                new_ec50 = original_ec50 * ec50_factor
                                new_data['drug_db'][drug_index]['EC50'][ec50_index] = new_ec50
                                # print(drug_index, ec50_index, original_ec50, new_ec50)
                        
                        new_data['p_compliance'] = comp                   
                        
                        new_data['location_db']['p_treatment_for_less_than_5_by_location'][0] = tc
                        new_data['location_db']['p_treatment_for_more_than_5_by_location'][0] = tc
                        
                        new_data['location_db']['beta_by_location'][0] = beta
                        
                        ## save to file
                        output_filename = 'SA2/%s_ec50_factor_%s_mu_0p001983_introduce_mutant_%s_comp_%s_tc_%s_pfpr_%s.yml'%( fl, ec50_factor_str ,mu_str, comp_str, tc_map['f'], pfpr);
                        output_stream = open(output_filename, 'w');
                        yaml.dump(new_data, output_stream); 
                        output_stream.close();
