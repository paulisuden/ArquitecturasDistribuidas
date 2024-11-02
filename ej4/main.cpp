#include <iostream>
#include <vector>
#include <cmath>
#include <pthread.h>
#include <chrono>
#include <algorithm>

using namespace std;

// encapsular los datos que cada hilo necesita para trabajar
struct ThreadData {
    long long int start;
    long long int end;
    vector<long long int>* primos;
    pthread_mutex_t* mutex;
};

//
bool esPrimo(long long int n) {
    if (n <= 1) return false;
    if (n == 2 || n == 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    for (long long int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

//sin multihilos para encontrar números primos
vector<long long int> encontrarPrimos(long long int N) {
    vector<long long int> primos;
    for (long long int i = 2; i < N; ++i) {
        if (esPrimo(i)) {
            primos.push_back(i);
        }
    }
    return primos;
}

//multihilos
void* encontrarPrimosPorRango(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    vector<long long int> primos_local;

    for (long long int i = data->start; i < data->end; ++i) {
        if (esPrimo(i)) {
            primos_local.push_back(i);
        }
    }

    pthread_mutex_lock(data->mutex);
    data->primos->insert(data->primos->end(), primos_local.begin(), primos_local.end());
    pthread_mutex_unlock(data->mutex);

    pthread_exit(nullptr);
}

// multihilos para encontrar números primos
vector<long long int> encontrarPrimosMultithreaded(long long int N, int num_threads) {
    vector<long long int> primos;
    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  //es un mutex que sincorniza el acceso al vector compartido primos

    //cuantos numeros cada hilo debe analizar
    long long int rango_por_hilo = N / num_threads;

    // crear hilos
    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].start = i * rango_por_hilo + 2; //0 y 1 no son primos
        thread_data[i].end = (i == num_threads - 1) ? N : (i + 1) * rango_por_hilo; //final de nros que analiza el hilo
        thread_data[i].primos = &primos; //puntero para q el hilo inserte los primos encontrados
        thread_data[i].mutex = &mutex; //para que el hilo pueda sincornizar el acceso al vector
        pthread_create(&threads[i], nullptr, encontrarPrimosPorRango, &thread_data[i]); //inicia el hilo
    }

    // esperar que todos los hilos terminen
    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return primos;
}

int main() {
    long long int N;
    int num_threads;
    cout << "Ingrese el valor de N: ";
    cin >> N;
    cout << "Ingrese el número de hilos: ";
    cin >> num_threads;


    //sin multihilos 
    auto start2 = std::chrono::high_resolution_clock::now();
    vector<long long int> primos = encontrarPrimos(N);
    auto end2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration2 = end2 - start2;
    cout << "\nTiempo de ejecución sin multihilos: " << duration2.count() << " segundos" << endl;
    cout << "Cantidad de números primos menores que " << N << ": " << primos.size() << endl;

    // los 10 números primos más grandes
    cout << "Los 10 números primos más grandes menores que " << N << " son:\n";
    for (int i = max(0, (int)primos.size() - 10); i < primos.size(); ++i) {
        cout << primos[i] << " ";
    }
    cout << endl;

    // con multihilos
    auto start = std::chrono::high_resolution_clock::now();
    vector<long long int> primos_multithreaded = encontrarPrimosMultithreaded(N, num_threads);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration1 = end - start;
    cout << "\nTiempo de ejecución con multihilos: " << duration1.count() << " segundos" << endl;
    cout << "Cantidad de números primos menores que " << N << ": " << primos_multithreaded.size() << endl;

    // los 10 números primos más grandes
    sort(primos_multithreaded.begin(), primos_multithreaded.end());
    cout << "Los 10 números primos más grandes menores que " << N << " son:\n";
    for (int i = max(0, (int)primos_multithreaded.size() - 10); i < primos_multithreaded.size(); ++i) {
        cout << primos_multithreaded[i] << " ";
    }
    cout << endl;

    // speedup
    double speedup = duration2 / duration1;
    cout << "Speedup: " << speedup << endl;

    return 0;
}
