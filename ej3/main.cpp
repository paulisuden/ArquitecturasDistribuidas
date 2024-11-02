#include <iostream>
#include <vector>
#include <pthread.h>
#include <chrono>

using namespace std;

#define BLOCK_SIZE 64  // Tamaño de bloque para la técnica de tiling

//  encapsular los datos que cada hilo necesita para trabajar
struct ThreadData {
    int thread_id;
    int N;
    int num_threads;
    const vector<vector<float>>* A; //punteros de las matrices de entrada
    const vector<vector<float>>* B;
    vector<vector<float>>* C; // puntero de la matriz de salida
};

// Función para imprimir las esquinas de una matriz NxN
void printCorners(const vector<vector<float>>& matrix, int N) {
    cout << "Esquinas de la matriz:\n";
    cout << matrix[0][0] << " ... " << matrix[0][N-1] << endl;
    cout << "...        ...\n";
    cout << matrix[N-1][0] << " ... " << matrix[N-1][N-1] << endl;
}

// Multiplicación de matrices sin multihilos (con bloqueo)
vector<vector<float>> multiplyMatrices(const vector<vector<float>>& A, const vector<vector<float>>& B, int N) {
    vector<vector<float>> result(N, vector<float>(N, 0.0f));
    for (int ii = 0; ii < N; ii += BLOCK_SIZE) {
        for (int jj = 0; jj < N; jj += BLOCK_SIZE) {
            for (int kk = 0; kk < N; kk += BLOCK_SIZE) {
                // Bloqueando los bucles internos para mejorar la caché
                for (int i = ii; i < min(ii + BLOCK_SIZE, N); ++i) {
                    for (int j = jj; j < min(jj + BLOCK_SIZE, N); ++j) {
                        for (int k = kk; k < min(kk + BLOCK_SIZE, N); ++k) {
                            result[i][j] += A[i][k] * B[k][j];
                        }
                    }
                }
            }
        }
    }
    return result;
}

// void* (un puntero genérico que luego se usa como puntero a thread data)
void* threadMultiply(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    int N = data->N;
    int thread_id = data->thread_id;
    int num_threads = data->num_threads;
    const vector<vector<float>>* A = data->A;
    const vector<vector<float>>* B = data->B;
    vector<vector<float>>* C = data->C;

    int start_row = (thread_id * N) / num_threads;            //son las filas que procesa cada hilo
    int end_row = ((thread_id + 1) * N) / num_threads;       //  

    for (int ii = start_row; ii < end_row; ii += BLOCK_SIZE) {
        for (int jj = 0; jj < N; jj += BLOCK_SIZE) {
            for (int kk = 0; kk < N; kk += BLOCK_SIZE) {
                for (int i = ii; i < min(ii + BLOCK_SIZE, end_row); ++i) {
                    for (int j = jj; j < min(jj + BLOCK_SIZE, N); ++j) {
                        for (int k = kk; k < min(kk + BLOCK_SIZE, N); ++k) {
                            (*C)[i][j] += (*A)[i][k] * (*B)[k][j];
                        }
                    }
                }
            }
        }
    }
    pthread_exit(nullptr); //esta función se utiliza para terminar el hilo de forma segura
}

// multiplicación de matrices con multihilos
vector<vector<float>> multiplyMatricesMultithreaded(const vector<vector<float>>& A, const vector<vector<float>>& B, int N, int num_threads) {
    vector<vector<float>> C(N, vector<float>(N, 0.0f));
    pthread_t threads[num_threads]; //arreglo de hilos
    ThreadData thread_data[num_threads]; //arreglo de datos de hilos

    for (int i = 0; i < num_threads; ++i) {
        thread_data[i] = {i, N, num_threads, &A, &B, &C};
        pthread_create(&threads[i], nullptr, threadMultiply, &thread_data[i]); //se crea cada hilo y se pasa a la función threadMultiply
    }

    // espera a que todos los hilos terminen
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return C;
}

// Sumatoria de todos los elementos de la matriz
float sumMatrix(const vector<vector<float>>& matrix, int N) {
    float sum = 0.0f;
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            sum += matrix[i][j];
        }
    }
    return sum;
}

int main() {
    int N, num_threads;
    cout << "Ingrese el tamaño de las matrices NxN: ";
    cin >> N;
    cout << "Ingrese el número de hilos (10 o 20): ";
    cin >> num_threads;

    // inicializamoa las matrices conn 0.1 y 0.2
    vector<vector<float>> A(N, vector<float>(N, 0.1f));
    vector<vector<float>> B(N, vector<float>(N, 0.2f));

    //esquinas de las matrices iniciales
    cout << "Matriz A:\n";
    printCorners(A, N);
    cout << "\nMatriz B:\n";
    printCorners(B, N);

    // multiplicación con multihilos ---
    auto start = std::chrono::high_resolution_clock::now();
    vector<vector<float>> C_threads = multiplyMatricesMultithreaded(A, B, N, num_threads);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration1 = end - start;
    cout << "\nTiempo de ejecución con multihilos: " << duration1.count() << " segundos" << endl;

    // Imprimir esquinas de la matriz resultante
    cout << "\nMatriz resultante C con multihilos:\n";
    printCorners(C_threads, N);

    // sumatoria de los elementos de la matriz resultante
    float sum_threads = sumMatrix(C_threads, N);
    cout << "\nSumatoria de todos los elementos de la matriz resultante con multihilos: " << sum_threads << endl;

    // multiplicación sin multihilos ---
    auto start2 = std::chrono::high_resolution_clock::now();
    vector<vector<float>> C_no_threads = multiplyMatrices(A, B, N);
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration2 = end2 - start2;
    cout << "\nTiempo de ejecución sin multihilos: " << duration2.count() << " segundos" << endl;


    float sum_no_threads = sumMatrix(C_no_threads, N);
    cout << "\nSumatoria de todos los elementos de la matriz resultante sin multihilos: " << sum_no_threads << endl;


    cout << "\nMatriz resultante:\n";
    printCorners(C_no_threads, N);

    // speedup
    double speedup = duration2 / duration1;
    cout << "\nSpeedup: " << speedup << endl;

    return 0;
}
