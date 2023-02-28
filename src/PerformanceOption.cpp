#include "PerformanceOption.hpp"

PerformanceOption::PerformanceOption(double T, int dates, int size, double strike, PnlVect * payoffCoefficientsVector)
{
    T_ = T;
    dates_ = dates;
    size_ = size;
    strike_ = strike;
    payoffCoefficientsVector_ = payoffCoefficientsVector;
}

double PerformanceOption::payoff(const PnlMat* path)
{
    PnlVect* ActionPricesInT = pnl_vect_create(size_);
    pnl_mat_get_row(ActionPricesInT, path, dates_);
    double weightedPrice = pnl_vect_get(payoffCoefficientsVector_, 0) * pnl_vect_get(ActionPricesInT, 0);
    for(int d = 1; d < size_; d++)
    {
        if(weightedPrice <= pnl_vect_get(payoffCoefficientsVector_, d) * pnl_vect_get(ActionPricesInT, d))
        {
            weightedPrice = pnl_vect_get(payoffCoefficientsVector_, d) * pnl_vect_get(ActionPricesInT, d);
        }
    }
    double payoff = weightedPrice - strike_;
    if (payoff > 0)
    {
        return payoff;
    }
    else
    {
        return 0;    
    }
}

double PerformanceOption::payoff(const PnlVect* pricesAt)
{
    double weightedPrice = pnl_vect_get(payoffCoefficientsVector_, 0) * pnl_vect_get(pricesAt, 0);
    for(int d = 1; d < size_; d++)
    {
        if(weightedPrice <= pnl_vect_get(payoffCoefficientsVector_, d) * pnl_vect_get(pricesAt, d))
        {
            weightedPrice = pnl_vect_get(payoffCoefficientsVector_, d) * pnl_vect_get(pricesAt, d);
        }
    }
    double payoff = weightedPrice - strike_;
    if(payoff > 0)
    {
        return payoff;
    }
    else
    {
        return 0;    
    }
}

PerformanceOption::~PerformanceOption()
{
    pnl_vect_free(&payoffCoefficientsVector_);
}