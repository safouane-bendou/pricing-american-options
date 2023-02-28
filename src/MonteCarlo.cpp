#include "MonteCarlo.hpp"


/**
    Constructeur à cinq arguments d'un objet Monte carlo
*/

MonteCarlo::MonteCarlo(BlackScholesModel* mod, Option* opt, PnlRng* rng, long nbSamples, int polynomDeg)
{
    mod_ = mod;
    opt_ = opt;
    rng_ = rng;
    nbSamples_ = nbSamples;
    polynomDeg_ = polynomDeg;
    timeStep = opt_->T_ / (double)opt_->dates_;
    polynom = pnl_basis_create_from_degree(PNL_BASIS_HERMITE, polynomDeg_, mod_->size_);;
}


/*
*Simulates nbSamples_ paths and stores them in a vector<PnlMat *>
**/
vector<PnlMat *> MonteCarlo::simulateSamples()
{
  vector<PnlMat *> samples;
  for(int round = 0; round < nbSamples_; round++)
  {
    PnlMat * path = pnl_mat_create(opt_->dates_ + 1, mod_->size_);
    mod_->asset(path, opt_->T_, opt_->dates_, rng_);
    samples.push_back(path);
  }
  return samples;
}


/**
 *returns indexes of paths that are in the money at date n
*/
vector<int> MonteCarlo::inTheMoney(vector<PnlMat *> simulations, int n)
{
  vector<int> inTheMoneySimulations;
  for(int round = 0; round < nbSamples_; round++)
  {
    PnlVect * currentSample = pnl_vect_create(mod_->size_);
    pnl_mat_get_row(currentSample, simulations[round], n); 
    if(opt_->payoff(currentSample) > 0)
    {
      inTheMoneySimulations.push_back(round);
    }
  }
  return inTheMoneySimulations;
}

/**
 *Computes the regression coefficients that are in the money at instant n
*/

void MonteCarlo::getRegressionCoefficients(PnlVect * alphas, PnlVect * stoppingTime, int n, vector<PnlMat *> simulations,  vector<int> inTheMoneySimulations)
{
  PnlMat * x = pnl_mat_create(inTheMoneySimulations.size(), mod_->size_);
  PnlVect * y = pnl_vect_create(inTheMoneySimulations.size());
  //We compute the matrix X and the vector Y of the regression
  PnlVect * currentSample = pnl_vect_create(mod_->size_);
  for(int round = 0; round < inTheMoneySimulations.size(); round++)
  {
    pnl_mat_get_row(currentSample, simulations[inTheMoneySimulations[round]], n);   // we only work with in the money paths  
    pnl_mat_set_row(x, currentSample, round);
    int indexStopping = pnl_vect_get(stoppingTime, inTheMoneySimulations[round]);
    PnlVect * roundSampleAtStoppingTime = pnl_vect_create(mod_->size_);
    pnl_mat_get_row(roundSampleAtStoppingTime, simulations[inTheMoneySimulations[round]], indexStopping); 
    double component = exp(-mod_->r_ * opt_->T_ * (indexStopping - n)  / opt_->dates_) * opt_->payoff(roundSampleAtStoppingTime);
    pnl_vect_set(y, round, component);
  }
  pnl_basis_fit_ls(polynom, alphas, x, y);
  pnl_mat_free(&x);
  pnl_vect_free(&y);
}


/**
 Updates the strategy at instant n. 
*/
void MonteCarlo::updateStoppingTime(PnlVect * stoppingTime, int n, PnlVect * alphas, vector<PnlMat *> simulations, vector<int> inTheMoneySimulations)
{
  PnlVect * currentSample = pnl_vect_create(mod_->size_); 
  for(int round = 0; round < inTheMoneySimulations.size(); round++)
  {   
    pnl_mat_get_row(currentSample, simulations[inTheMoneySimulations[round]], n);
    double conditionalExpectation = pnl_basis_eval_vect(polynom, alphas, currentSample);
    if(opt_->payoff(currentSample) >= conditionalExpectation)
    {
      pnl_vect_set(stoppingTime, inTheMoneySimulations[round], n);
    }
  }
}


/**
 *Computes the option price by averaging the discounted payoff of the paths
  at the stopping times.
*/
double MonteCarlo::computePriceFinal(PnlVect * stoppingTime, vector<PnlMat *> simulations)
{
  PnlVect * roundSampleAtStoppingTime = pnl_vect_create(mod_->size_);
  double price = 0;
  for(int round = 0; round < nbSamples_; round++)
  {
    int indexStopping = pnl_vect_get(stoppingTime, round);
    pnl_mat_get_row(roundSampleAtStoppingTime, simulations[round], indexStopping); 
    price += exp(-mod_->r_ * opt_->T_ * indexStopping / opt_->dates_) * opt_->payoff(roundSampleAtStoppingTime);
  }
  return price / nbSamples_;
}


void MonteCarlo::freeSamples(vector<PnlMat *> simulations)
{
  for(auto& sample : simulations)
  {
      pnl_mat_free(&sample);
  }
}

/**
 * Computes the option price
*/
double MonteCarlo::price()
{
  vector<PnlMat *> simulations = simulateSamples();
  PnlVect * alphas = pnl_vect_create(polynomDeg_);
  PnlVect * stoppingTime = pnl_vect_create_from_scalar(nbSamples_, opt_->dates_);
  for(int n = opt_->dates_ - 1; n > 0; n--)
  {
    vector<int> inTheMoneySimulations = inTheMoney(simulations, n);
    getRegressionCoefficients(alphas, stoppingTime, n, simulations, inTheMoneySimulations);
    updateStoppingTime(stoppingTime, n, alphas, simulations, inTheMoneySimulations);
  }
  double price = computePriceFinal(stoppingTime, simulations);
  freeSamples(simulations);
  return max(price, opt_->payoff(mod_->spot_));
}




