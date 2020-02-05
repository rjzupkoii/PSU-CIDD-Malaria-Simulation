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


stream = open('input_mmc_A5_mu_0p001983.yml', 'r');
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

aq_mutant_introductions = {
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
                    0.09: 'PFPR05',
                    0.15: 'PFPR10',
                    0.4: 'PFPR20',                
                    }
        },
        0.4: {
            'f': '0p4',
            'beta_pfpr': {
                    0.1: 'PFPR05',
                    0.19: 'PFPR10',
                    0.46: 'PFPR20',                
                    }
        },
        0.6: {
            'f': '0p6',
            'beta_pfpr': {
                    0.12: 'PFPR05',
                    0.21: 'PFPR10',
                    0.49: 'PFPR20',                
                    }
        }
};    

compliance = {
        0.5: '0p5',
        0.9: '0p9',        
        };
              
#%%
for aq, aq_str in aq_mutant_introductions.items():    
    for comp, comp_str in compliance.items():        
        for tc, tc_map in treatment_coverages.items():            
            for beta, pfpr in tc_map['beta_pfpr'].items():
                new_data = copy.deepcopy(data)
                
#                print(plas2, plas2_str, comp, comp_str, tc, tc_map['f'], beta, pfpr)
                
                ##modify parameters
                for index,event in enumerate(data['events']):
                    if event['name'] == 'introduce_aq_mutant_parasites':
                        new_data['events'][index]['info'][0]['fraction'] = aq
                
                new_data['p_compliance'] = comp                   
                
                new_data['location_db']['p_treatment_for_less_than_5_by_location'][0] = tc
                new_data['location_db']['p_treatment_for_more_than_5_by_location'][0] = tc
                
                new_data['location_db']['beta_by_location'][0] = beta
                
                ## save to file
                output_filename = 'A5/input_mmc_A5_mu_0p001983_aq_mutant_%s_comp_%s_tc_%s_pfpr_%s.yml'%( aq_str, comp_str, tc_map['f'], pfpr);
                output_stream = open(output_filename, 'w');
                yaml.dump(new_data, output_stream); 
                output_stream.close();
                
                
#for i,mutation_rate in enumerate(mutation_rates):
#    new_data = copy.deepcopy(data)
#    for drug_id,drug in enumerate(data['drug_db']):
#        print(mutation_rate)
#        new_data['drug_db'][drug_id]['mutation_probability'] = mutation_rate
#        output_filename = 'input_mmc_s2_mu_%s.yml'%(mutation_rates_str[i]);
#        output_stream = open(output_filename, 'w');
#        yaml.dump(new_data, output_stream); 
#        output_stream.close();
        
#    for beta in betas:
#        for _,itc in improved_tc.items():                
#            new_data = copy.deepcopy(data)
#            new_data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
#            
#            for index,event in enumerate(data['events']):
#                if event['name'] == 'single_round_MDA':
#                    new_data['events'][index]['info'] = data['events'][index]['info'][0:mda_round]                    
#            pfpr_str = pfpr[beta]#            
#            if itc == '':
#                for index,event in enumerate(data['events']):
#                    if event['name'] == 'change_treatment_coverage':
#                        new_data['events'][index]['info']= []
#        
#            output_filename = 'FLAL/%s/ONELOC_%s_%dRMDA_%s_OPPUNIFORM_FLAL%s.yml'%(kFormatter(popsize), kFormatter(popsize),mda_round,pfpr_str,itc);
#            output_stream = open(output_filename, 'w');
#            yaml.dump(new_data, output_stream); 
#            output_stream.close();

#for index,beta in enumerate(betas):
#    data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
#    output_filename = 'beta/input_beta_%d.yml'%index;
#    output_stream = open(output_filename, 'w');
#    yaml.dump(data, output_stream);
#    output_stream.close();

#
#
#print(kFormatter(9000));
#print(p.number_to_words( number_of_locations, threshold=10));

#output_filename = 'ONELOC_300K_3RMDA_PFPR15_OPPUNIFORM_FLAL.yml';
#
#output_filename = 'input_test.yml';
#
#output_stream = open(output_filename, 'w');
#yaml.dump(data, output_stream);