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
#original_beta = [0.13356084,0.15162966,0.09243984,0.16028422,0.20697847,0.15419583,0.10739227,0.09094083,0.16582377,0.23753038,0.21442202,0.096879,0.19005938,0.13223566,0.13402116,0.30217877,0.14398083,0.16429467,0.17459542,0.15144762,0.14584802,0.17901,0.08933166,0.13688217,0.21134582];

def loguniform(low=0, high=1, size=None):
    return np.exp(np.random.uniform(low, high, size))

def ran_beta(alpha=2, beta=1, size=None):
    return np.random.beta(alpha, beta, size)*0.08 + 0.01

#%%

number_of_locations = 25
number_of_beta_sets = 400
betas = [];
for i in range(number_of_beta_sets):
    betas.append(loguniform(log(0.01),log(0.08),number_of_locations))
#    betas.append(ran_beta(2, 5,number_of_locations))


#%%
for index,beta in enumerate(betas):
    data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
    output_filename = 'Cambodia/pre3/beta/input_beta_%d.yml'%index;
    output_stream = open(output_filename, 'w');
    yaml.dump(data, output_stream);
    output_stream.close();

#%%

import numpy
a = numpy.asarray(betas)
numpy.savetxt("Cambodia/pre3/beta.csv", a, delimiter=",")
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