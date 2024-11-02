#include <iostream>
#include <iomanip>  // para std::setprecision
#include <cmath>    // para std::pow

void sin_multihilo(long double x) {
    
    int num_terms = 10000000;  // 10 millones de términos de la serie
    long double log_approx = 0.0;
    long double z = (x - 1) / (x + 1);  // (x-1)/(x+1)

    // calculo de la serie de taylor
    for (int n = 0; n < num_terms; ++n) {
        log_approx += (1.0 / (2 * n + 1)) * std::pow(z, 2 * n + 1);
    }
    log_approx *= 2;  
    std::cout << "Logaritmo natural aproximado: " << std::setprecision(15) << log_approx << std::endl;
}
