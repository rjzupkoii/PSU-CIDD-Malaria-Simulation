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


stream = open('input_FLAL.yml', 'r');
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
popsize = 300000
data['location_db']['population_size_by_location'] = [popsize];       

#3RMDA
number_MDA_round = [0,1,2,3,4];

#
sd_prob_individual_present_at_mda = [0.3, 0.3, 0.3]
data['sd_prob_individual_present_at_mda'] = sd_prob_individual_present_at_mda

#for index,event in enumerate(data['events']):
#    if event['name'] == 'single_round_MDA':
#        data['events'][index]['info'] = data['events'][index]['info'][0:number_MDA_round]


betas = [0.184865, 0.0776493, 0.0649498, 0.058,0.053]

pfpr = {0.184865: 'PFPR15',        
        0.0776493: 'PFPR5',
        0.0649498: 'PFPR3',
        0.058: 'PFPR2',
        0.053: 'PFPR1'}

improved_tc = {True: '_itc' , 
               False: ''}


for mda_round in number_MDA_round:
    for beta in betas:
        for _,itc in improved_tc.items():                
            new_data = copy.deepcopy(data)
            new_data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
            
            for index,event in enumerate(data['events']):
                if event['name'] == 'single_round_MDA':
                    new_data['events'][index]['info'] = data['events'][index]['info'][0:mda_round]                    
            pfpr_str = pfpr[beta]#            
            if itc == '':
                for index,event in enumerate(data['events']):
                    if event['name'] == 'change_treatment_coverage':
                        new_data['events'][index]['info']= []
        
            output_filename = 'FLAL/%s/ONELOC_%s_%dRMDA_%s_OPPUNIFORM_FLAL%s.yml'%(kFormatter(popsize), kFormatter(popsize),mda_round,pfpr_str,itc);
            output_stream = open(output_filename, 'w');
            yaml.dump(new_data, output_stream); 
            output_stream.close();

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