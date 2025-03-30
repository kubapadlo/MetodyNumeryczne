#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <math.h>

using namespace std;

// Funkcja obliczająca wartość wielomianu w postaci naturalnej
double metodanaturalna(const vector<double>& wyznaczniki, double x) {
    double result = 0.0;
    int n = wyznaczniki.size();
    for (int i = 0; i < n; ++i) {
        result += wyznaczniki[i] * pow(x, i);
    }
    return result;
}

// Funkcja obliczająca wartość wielomianu metodą Hornera
double schematHornera(const vector<double>& wyznaczniki, double x) {
    double result = 0.0;
    int n = wyznaczniki.size();
    for (int i = n - 1; i >= 0; --i) {
        result = result * x + wyznaczniki[i];
    }
    return result;
}

int main() {
    ifstream inputFile("daneHorner.txt"); // Nazwa pliku wejściowego
    if (!inputFile) {
        cerr << "Nie można otworzyć pliku!" << endl;
        return 1;
    }

    ofstream outputFile("wyniki.csv"); // Plik do eksportu danych
    if (!outputFile) {
        cerr << "Nie można otworzyć pliku wyjściowego!" << endl;
        return 1;
    }

    outputFile << "Liczba argumentów, Czas naturalna (ns), Czas Horner (ns)\n";

    vector<double> wyznaczniki;
    vector<double> x_values;
    string line;

    // Wczytywanie współczynników
    getline(inputFile, line);
    istringstream coefStream(line.substr(3)); // Pominięcie "a:"
    double coef;
    while (coefStream >> coef) {
        wyznaczniki.push_back(coef);
    }

    // Wczytywanie wartości x
    getline(inputFile, line);
    istringstream xStream(line.substr(3)); // Pominięcie "xi:"
    double x;
    while (xStream >> x) {
        x_values.push_back(x);
    }
    inputFile.close();

    cout << fixed << setprecision(6);

    int curr_n = 10; // Liczba wyznaczników
    vector<double> current_wyznaczniki(wyznaczniki.begin(), wyznaczniki.begin() + curr_n);
    cout << "ilosc wyznacznikow: " << current_wyznaczniki.size() << endl;

    for (int i = 1; i < x_values.size(); i += 5) {
        vector<double> curr_args(x_values.begin(), x_values.begin() + i);
        cout << "ilosc argumentow: " << i << endl;

        auto start1 = chrono::high_resolution_clock::now();
        for (const double x : curr_args) {
            double naturalResult = metodanaturalna(current_wyznaczniki, x);
        }
        auto end1 = chrono::high_resolution_clock::now();
        auto microseconds1 = chrono::duration_cast<chrono::nanoseconds>(end1 - start1);

        auto start2 = chrono::high_resolution_clock::now();
        for (const double x : curr_args) {
            double hornerResult = schematHornera(current_wyznaczniki, x);
        }
        auto end2 = chrono::high_resolution_clock::now();
        auto microseconds2 = chrono::duration_cast<chrono::nanoseconds>(end2 - start2);

        cout << "czas dla naturalnej metody (ns): " << microseconds1.count() << endl;
        cout << "czas dla hornera (ns): " << microseconds2.count() << endl;

        // Zapis wyników do pliku CSV
        outputFile << i << "," << microseconds1.count() << "," << microseconds2.count() << "\n";
    }

    cout << "Dane zostały zapisane do pliku wyniki.csv" << endl;
    outputFile.close();

    return 0;
}
