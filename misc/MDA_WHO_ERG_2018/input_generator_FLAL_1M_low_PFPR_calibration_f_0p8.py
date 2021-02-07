# -*- coding: utf-8 -*-
"""
Created on Tue Aug  7 14:23:33 2018

@author: NguyenTran
"""

import yaml;
import numpy as np;
from math import log;
import copy;

import inflect;
p = inflect.engine();

def kFormatter(num):
    return str(num) if num <=999 else str(round(num/1000)) +'k';


stream = open('input_FLAL_small_pop_size.yml', 'r');
data = yaml.load(stream);
stream.close();

data['starting_date'] = '2006/1/1';
data['ending_date'] = '2020/1/1';
data['start_of_comparison_period']= '2020/1/1';

data['seasonal_info']['enable'] = 'false';

#1 location
location_info =  [[0, 0, 0]];
number_of_locations = len(location_info);
data['location_db']['location_info']= location_info;


#population size 
popsize = 1000000
data['location_db']['population_size_by_location'] = [popsize];       

#3RMDA
number_MDA_round = [0];

#
sd_prob_individual_present_at_mda = [0.3, 0.3, 0.3]
data['sd_prob_individual_present_at_mda'] = sd_prob_individual_present_at_mda

# p_treatment_for_less_than_5_by_location: [0.600000]
#   p_treatment_for_more_than_5_by_location: [0.500000]

data['location_db']['p_treatment_for_less_than_5_by_location'] = [0.8]
data['location_db']['p_treatment_for_more_than_5_by_location'] = [0.8]

#for index,event in enumerate(data['events']):
#    if event['name'] == 'single_round_MDA':
#        data['events'][index]['info'] = data['events'][index]['info'][0:number_MDA_round]


# betas = [0.048, 0.0538]

beta_from = 0.0555
beta_to = 0.065

betas = np.round(np.arange(beta_from, beta_to, (beta_to-beta_from)/100),5)
print(betas)

for mda_round in number_MDA_round:
    for beta in betas:
        new_data = copy.deepcopy(data)
        new_data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
        
        for index,event in enumerate(data['events']):
            if event['name'] == 'single_round_MDA':
                new_data['events'][index]['info'] = data['events'][index]['info'][0:mda_round]                    
        
        
        
        for index,event in enumerate(data['events']):
            if event['name'] == 'change_treatment_coverage':
                new_data['events'][index]['info']= []
        
        for index,event in enumerate(data['events']):
            if event['name'] == 'introduce_parasites_periodically':
                new_data['events'][index]['info']= []     
    
        output_filename = 'FLAL_low_PFPR_calibration_f_0p8/ONELOC_%s_%dRMDA_beta_%s_OPPUNIFORM_FLAL.yml'%(kFormatter(popsize),mda_round,format(beta,'.5f'));
        output_stream = open(output_filename, 'w');
        yaml.dump(new_data, output_stream); 
        output_stream.close();
