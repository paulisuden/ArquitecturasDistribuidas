#include <iostream>
#include <iomanip>
#include <cmath>
#include <mpi.h>
#include <vector>
#include <chrono>
#include <string>

void calculate_partial_sum(long double z, int start, int end, long double& result) {
    long double partial_sum = 0.0;

    for (int n = start; n < end; ++n) {
        partial_sum += (1.0 / (2 * n + 1)) * std::pow(z, 2 * n + 1);
    }

    result = partial_sum * 2;  
}

int main() {

    int rank, size;

    if (MPI_Init(NULL, NULL) != MPI_SUCCESS) {
        std::cerr << "Error iniciando MPI" << std::endl;
        return 1;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    long double x = 1500000.0;  
    std::string inputX;

    do {
        std::cout << "Ingrese el operando del logaritmo natural (mayor o igual a 1500000). "
                << "Presione Enter para usar el valor predeterminado " << std::setprecision(15) << x << ": ";
        std::getline(std::cin, inputX);
        

        if (!inputX.empty()) { 
            try {
                x = std::stold(inputX); 
            } catch (const std::invalid_argument&) {
                std::cerr << "Entrada no válida. Intente de nuevo." << std::endl;
                continue; 
            }
        }

        if (x < 1500000.0) {
            std::cerr << "Error: El valor debe ser mayor o igual a 1500000.\n";
        }
        
    } while (x < 1500000.0); 

    std::cout << "Valor válido ingresado: " << x << std::endl;

    int num_terminos = 10000000;
    long double z = (x - 1) / (x + 1);  

    int terminos_por_proceso = num_terminos / size;
    int start = rank * terminos_por_proceso;
    int end = (rank == size - 1) ? num_terminos : (rank + 1) * terminos_por_proceso;

    long double partial_sum = 0.0;

    auto start_time = std::chrono::high_resolution_clock::now();
    calculate_partial_sum(z, start, end, partial_sum);
    auto end_time = std::chrono::high_resolution_clock::now();

    /* Cada proceso calcula una suma parcial de la serie, y MPI_Reduce 
    las suma todas en el proceso raíz (rank == 0) */

    long double total_sum;
    MPI_Reduce(&partial_sum, &total_sum, 1, MPI_LONG_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);


    //solo el proceso raíz tiene la suma total de la reducción (total_sum)
    if (rank == 0) {
        long double log_approx = total_sum;
        std::cout << "Logaritmo natural aproximado: " << std::setprecision(15) << log_approx << std::endl;

        auto total_end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(total_end_time - start_time).count();
        std::cout << "Tiempo de ejecución (ms): " << duration << std::endl;

    }

    if (MPI_Finalize() != MPI_SUCCESS) {
        std::cerr << "Error finalizando MPI" << std::endl;
        return 1;
    }

    return 0;
}
