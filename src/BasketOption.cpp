#include "BasketOption.hpp"

BasketOption::BasketOption(double T, int dates, int size, double strike, PnlVect * payoffCoefficientsVector) {
    T_ = T;
    dates_ = dates;
    size_ = size;
    strike_ = strike;
    payoffCoefficientsVector_ = payoffCoefficientsVector;
}

double BasketOption::payoff(const PnlMat* path)
{
    PnlVect* ActionPricesInT = pnl_vect_create(size_);
    pnl_mat_get_row(ActionPricesInT, path, dates_);
    double sum = pnl_vect_scalar_prod(payoffCoefficientsVector_, ActionPricesInT);

    pnl_vect_free(&ActionPricesInT);
    double payoff = sum - strike_;
    if (payoff > 0)
    {
        return payoff;
    } 
    else
    {
        return 0;    
    }   
}

double BasketOption::payoff(const PnlVect* pricesAt)
{
    double sum = pnl_vect_scalar_prod(payoffCoefficientsVector_, pricesAt);
    double payoff = sum - strike_;
    if (payoff > 0)
    {
        return payoff;
    } 
    else
    {
        return 0;    
    }   
}

BasketOption::~BasketOption()
{
    pnl_vect_free(&payoffCoefficientsVector_);
}