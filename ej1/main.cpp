#include "ej1_multihilo.cpp"
#include "ej1_sin_multihilos.cpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <vector>
#include <string>

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
