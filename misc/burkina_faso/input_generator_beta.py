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

stream = open('input_BF_Beta.yml', 'r');
data = yaml.load(stream);
#%%
import numpy as np
original_beta = np.array([0.1797668 ,  0.15293501,  0.34663612,  0.2344787 ,  0.20464383,0.21876676,  0.47167355,  0.316815  ,  0.10966629,  0.18364811,0.09143069,  0.12007236,  0.16078202,  0.11727253,  0.09351677,0.1742388 ,  0.39237672,  0.37839532,  0.13445085,  0.23445965,0.13207692,  0.06955081,  0.16047367,  0.48288664,  0.66  ,10.752139  ,  0.5313675 ,  0.12206222,  0.3429092 ,  0.24306174,0.1946386 ,  0.12191316,  0.1761821 ,  0.09442411,  0.10265338,0.08236716,  0.12562111,  0.6 ,  0.22578228,  0.4, 0.42,  0.27098376,  0.15900558,  0.5581483 ,  0.29239067]);
original_beta_sd = original_beta/3;

def loguniform(low=0, high=1, size=None):
    return np.exp(np.random.uniform(low, high, size))

def ran_beta():
    result = np.random.normal(original_beta,original_beta_sd)    
    result[result < 0] = 0.01
    return result

#%%

number_of_locations = 45
number_of_beta_sets = 400
betas = [];
for i in range(number_of_beta_sets):
#    betas.append(loguniform(log(0.01),log(0.1),number_of_locations))
    betas.append(ran_beta())


#%%
for index,beta in enumerate(betas):
    data['location_db']['beta_by_location'] = np.full(number_of_locations, beta).tolist()
    output_filename = 'beta_fitting/input_beta_%d.yml'%index;
    output_stream = open(output_filename, 'w');
    yaml.dump(data, output_stream);
    output_stream.close();

#%%

import numpy
a = numpy.asarray(betas)
numpy.savetxt("beta_fitting/beta.csv", a, delimiter=",")

#%%
import pandas as pd

import seaborn as sns
import matplotlib.pyplot as plt
sns.set(style="darkgrid")


betas= pd.read_csv('beta_fitting/beta.csv',delimiter=',',header=None)

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