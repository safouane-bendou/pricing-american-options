#pragma once
#include "Option.hpp"


class PerformanceOption : public Option
{
  public:
    double strike_; // Prix d'exercice de l'option
    PnlVect* payoffCoefficientsVector_; // vecteur contenant les coefficients du payoff de l'option


    // Constructeur de la classe Basket Option prenant 5 arguments
    PerformanceOption(double T, int dates, int size, double strike, PnlVect * payoffCoefficientsVector);


    // Fonction calculant le payoff de l'option 
    double payoff(const PnlMat* path);

    // Fonction calculant le payoff de l'option à l'instant t
    double payoff(const PnlVect* pricesAt);
    

  // Destructeur de la classe PerformanceOption
  ~PerformanceOption();
};