#include "GeometricOption.hpp"

GeometricOption::GeometricOption(double T, int dates, int size, double strike)
{
    T_ = T;
    dates_ = dates;
    size_ = size;
    strike_ = strike;
}

double GeometricOption::payoff(const PnlMat* path)
{
    double productShares = 1;
    for (int d = 0; d < size_; d++)
    {
        productShares *= pnl_mat_get(path, dates_, d);
    }
    double exponent = 1.0 / size_;
    productShares = pow(productShares, exponent);
    double ourPayoff = strike_ - productShares;
    if (ourPayoff > 0)
    {
        return ourPayoff;
    } 
    else
    {
        return 0;
    }
}

double GeometricOption::payoff(const PnlVect* pricesAt)
{
    double productShares = 1;
    for (int d = 0; d < size_; d++)
    {
        productShares *= pnl_vect_get(pricesAt, d);
    }
    double exponent = 1.0 / size_;
    productShares = pow(productShares, exponent); 
    double ourPayoff = strike_ - productShares;
    if (ourPayoff > 0)
    {
        return ourPayoff;
    } 
    else
    {
        return 0;
    }
}


GeometricOption::~GeometricOption(){
}