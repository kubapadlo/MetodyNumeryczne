#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

using namespace std;

// Funkcja do wczytania danych z pliku
bool wczytajDane(const string& nazwaPliku, vector<double>& xWartosci, vector<double>& yWartosci) {
    ifstream plik(nazwaPliku);
    if (!plik.is_open()) {
        cerr << "Blad otwarcia pliku\n";
        return false;
    }

    string linia;
    vector<string> wiersze;
    while (getline(plik, linia)) {
        wiersze.push_back(linia);
    }
    plik.close();
    
    if (wiersze.size() < 3) {
        cerr << "Nieprawidlowy format pliku\n";
        return false;
    }

    int pozycja = 0;
    wiersze[1].erase(0, 5); 
    while ((pozycja = wiersze[1].find('\t')) != string::npos) {
        xWartosci.push_back(stod(wiersze[1].substr(0, pozycja)));
        wiersze[1].erase(0, pozycja + 1);
    }
    xWartosci.push_back(stod(wiersze[1]));

    pozycja = 0;
    wiersze[2].erase(0, 7);
    while ((pozycja = wiersze[2].find('\t')) != string::npos) {
        yWartosci.push_back(stod(wiersze[2].substr(0, pozycja)));
        wiersze[2].erase(0, pozycja + 1);
    }
    yWartosci.push_back(stod(wiersze[2]));

    return true;
}

// Obliczanie współczynników interpolacji Newtona
vector<double> obliczWspolczynnikiNewtona(const vector<double>& x, const vector<double>& y) {
    int n = x.size();
    vector<vector<double>> podzieloneRoznice(n, vector<double>(n, 0.0));
    
    for (int i = 0; i < n; i++) {
        podzieloneRoznice[i][0] = y[i];
    }
    
    for (int j = 1; j < n; j++) {
        for (int i = 0; i < n - j; i++) {
            podzieloneRoznice[i][j] = (podzieloneRoznice[i + 1][j - 1] - podzieloneRoznice[i][j - 1]) / (x[i + j] - x[i]);
        }
    }
    
    vector<double> wspolczynniki(n);
    for (int i = 0; i < n; i++) {
        wspolczynniki[i] = podzieloneRoznice[0][i];
    }
    return wspolczynniki;
}

// Obliczanie wartości wielomianu Newtona
double obliczWartoscWielomianu(const vector<double>& x, const vector<double>& wspolczynniki, double wartoscX) {
    double wynik = wspolczynniki.back();
    for (int i = wspolczynniki.size() - 2; i >= 0; --i) {
        wynik = wynik * (wartoscX - x[i]) + wspolczynniki[i];
    }
    return wynik;
}

// Testowanie dokładności interpolacji
void testujDokladnoscInterpolacji(const vector<double>& xPelne, const vector<double>& yPelne) {
    int maksPunkty = xPelne.size();
    double najlepszyBlad = 1e9;
    int najlepszaIloscPunktow = 0;

    cout << "Liczba punktow | Sredni blad interpolacji" << endl;
    cout << "----------------------------------------" << endl;

    for (int n = 2; n <= maksPunkty; n++) {
        vector<double> xWezly, yWezly;
        for (int i = 0; i < n; i += 5) {
            if (i < xPelne.size()) {
                xWezly.push_back(xPelne[i]);
                yWezly.push_back(yPelne[i]);
            }
        }

        if (xWezly.size() < 2) continue;

        vector<double> wspolczynniki = obliczWspolczynnikiNewtona(xWezly, yWezly);

        double sumaBledow = 0.0;
        for (size_t i = 0; i < xPelne.size(); i++) {
            double interpolowana = obliczWartoscWielomianu(xWezly, wspolczynniki, xPelne[i]);
            sumaBledow += abs(interpolowana - yPelne[i]);
        }
        double sredniBlad = sumaBledow / xPelne.size();

        cout << n << "              | " << sredniBlad << endl;

        if (sredniBlad < najlepszyBlad) {
            najlepszyBlad = sredniBlad;
            najlepszaIloscPunktow = n;
        }
    }

    cout << "\nNajlepsza dokladnosc osiagnieta dla " << najlepszaIloscPunktow << " punktow wezlowych." << endl;
}

int main()
{
    vector<double> xWartosci, yWartosci, xWezly, yWezly;

    if (!wczytajDane("daneNewton.txt", xWartosci, yWartosci)) {
        return 1;
    }

    for (int i = 0; i < xWartosci.size(); i += 5) 
    {
        xWezly.push_back(xWartosci[i]);
        yWezly.push_back(yWartosci[i]);
    }

    vector<double> wspolczynniki = obliczWspolczynnikiNewtona(xWezly, yWezly);

    double liczba;
    cout << "Podaj liczbe dla ktorej mamy policzyc wartosc wielomianu: ";
    cin >> liczba;
    cout << endl;
    
    cout << "P(" << liczba << ") = " << obliczWartoscWielomianu(xWezly, wspolczynniki, liczba)  << endl;

    ofstream plikWyjsciowy("interpolacja.csv");
    if (!plikWyjsciowy) {
        cerr << "Blad zapisu do pliku";
        return 1;
    }
    
    plikWyjsciowy << "x,y" << endl;
    
    for(int i = 0; i < xWartosci.size(); ++i) {
        plikWyjsciowy << xWartosci[i] << "," << obliczWartoscWielomianu(xWezly, wspolczynniki, xWartosci[i]) << endl;
    }
    
    plikWyjsciowy.close();
    cout << "Dane interpolacji zapisane do pliku interpolacja.csv" << endl;

    testujDokladnoscInterpolacji(xWartosci, yWartosci);

    return 0;
}