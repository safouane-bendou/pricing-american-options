#pragma once
#include <iostream>
#include <cmath>

#include "Option.hpp"

using namespace std;


class GeometricOption : public Option
{
  public:
    double strike_; // Prix d'exercice de l'option


    // Constructeur de la classe GeometricOption prenant 5 arguments
    GeometricOption(double T, int dates, int size, double strike);

    // Fonction calculant le payoff de l'option
    double payoff(const PnlMat* path);
    
    //payoff de l'option en t
    double payoff(const PnlVect* pricesAt);

     // Destructeur de la classe GeometricOption
    ~GeometricOption();
};