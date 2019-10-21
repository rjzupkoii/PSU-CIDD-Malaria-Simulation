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


stream = open('input_FLAL_no_importation.yml', 'r');
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
popsize = 40000
data['location_db']['population_size_by_location'] = [popsize];       

#3RMDA
number_MDA_round = [0,1,2,3,4];

#
sd_prob_individual_present_at_mda = [0.3, 0.3, 0.3]
data['sd_prob_individual_present_at_mda'] = sd_prob_individual_present_at_mda

#for index,event in enumerate(data['events']):
#    if event['name'] == 'single_round_MDA':
#        data['events'][index]['info'] = data['events'][index]['info'][0:number_MDA_round]


pfprs = {
        0.06413: 'PFPR3',
        0.0585: 'PFPR2',
        0.0538: 'PFPR1'
        }

p_plas2s = {
        0.01: '0p01',
        0.02: '0p02',
        0.05: '0p05',
        0.10: '0p10',
        }

improved_tc = {True: '_itc' , 
               False: ''}

for mda_round in number_MDA_round:
    for beta,pfpr_str in pfprs.items():
        for p_plas2, p_plas2_str in p_plas2s.items():      
            for _,itc in improved_tc.items():
                new_data = copy.deepcopy(data)
                new_data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()            
    
                
                for index,event in enumerate(data['events']):
                    if event['name'] == 'single_round_MDA':
                        new_data['events'][index]['info'] = data['events'][index]['info'][0:mda_round]
#                        if mda_round >0 :
#                            print(new_data['events'][index]['info'])
                    
                if itc == '':
                    for index,event in enumerate(data['events']):
                        if event['name'] == 'change_treatment_coverage':
                            new_data['events'][index]['info']= []
                new_data['events'].append({'name': 'introduce_plas2_parasites', 'info':[{'location': 0,'day': '2019/11/1', 'fraction': p_plas2}]})
                
                print(mda_round, beta, p_plas2_str, itc);
                output_filename = 'FLAL_varied_plas2_2020_no_importation/%s/ONELOC_%s_%dRMDA_%s_OPPUNIFORM_FLAL_plas2_%s%s.yml'%(kFormatter(popsize), kFormatter(popsize),mda_round,pfpr_str,p_plas2_str,itc);
                output_stream = open(output_filename, 'w');
                yaml.dump(new_data, output_stream); 
                output_stream.close();
