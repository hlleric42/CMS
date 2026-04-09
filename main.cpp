#define WIN32_LEAN_AND_MEAN  
#define NOMINMAX            
#include <Windows.h>
#include <pqxx/pqxx>        
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <ctime>
#include <iomanip>

#include "Database.h" 
#include "tools.h"

using namespace std;

void menuUzytkownika(Database& db) {
    time_t t = time(nullptr);
    tm teraz_obj;

    if (localtime_s(&teraz_obj, &t) != 0) {
        cerr << "Blad pobierania czasu systemowego!" << endl;
        return;
    }

    tm* teraz = &teraz_obj;

    int opcja;
    cout << "\n--- MENU UZYTKOWNIKA ---" << endl;
    cout << "1. Wyswietl seanse na dany dzien" << endl;
    cout << "0. Powrot" << endl;
    cout << "Wybor: ";
    cin >> opcja;
    cin.ignore();

    if (opcja == 1) {
        string dataIn, godzinaIn;
        cout << "Podaj date (np. 14.04, 14 04 2026 lub Enter dla dzisiaj): ";
        getline(cin, dataIn);


        cout << "Podaj godzine (np. 19:30 lub Enter dla zaokraglonej): ";
        getline(cin, godzinaIn);

        string dataSQL = formatujDate(dataIn, teraz);
        string godzinaSQL = formatujGodzine(godzinaIn, teraz);

#ifdef _DEBUG
        cout << "[DEBUG] Parametry: " << dataSQL << " " << godzinaSQL << endl;
#endif

        db.wyswietlSeanse(dataSQL + " " + godzinaSQL);
    }
}

void menuAdmina(Database& db) {
    int opcja;
    cout << "\n--- PANEL ADMINISTRATORA ---" << endl;
    cout << "1. Dodaj film" << endl;
    cout << "2. Dodaj sale" << endl;
    cout << "0. Powrot" << endl;
    cout << "Wybor: ";
    cin >> opcja;
    cin.ignore();

    if (opcja == 1) {
        string tytul, gatunek;
        int czas;
        cout << "Tytul: "; getline(cin, tytul);
        cout << "Gatunek: "; getline(cin, gatunek);
        cout << "Czas trwania (min): "; cin >> czas;
    }
}


int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    try {
        Database db("host=localhost dbname=kino_db user=postgres password=root");

        int rola = -1;
        while (rola != 0) {
            cout << "\n============================" << endl;
            cout << "  SYSTEM ZARZADZANIA KINEM  " << endl;
            cout << "============================" << endl;
            cout << "1. KLIENT\n2. ADMINISTRATOR\n0. WYJSCIE\nWybor: ";
            if (!(cin >> rola)) break;

            if (rola == 1) menuUzytkownika(db);
            else if (rola == 2) menuAdmina(db);
        }
    }
    catch (const exception& e) {
        cerr << "BLAD KRYTYCZNY: " << e.what() << endl;
        return 1;
    }

    return 0;
}