/* 
 * File:   Random.cpp
 * Author: nguyentran
 * 
 * Created on May 27, 2013, 10:46 AM
 */

#include "Random.h"
#include <iostream>
//#include <cstdlib>
#include <fstream>
#include <ctime>
#include <math.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>

Random::Random(Model* model, gsl_rng* g_rng) : model_(model), G_RNG(g_rng), bitting_level_generator_(), moving_level_generator_(), external_population_moving_level_generator_() {

}

Random::~Random() {
    release();
}

void Random::initialize(const unsigned int& seed) {
    // get random number seed from current time
    //    int seed = time(0);

    const gsl_rng_type *TT = gsl_rng_mt19937;
    G_RNG = gsl_rng_alloc(TT);

    // seed the RNG
    if (seed == -1) {
        seed_ = good_seed();
        gsl_rng_set(G_RNG, seed_);
    } else {
        seed_ = seed;
        gsl_rng_set(G_RNG, seed_);
    }

}

unsigned int Random::good_seed() {
    unsigned int random_seed, random_seed_a, random_seed_b;
    std::ifstream file("/dev/urandom", std::ios::binary);
    if (file.is_open()) {
        char * memblock;
        const int size = sizeof (int);
        memblock = new char [size];
        file.read(memblock, size);
        file.close();
        random_seed_a = *reinterpret_cast<int*> (memblock);
        delete[] memblock;
    }// end if
    else {
        random_seed_a = 0;
    }
    random_seed_b = std::time(0);
    random_seed = random_seed_a ^ random_seed_b;

    return random_seed;
}

void Random::release() {
    gsl_rng_free(G_RNG);
}

int Random::random_poisson(const double& poisson_mean) {
    return gsl_ran_poisson(G_RNG, poisson_mean);
}

int Random::random_uniform(const int& range) {
    return gsl_rng_uniform_int(G_RNG, range);
}

//return an interger in  [from, to) , not include to
int Random::random_uniform_int(const int& from, const int& to) {
    return from + gsl_rng_uniform_int(G_RNG, to - from);
}

double Random::random_uniform_double(const double& from, const double& to) {
    //    return from + gsl_rng_uniform_pos(G_RNG)*(to-from);
    return gsl_ran_flat(G_RNG, from, to);
}

double Random::random_uniform() {
    return gsl_rng_uniform(G_RNG);
}

double Random::random_normal(const double& mean, const double& sd) {
    return mean + gsl_ran_gaussian(G_RNG, sd);
}

double Random::random_normal_truncated(const double& mean, const double& sd) {
    double value = gsl_ran_gaussian(G_RNG, sd);
    while (value > 3 * sd || value < -3 * sd) {
        value = gsl_ran_gaussian(G_RNG, sd);
    }

    return mean + value;
}

int Random::random_normal(const int& mean, const int& sd) {
    return mean + floor(gsl_ran_gaussian(G_RNG, sd) + 0.5);
}

int Random::random_normal_truncated(const int& mean, const int& sd) {
    double value = gsl_ran_gaussian(G_RNG, sd);
    while (value > 3 * sd || value < -3 * sd) {
        value = gsl_ran_gaussian(G_RNG, sd);
    }

    return mean + floor(value + 0.5);
}

double Random::random_beta(const double& alpha, const double& beta) {
    //if beta =0, alpha = means
    if (beta == 0)
        return alpha;
    return gsl_ran_beta(G_RNG, alpha, beta);
}

//
// the parameterization below is done so alpha*beta = mean
// and alpha*beta^2 = variance
//

double Random::random_gamma(const double& shape, const double& scale) {
    //if beta =0, alpha = means
    if (scale == 0)
        return shape;
    return gsl_ran_gamma(G_RNG, shape, scale);
}

double Random::cdf_gamma_distribution(const double& x, const double& alpha, const double& beta) {
    //if beta =0, alpha = means
    if (beta == 0)
        return 1.0;
    return gsl_cdf_gamma_P(x, alpha, beta);
}

double Random::cdf_gamma_distribution_inverse(const double& P, const double& alpha, const double& beta) {
    return gsl_cdf_gamma_Pinv(P, alpha, beta);
}

double Random::random_flat(const double& from, const double& to) {
    return gsl_ran_flat(G_RNG, from, to);
}

void Random::random_multinomial(const int& K, const unsigned& N, double p[], unsigned n[]) {
    gsl_ran_multinomial(G_RNG, K, N, p, n);
}

void Random::random_shuffle(void* base, size_t nmembm, size_t size) {
    gsl_ran_shuffle(G_RNG, base, nmembm, size);
}

int Random::random_biting_level() {
    //    std::cout << bitting_level_generator().draw_random_level(model_)<< std::endl;

    //    std::cout << "b" << std::endl;
    //    int value = bitting_level_generator().draw_random_level(model_);
    //    std::cout << "a" << std::endl;
    //    return value;
    return bitting_level_generator().draw_random_level(model_);
}

int Random::random_moving_level() {
    //    std::cout << "b" << std::endl;
    //    int value = moving_level_generator().draw_random_level(model_);
    //    std::cout << "a" << std::endl;
    //    return value;

    return moving_level_generator().draw_random_level(model_);
}

int Random::random_external_population_moving_level() {
    return external_population_moving_level_generator().draw_random_level(model_);
}

double Random::cdf_standard_normal_distribution(const double& p) {
    return gsl_cdf_ugaussian_P(p);
}

int Random::random_binomial(const double& p, const int& n) {
    return gsl_ran_binomial(G_RNG, p, n);
}
