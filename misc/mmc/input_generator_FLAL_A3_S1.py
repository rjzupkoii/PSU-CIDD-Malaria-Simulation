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


stream = open('input_mmc_s1.v8.yml', 'r');
data = yaml.load(stream);
stream.close();

data['starting_date'] = '2006/1/1';
data['ending_date'] = '2040/1/1';
data['start_of_comparison_period']= '2020/1/1';

data['seasonal_info']['enable'] = 'false';

#1 location
location_info =  [[0, 0, 0]];
number_of_locations = len(location_info);
data['location_db']['location_info']= location_info;


#population size 
popsize = 100000
data['location_db']['population_size_by_location'] = [popsize];       

mutation_rates = [0.0016, 0.0019, 0.0022];

mutation_rates_str = ['0p0016','0p0019','0p0022'];

prob_symptoms={ 0.4: '0p4', 
               0.6:'0p6', 
               0.8: '0p8'
               }; 

plas2_introductions = {
        0.05 : '0p05',
        0.1: '0p10',
        0.2: '0p20'        
        };
              
              
#%%
for i,mutation_rate in enumerate(mutation_rates):
    new_data = copy.deepcopy(data)
    for drug_id,drug in enumerate(data['drug_db']):
        print(mutation_rate)
        new_data['drug_db'][drug_id]['mutation_probability'] = mutation_rate
        output_filename = 'input_mmc_s2_mu_%s.yml'%(mutation_rates_str[i]);
        output_stream = open(output_filename, 'w');
        yaml.dump(new_data, output_stream); 
        output_stream.close();
        
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