#include <iostream>
#include <iomanip>
#include <cmath>
#include <thread>
#include <vector>
#include <chrono>
#include <string>



//multihilo

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

//secuencial

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


//main

using namespace std;

int main() {
    long double x = 1500000.0;  // Valor para calcular el logaritmo
    string inputX;

    do {
        std::cout << "Ingrese el operando del logaritmo natural. Presione Enter para usar el valor predeterminado " << std::setprecision(15) << x << ":";
        std::getline(std::cin, inputX);
        if (!inputX.empty()) { // si no es enter 
            x = std::stold(inputX); // asigna el input a x
        }
        //validacion
        if (x < 1500000.0) {
            std::cerr << "Error: El valor debe ser mayor o igual a 1500000.\n";
        }
    } while (x < 1500000.0); 
    
    std::cout << "Sin Hilos:" << endl;
    //Sin hilos
    auto start_time1 = chrono::high_resolution_clock::now(); //empieza el clock
    
    sin_multihilo(x);

    auto end_time1 = chrono::high_resolution_clock::now();
    auto duration1 = chrono::duration_cast<chrono::milliseconds>(end_time1 - start_time1);

    cout << " - Tiempo de ejecución: " << duration1.count() << " ms\n" << endl;

    //Con hilos
    cout << "Con Hilos:" << endl;
    int num_threads;
    std::cout << "Ingrese el número de hilos: ";
    std::cin >> num_threads;

    auto start_time2 = chrono::high_resolution_clock::now(); //empieza el clock
    
    multihilo(x, num_threads);

    auto end_time2 = chrono::high_resolution_clock::now();
    auto duration2 = chrono::duration_cast<chrono::milliseconds>(end_time2 - start_time2);

    cout << " - Tiempo de ejecución: " << duration2.count() << " ms\n" << endl;
    

    double speed_up = ((double) duration1.count()) / duration2.count();
    cout << "Speed up: " << speed_up << endl;

    return 0;
}
