#pragma once

#include <iostream>
#include <vector>
#include "Option.hpp"
#include "BlackScholesModel.hpp"
#include "pnl/pnl_random.h"
#include "pnl/pnl_basis.h"


using namespace std;

class MonteCarlo
{
public:
    BlackScholesModel *mod_; /*! pointeur vers le modèle */
    Option *opt_; /*! pointeur sur l'option */
    PnlRng* rng_;            /*! pointeur sur le générateur */
    long nbSamples_;         /*! nombre de tirages Monte Carlo */
    int polynomDeg_;          /*! Degré du polynome dans la régression */
    double timeStep;
    PnlBasis * polynom;


    /**
     * Constructeur de la classe MonteCarlo 
     */
    MonteCarlo(BlackScholesModel* mod, Option* opt, PnlRng* rng, long nbSamples, int polynomDeg);


    /*
    *Simulates nbSamples_ paths and stores them in a vector<PnlMat *>
    **/
    vector<PnlMat *> simulateSamples();

    /**
     *returns indexes of paths that are in the money at date n
    */
    vector<int> inTheMoney(vector<PnlMat *> simulations, int n);

    /**
     *Computes the regression coefficients that are in the money at instant n
    */
    void getRegressionCoefficients(PnlVect * alphas, PnlVect * stoppingTime, int n, vector<PnlMat *> simulations,  vector<int> inTheMoneySimulations);

    /**
     *Updates the strategy at instant n. 
    */
    void updateStoppingTime(PnlVect * stoppingTime, int n, PnlVect * alphas, vector<PnlMat *> simulations, vector<int> inTheMoneySimulations);


    /**
     *Compute the option price by averaging the discounted payoff of the paths
    at the stopping times.
    */
    double computePriceFinal(PnlVect * stoppingTime, vector<PnlMat *> simulations);


    void freeSamples(vector<PnlMat *> simulations);

    /**
     * Calcule le prix de l'option à la date 0
     *
     * @return valeur de l'estimateur Monte Carlo
     */
    double price();



};


