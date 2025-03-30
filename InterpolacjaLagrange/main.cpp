#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>

using namespace std;

bool wczytajDane(const string& nazwaPliku, vector<float>& x, vector<float>& y) {
    fstream file(nazwaPliku);
    if (!file.is_open()) {
        cerr << "Blad otwarcia pliku";
        return false;
    }

    string linia;
    vector<string> tab;
    while (getline(file, linia)) {
        tab.push_back(linia);
    }
    file.close();
    
    if (tab.size() < 3) {
        cerr << "Nieprawidlowy format pliku";
        return false;
    }
    
    int pos = 0;
    tab[1].erase(0, 5); 
    while ((pos = tab[1].find('\t')) != string::npos) {
        x.push_back(stof(tab[1].substr(0, pos)));
        tab[1].erase(0, pos + 1);
    }
    x.push_back(stof(tab[1]));
    
    pos = 0;
    tab[2].erase(0, 7);
    while ((pos = tab[2].find('\t')) != string::npos) {
        y.push_back(stof(tab[2].substr(0, pos)));
        tab[2].erase(0, pos + 1);
    }
    y.push_back(stof(tab[2]));
    
    return true;
}

float lagrangeInterpolation(float xp, const vector<float>& x, const vector<float>& y)
{
    int n = x.size();
    float result = 0;
    for (int i = 0; i < n; i++) //petla wezlow glownych
    {
        float tmp = y[i];
        for (int j = 0; j < n; j++) //petla pozostalych wezlow
        {
            if (i != j)
                tmp *= (xp - x[j]) / (x[i] - x[j]);
        }
        result += tmp;
    }
    return result;
}

float obliczSredniBladKwadratowy(const vector<float>& x, const vector<float>& y,
     const vector<float>& x_wezly, const vector<float>& y_wezly)
{
    float sumaBledow = 0;
    int liczbaPunktow = 0;
    
    for (size_t i = 0; i < x.size(); i++)
    {
        // Jeśli x[i] nie jest wezłem, obliczamy błąd
        if (i%5!=0)
        {
            float yInterpolowane = lagrangeInterpolation(x[i], x_wezly, y_wezly);
            sumaBledow += pow(y[i] - yInterpolowane, 2);
            liczbaPunktow++;
        }
    }

    return (liczbaPunktow > 0) ? sumaBledow / liczbaPunktow : 0;
}


int main() {
    vector<float> x, y, x_wezly, y_wezly;
    string nazwaPliku = "dane.txt";
    
    if (!wczytajDane(nazwaPliku, x, y)) {
        return 1;
    }
    
    for (size_t i = 0; i < x.size(); i += 5) {
        x_wezly.push_back(x[i]);
        y_wezly.push_back(y[i]);
    }
    
    float xp;
    cout << "Podaj punkt interpolacji: ";
    cin >> xp;
    
    float wynik = lagrangeInterpolation(xp, x_wezly, y_wezly);
    cout << "Wartosc wielomianu interpolacyjnego w punkcie " << xp << " wynosi: " << wynik << endl;
    
    ofstream outFile("interpolacja.csv");
    if (!outFile) {
        cerr << "Blad zapisu do pliku";
        return 1;
    }
    
    outFile << "x,y" << endl;
    
    for (const float iks:x)
    {
        outFile << iks << "," << lagrangeInterpolation(iks, x_wezly, y_wezly) << endl;
    }
    
    outFile.close();
    cout << "Dane interpolacji zapisane do pliku interpolacja.csv" << endl;
    
    float sredniBladKwadratowy = obliczSredniBladKwadratowy(x, y, x_wezly, y_wezly);
    cout << "Sredni blad kwadratowy: " << sredniBladKwadratowy << endl;

    return 0;
}

/* ZMODYFIKOWANY MAIN DO OSTATNIEGO PODPUNKTU

int main() {
    vector<float> x, y, x_wezly, y_wezly;
    string nazwaPliku = "dane.txt";
    
    if (!wczytajDane(nazwaPliku, x, y)) {
        return 1;
    }
    
    cout<<"ilosc danych "<<x.size()<<endl;
    // Przechodzimy przez różną liczbę punktów węzłowych i obliczamy błąd
    float najmniejszyBlad = FLT_MAX; // FLT_MAX to największa możliwa liczba zmiennoprzecinkowa
    int najlepszaLiczbaWezlow = 0;

    // Testujemy różną liczbę punktów węzłowych
    for (int nWezly = 1; nWezly <= x.size()/5; ++nWezly) {
        x_wezly.clear();
        y_wezly.clear();
        
        // Wybieramy nWezly punktów rozłożonych równomiernie na wektorze x
        for (int i = 0; i <= nWezly*5; i+=5) {
            x_wezly.push_back(x[i]);
            y_wezly.push_back(y[i]);
        }
        // Obliczamy błąd średniokwadratowy dla danej liczby węzłów
        float sredniBladKwadratowy = obliczSredniBladKwadratowy(x, y, x_wezly, y_wezly);
        cout<<"blad dla "<<nWezly<<" wezlow wynosi: "<<sredniBladKwadratowy<<endl;
        if (sredniBladKwadratowy < najmniejszyBlad) {
            najmniejszyBlad = sredniBladKwadratowy;
            najlepszaLiczbaWezlow = nWezly;
        }
    }
    cout << "Najmniejszy blad sredniokwadratowy uzyskano dla " << najlepszaLiczbaWezlow << " wezlow." << endl;
    cout << "Najmniejszy blad: " << najmniejszyBlad << endl;

    return 0;
}
*/
