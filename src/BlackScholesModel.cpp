#include <iostream>
#include "BlackScholesModel.hpp"
using namespace std;

 /**
     Constructeur à quatre paramètres définissant le modèle BlackScholes 
     */

BlackScholesModel::BlackScholesModel(int size, double r, double rho, PnlVect *sigma, PnlVect *divid, PnlVect *spot)
{
    size_ = size;
    r_ = r;
    rho_ = rho;
    sigma_ = pnl_vect_create(size_);
    pnl_vect_clone(sigma_, sigma);
    divid_ = pnl_vect_create(size_);
    pnl_vect_clone(divid_, divid);
    spot_ = pnl_vect_create(size_);
    pnl_vect_clone(spot_, spot);
    cholesky = pnl_mat_create_from_scalar(size_, size_, rho_);
    choleskyComposition(cholesky);
    choleskyComponent = pnl_vect_create(size_);
    gaussianVector = pnl_vect_create(size_);
    currentShares = pnl_vect_create(size_);
    nextShares = pnl_vect_create(size_);
}

void BlackScholesModel::choleskyComposition(PnlMat* cholesky)
{
    for(int d = 0; d < size_; d++)
    {
        pnl_mat_set_diag(cholesky, 1, d);
    }
    pnl_mat_chol(cholesky);
}

void BlackScholesModel::asset(PnlMat *path, double T, int dates, PnlRng *rng)
{
    double timeStep = T / dates;
    PnlMat* gaussian = pnl_mat_create(dates, size_);
    pnl_mat_rng_normal(gaussian, dates + 1, size_, rng);
    pnl_mat_set_row(path, spot_, 0);
    pnl_mat_get_row(currentShares, path, 0);
    PnlVect * expo = pnl_vect_create(size_);
    for(int d = 0; d < size_; d++)
    {
        double deviation = pnl_vect_get(sigma_, d);
        pnl_vect_set(expo, d, exp((r_ - pnl_vect_get(divid_, d) - deviation * deviation / 2) * timeStep));
    }
    for(int i = 1; i < dates + 1; i++)
    {
        pnl_mat_get_row(gaussianVector, gaussian, i);
        for(int d = 0; d < size_; d++)
        {
            pnl_mat_get_row(choleskyComponent, cholesky, d);
            double deviation = pnl_vect_get(sigma_, d);
            double scaleCholeskyGaussian = pnl_vect_scalar_prod(choleskyComponent, gaussianVector);
            double computedShare = pnl_vect_get(currentShares, d) * pnl_vect_get(expo, d) * exp(deviation * sqrt(timeStep) * scaleCholeskyGaussian);
            pnl_vect_set(nextShares, d, computedShare);
        }
        pnl_mat_set_row(path, nextShares, i);
        pnl_vect_clone(currentShares, nextShares);
    }
    pnl_vect_free(&expo);
    pnl_mat_free(&gaussian);
}