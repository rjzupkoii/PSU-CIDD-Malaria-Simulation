# -*- coding: utf-8 -*-
"""
Created on Tue Aug  7 14:23:33 2018

@author: NguyenTran
"""

import yaml;
import numpy as np;
from math import log ;
#import inflect;
#p = inflect.engine();

#%%
def kFormatter(num):
    return str(num) if num <=999 else str(round(num/1000)) +'k';


stream = open('input_FLAL.yml', 'r');
data = yaml.load(stream);

data['starting_date'] = '2006/1/1';
data['ending_date'] = '2022/1/1';
data['start_of_comparison_period']= '2020/1/1';

data['seasonal_info']['enable'] = 'false';

#1 location
location_info =  [[0, 0, 0]];
number_of_locations = len(location_info);
data['location_db']['location_info']= location_info;


#population size 
data['location_db']['population_size_by_location'] = [1000000];       

#3RMDA
number_MDA_round = 0;
for index,event in enumerate(data['events']):
    if event['name'] == 'single_round_MDA':
        data['events'][index]['info'] = data['events'][index]['info'][0:number_MDA_round]


betas = np.logspace(log(0.04,10), log(0.5,10), num= 100)


#%%

for index,beta in enumerate(betas):
    data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
    output_filename = 'beta/input_beta_%d.yml'%index;
    output_stream = open(output_filename, 'w');
    yaml.dump(data, output_stream);
    output_stream.close();

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