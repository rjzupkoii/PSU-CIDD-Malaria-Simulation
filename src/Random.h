/* 
 * File:   Random.h
 * Author: nguyentran
 *
 * Created on May 27, 2013, 10:46 AM
 */

#ifndef RANDOM_H
#define	RANDOM_H

#include <gsl/gsl_rng.h> // random number generators from Gnu Scientific Library
#include "PropertyMacro.h"
#include "BittingLevelGenerator.h"

class Model;

class Random {
    VIRTUAL_PROPERTY(unsigned int, seed);
    READ_ONLY_PROPERTY_REF(BittingLevelGenerator, bitting_level_generator)
    READ_ONLY_PROPERTY_REF(BittingLevelGenerator, moving_level_generator)
    READ_ONLY_PROPERTY_REF(BittingLevelGenerator, external_population_moving_level_generator)
    POINTER_PROPERTY(Model, model)
   
    
public:
    gsl_rng* G_RNG;

    Random(Model* model = nullptr,gsl_rng* g_rng = nullptr);

    virtual ~Random();

    void initialize(const unsigned int& seed = -1);

    void release();

    virtual int random_poisson(const double& poissonMeans);

    virtual int random_uniform(const int& range);
    virtual int random_uniform_int(const int& from, const int& to);
    virtual double random_uniform_double(const double& from, const double& to);
    

    /*
     * This function will return a random number in [0,1)
     */
    virtual double random_uniform();

    virtual double random_normal(const double& mean, const double& sd);
    virtual double random_normal_truncated(const double& mean, const double& sd);

    virtual int random_normal(const int& mean, const int& sd);
    virtual int random_normal_truncated(const int& mean, const int& sd);

    virtual double random_beta(const double& alpha, const double& beta);

    virtual double random_gamma(const double& shape, const double& scale);

    virtual double cdf_gamma_distribution(const double& x, const double& alpha, const double& beta);

    virtual double cdf_gamma_distribution_inverse(const double& p, const double& alpha, const double& beta);

    virtual double random_flat(const double& from, const double& to);

    virtual void random_multinomial(const int& K, const unsigned& N, double p[], unsigned n[]);
    
    virtual void random_shuffle( void * base, size_t nmembm, size_t size);

    virtual int random_biting_level();
    virtual int random_moving_level();
    virtual int random_external_population_moving_level();
    
    
    virtual double cdf_standard_normal_distribution(const double& p);
    
    virtual int random_binomial(const double& p, const int& n);
    
private:
    unsigned int good_seed();
};

#endif	/* RANDOM_H */

