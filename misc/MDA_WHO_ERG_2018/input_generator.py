# -*- coding: utf-8 -*-
"""
Created on Tue Aug  7 14:23:33 2018

@author: NguyenTran
"""

import yaml;

import inflect;
p = inflect.engine();

start_date= "2000/1/1";
end_date= "2022/1/1";

stream = open('input.yml', 'r');
data = yaml.load(stream);

#1 location
location_info =  [[0, 0, 0]];
number_of_locations = len(location_info);
data['location_db']['location_info']= location_info;


#population size 
population_size_by_location = [300000];
data['location_db']['population_size_by_location'] = population_size_by_location;       

#3RMDA
number_MDA_round = 0;

#OPPUNIFORM

#FLAL  

#
#def kFormatter(num):
#    return str(num) if num <=999 else str(round(num/1000)) +'k';
#
#print(kFormatter(9000));
#print(p.number_to_words( number_of_locations, threshold=10));

#output_filename = 'ONELOC_300K_3RMDA_PFPR15_OPPUNIFORM_FLAL.yml';

output_filename = 'input_test.yml';

output_stream = open(output_filename, 'w');
yaml.dump(data, output_stream);