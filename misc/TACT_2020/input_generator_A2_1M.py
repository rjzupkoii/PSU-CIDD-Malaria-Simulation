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


stream = open('input_base_A2_mu_0p001983.yml', 'r');
data = yaml.load(stream);
stream.close();

 # COMMENT IT LATER
# data['ending_date'] = '2022/1/1';

location_info =  [[0, 0, 0]];
number_of_locations = len(location_info);
data['location_db']['location_info']= location_info;

#population size 
popsize = 1000000
data['location_db']['population_size_by_location'] = [popsize];       

#3RMDA
number_MDA_round = [0,1,2,3,4];

tcs = {
       0.25: {
           'tc_str': '0p25',
           'pfprs': {
               0.12: 'PFPR10p0',
               0.048: 'PFPR01p0',
               0.043: 'PFPR00p1'
               }           
           },
       0.5: {
           'tc_str': '0p50',
           'pfprs': {
               0.14: 'PFPR10p0',
               0.054: 'PFPR01p0',
               0.048: 'PFPR00p1'
               }           
           },
       0.75: {
           'tc_str': '0p75',
           'pfprs': {
               0.17: 'PFPR10p0',
               0.06: 'PFPR01p0',
               0.055: 'PFPR00p1'
               }           
           },
       }

tact = {
        1: 'ASAQ',
        4: 'ALAQ',
        5: 'AMP'
        }

for tc, tc_details in tcs.items():
    tc_str = tc_details['tc_str'];
    pfprs = tc_details['pfprs'];
    for beta,pfpr in pfprs.items():
        for tact_id, tact_str in tact.items():
            print(beta, tc_str, tact_id)
            new_data = copy.deepcopy(data)
            new_data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
            
            new_data['location_db']['p_treatment_for_less_than_5_by_location'] = np.full(number_of_locations, tc).tolist()
            new_data['location_db']['p_treatment_for_more_than_5_by_location'] = np.full(number_of_locations, tc).tolist()
            
            for index,event in enumerate(data['events']):                    
                if event['name'] == 'modify_nested_mft_strategy':
                    new_data['events'][index]['info'][0]['strategy_id'] = tact_id
                    
            output_filename = 'A2_1M/TACT_PFPR_%s_TC_%s_TACT_%s.yml'%( pfpr,tc_str, tact_str);
            output_stream = open(output_filename, 'w');
            yaml.dump(new_data, output_stream); 
            output_stream.close();


# for mda_round in number_MDA_round:
#     for beta in betas:
#         for _,itc in improved_tc.items():                
#             new_data = copy.deepcopy(data)
#             new_data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
            
#             for index,event in enumerate(data['events']):
#                 if event['name'] == 'single_round_MDA':
#                     new_data['events'][index]['info'] = data['events'][index]['info'][0:mda_round]
                    
#                 if event['name'] == 'modify_nested_mft_strategy':
#                     new_data['events'][index]['info'][0]['day'] = switch_OZFQ_dates[mda_round]
                    
#             pfpr_str = pfpr[beta]#            
#             if itc == '':
#                 for index,event in enumerate(data['events']):
#                     if event['name'] == 'change_treatment_coverage':
#                         new_data['events'][index]['info']= []
        
#             output_filename = 'FigureS2.3/%s/ONELOC_%s_%dRMDA_%s_OPPUNIFORM_FLAL_imp_OZ%s.yml'%(kFormatter(popsize), kFormatter(popsize),mda_round,pfpr_str,itc);
#             output_stream = open(output_filename, 'w');
#             yaml.dump(new_data, output_stream); 
#             output_stream.close();

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