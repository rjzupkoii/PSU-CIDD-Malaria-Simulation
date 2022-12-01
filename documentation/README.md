# MaSim Overview

MaSim is a stochastic, [individually-based model](https://en.wikipedia.org/wiki/Agent-based_model) (or individually-based microsimulation) designed to investigate the evolution of antimalarial resistance by the *[Plasmodium falciparum](https://en.wikipedia.org/wiki/Plasmodium_falciparum)* in the presence of an antimalarial therapy given to a symptomatic individual, with transmission driven by a force of infection model. The simulation may be run as either a regionally-based model or a geographically-based model. While both approaches include the same levels of population heterogeneity, when run as a regionally-based model spatial heterogeneity (i.e., varying prevalence and transmission intensity) is limited whereas the geographically-based model allows for heterogeneity to be modeled at the configuration resolution of the simulation (e.g., 25 sq.km). While this document provides a high-level overview of the model and relevant sub-models, peer-reviewed documentation can be found in [Nguyen et al. (The Lancet Global Health, 2015)](https://doi.org/10.1016/S2214-109X(15)00162-X) and [Zupko et al. (PLOS Global Public Health, 2022)](https://journals.plos.org/globalpublichealth/article?id=10.1371/journal.pgph.0000111), with additional documentation in [Nguyen et al. (bioRxiv, 2021)](https://doi.org/10.1101/2021.03.08.434496).

## Individuals

All individuals within the simulation represent humans that may become infected by the *P. falciparum* parasite, which leads to either a symptomatic or asymptotic infection. In the event of a symptomatic infection, individuals will seek treatment at the configured rate and will receive a therapy based upon the configuration provided to the simulation. Upon taking a therapy, a simplified pharmacokinetic and pharmacodynamic (PK/PD) is used to determine the blood concentration of the drug(s) in blood. The blood concentration, combined with the drug resistance profile for the *P. falciparum* clone(s) determines the killing rate of the parasite and reduction of blood parasitemia. 

## Infection by *P. falciparum*

While *P. falciparum* is transmitted by female *Anopheles* mosquitos, as a modeling simplification a force of infection (FOI) model is used that presumes that transmission is driven in part by the total parasite load of all individuals within a given area. The likelihood that an individual is bitten is then determined by their individual biting attractiveness and a seasonal adjustment to account for the variable number of mosquitos present throughout the year. All bites are presumed to be infectious and the probability of infection is determined by the individual's underlying immune response to *P. falciparum*. 

## *P. falciparum* Genotypes

Upon model initialization, the circulating genotypes of *P. falciparum* is determined by the configuration; however, when mutations are enabled, there is a small probability of a mutation occurring. While this mutation may confer an evolutionary advantage (e.g., resistance to an antimalarial), if a companied by a fitness cost, the mutation may be lost or out competed by wild-type parasites without the mutation. Also included in the model is recombination due to interrupted feeding by a mosquito resulting in multiple clones being present in the gut. The probability for this occurring is based upon the configuration provided and the FOI of the circulating genotypes.

## Policy Interventions

Within the simulation, drug policy interventions (e.g., a change in first-line therapies) are supported with limited support for non-therapeutic interventions (e.g., bed net distribution). Due to the lack of an underlying vector model, interventions targeting the *Anopheles* mosquitos are not supported. 
