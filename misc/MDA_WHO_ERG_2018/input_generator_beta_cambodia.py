# -*- coding: utf-8 -*-
"""
Created on Tue Aug  7 14:23:33 2018

@author: NguyenTran
"""
#%%
import yaml;
import numpy as np;
from math import log ;
import inflect;
p = inflect.engine();

stream = open('input_Cambodia.yml', 'r');
data = yaml.load(stream);
#%%

def loguniform(low=0, high=1, size=None):
    return np.exp(np.random.uniform(low, high, size))

def ran_beta(alpha=2, beta=1, size=None):
    return np.random.beta(alpha, beta, size)*0.4 + 0.02

#%%

number_of_locations = 25
number_of_beta_sets = 400
betas = [];
for i in range(number_of_beta_sets):
#    betas.append(loguniform(log(0.01),log(0.1),number_of_locations))
    betas.append(ran_beta(2, 5,number_of_locations))


#%%
for index,beta in enumerate(betas):
    data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
    output_filename = 'Cambodia/run2/beta/input_beta_%d.yml'%index;
    output_stream = open(output_filename, 'w');
    yaml.dump(data, output_stream);
    output_stream.close();

#%%

import numpy
a = numpy.asarray(betas)
numpy.savetxt("Cambodia/run2/beta.csv", a, delimiter=",")
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