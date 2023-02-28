#include <iostream>
#include <string>
#include "BlackScholesModel.hpp"
#include "GeometricOption.hpp"
#include "BasketOption.hpp"
#include "PerformanceOption.hpp"
#include "MonteCarlo.hpp"
#include "jlparser/parser.hpp"
#include "PricingResults.hpp"


using namespace std;

int main(int argc, char **argv)
{
    double T, r, strike, rho;
    PnlVect *spot, *sigma, *divid, *payoffCoefficientsVector;
    string type;
    int size, dates;
    int n_samples;
    int polynomDeg;

    char *infile = argv[1];
    Param *P = new Parser(infile);

    P->extract("option type", type);
    P->extract("maturity", T);
    P->extract("model size", size);
    P->extract("spot", spot, size);
    P->extract("volatility", sigma, size);
    P->extract("correlation", rho);
    P->extract("interest rate", r);
    if (P->extract("dividend rate", divid, size, true) == false)
    {
        divid = pnl_vect_create_from_zero(size);
    }
    P->extract("dates", dates);
    P->extract("strike", strike);
    P->extract("MC iterations", n_samples);
    P->extract("degree for polynomial regression", polynomDeg);

    Option * option;
    if(type == "exchange"){
      P->extract("payoff coefficients", payoffCoefficientsVector, size);
      option = new BasketOption(T, dates, size, strike, payoffCoefficientsVector);
    
    }
    else if (type == "geometric_put"){
      option = new GeometricOption(T, dates, size, strike);
    }
    else if (type == "bestof"){
      P->extract("payoff coefficients", payoffCoefficientsVector, size);
      option = new PerformanceOption(T, dates, size, strike, payoffCoefficientsVector);
    }

    PnlRng* rng = pnl_rng_create(0);
    BlackScholesModel * model = new BlackScholesModel(size, r, rho, sigma, divid, spot);
    MonteCarlo * pricer = new MonteCarlo(model, option, rng, n_samples, polynomDeg);
    
    double prix = pricer->price();
    
    PricingResults res(prix);
    std::cout << res << std::endl;


    pnl_vect_free(&spot);
    pnl_vect_free(&sigma);
    pnl_vect_free(&divid);
    delete P;

    exit(0);
}
