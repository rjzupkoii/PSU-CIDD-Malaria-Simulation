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
import numpy as np
original_beta = np.array([0.06066162,0.06433545,0.046031743,0.04075625,0.055,0.06790156,0.047468297,0.039184507,0.021281302,0.11,0.14,0,0.11,0.057700768,0.08435011,0.14,0.055,0.04,0.13569869,0.010148501,0.005594737,0.052648053,0.028757822,0.046417046,0.055905398]);
original_beta_sd = original_beta/3;

def loguniform(low=0, high=1, size=None):
    return np.exp(np.random.uniform(low, high, size))

def ran_beta():
    result = np.random.normal(original_beta,original_beta_sd)    
    result[result < 0] = 0.001
    return result

#%%

number_of_locations = 25
number_of_beta_sets = 400
betas = [];
for i in range(number_of_beta_sets):
#    betas.append(loguniform(log(0.01),log(0.1),number_of_locations))
    betas.append(ran_beta())


#%%
for index,beta in enumerate(betas):
    data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
    output_filename = 'Cambodia/pre4/beta/input_beta_%d.yml'%index;
    output_stream = open(output_filename, 'w');
    yaml.dump(data, output_stream);
    output_stream.close();

#%%

import numpy
a = numpy.asarray(betas)
numpy.savetxt("Cambodia/pre4/beta.csv", a, delimiter=",")

#%%
import pandas as pd

import seaborn as sns
import matplotlib.pyplot as plt
sns.set(style="darkgrid")


betas= pd.read_csv('Cambodia/pre4/beta.csv',delimiter=',',header=None)

g = sns.boxplot(x="variable", y="value", data=pd.melt(betas))



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