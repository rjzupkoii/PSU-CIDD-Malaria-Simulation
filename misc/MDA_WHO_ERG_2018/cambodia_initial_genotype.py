# -*- coding: utf-8 -*-
"""
Created on Thu Aug 23 12:55:54 2018

@author: NguyenTran
"""

#%%
import pandas as pd
#import numpy as np
loci =  [ 
        ['K','T'] , 
        ['NY--', 'NF--', 'YY--','YF--','NYNY', 'NFNF','YYYY','YFYF'], 
        ['C','Y'], 
        ['1','2'] ,
        [ 'x','X'] 
        ]

loci_distribution =  [ 
        [.2,.8] , 
        [.5*.9*.7, .5*.1*.7, .5*.9*.7,.5*.1*.7,.5*.9*.3, .5*.1*.3,.5*.9*.3,.5*.1*.3], 
        [.99,.01], 
        [1,0] ,
        [ 1,0] 
        ]
#%%


genotype_db = ['']
genotype_db_distribution = [0.1]
for idx,locus in enumerate(loci):
#    print(idx, locus)
    current_size = len(genotype_db)
#    print(current_size)
    new_genotype_db=[]
    new_genotype_db_distribution=[]
    
    for idx2,allele in enumerate(locus):
        for i in range(current_size):
            new_genotype = genotype_db[i]+ allele
            new_genotype_distribution = genotype_db_distribution[i]*loci_distribution[idx][idx2]
            new_genotype_db.append(new_genotype)
            new_genotype_db_distribution.append(new_genotype_distribution)
#            print(new_genotype,new_genotype_distribution)
    genotype_db = new_genotype_db
    genotype_db_distribution = new_genotype_db_distribution

genotype_db_csv= pd.read_csv('genotypeID.txt', sep='\t')

labels = ['shortName', 'distribution']
df = pd.DataFrame({'name': genotype_db, 'distribution':genotype_db_distribution})

genotype_db_csv = genotype_db_csv.join(df.set_index('name'), on='Shortname')


pos_from = 0
pos_to = 0

for idx,locus in enumerate(loci):    
    pos_to = pos_to + len(locus[0])
    for idx2,allele in enumerate(locus):
         print(allele,sum(genotype_db_csv.loc[genotype_db_csv['Shortname'].str[pos_from:pos_to]==allele]['distribution']), sep=' : ')         
    pos_from = pos_from + len(locus[0])
    
genotype_list= []    
for i,row in genotype_db_csv.iterrows():    
    if row['distribution'] != 0:
        genotype_list.append( {'parasite_type_id': row['ID'], 'prevalence' : row['distribution']})        
#        print(row['ID'], row['distribution'], sep = ' : ')
#%%
import yaml;
print(yaml.dump(genotype_list))
        
         