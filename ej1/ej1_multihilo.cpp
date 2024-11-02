#include <iostream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <vector>

void calculate_partial_sum(long double z, int start, int end, long double& result) {
    long double partial_sum = 0.0;

    for (int n = start; n < end; ++n) {
        partial_sum += (1.0 / (2 * n + 1)) * std::pow(z, 2 * n + 1);
    }

    result = partial_sum * 2;  // Ajuste final según la fórmula
}

void multihilo(long double x, int num_threads) {
    
    int num_terms = 10000000;
    long double z = (x - 1) / (x + 1);  // (x-1)/(x+1)
    std::vector<std::thread> threads;
    std::vector<long double> partial_sums(num_threads);

    int terms_per_thread = num_terms / num_threads;
    //se ejecuta una vez por cada hilo que se va a crear
    for (int i = 0; i < num_threads; ++i) {
        int start = i * terms_per_thread; //índice del primer término de la serie que el hilo debe calcular.
        //terms_per_thread es el número de términos que se le asignan a cada hilo.
        int end = (i == num_threads - 1) ? num_terms : (i + 1) * terms_per_thread;
        //Si el hilo es el último (i == num_threads - 1), entonces end se establece en num_terms, 
        //asegurando que cubra todos los términos restantes.
        //Para los otros hilos, end se calcula como (i + 1) * terms_per_thread.
        threads.push_back(std::thread(calculate_partial_sum, z, start, end, std::ref(partial_sums[i])));
        //va agregando los hilos al vector y calculando las sumas parciales
    }

    for (auto& t : threads) {
        t.join();
    }

    long double log_approx = 0.0;
    //itera sobre todas las sumas parciales almacenadas en partial_sums y obtiene el rdo final
    for (const auto& sum : partial_sums) {
        log_approx += sum;
    }

    std::cout << "Logaritmo natural aproximado: " << std::setprecision(15) << log_approx << std::endl;


}
