#include "AdminUI.h"

std::map<int, std::string> AdminUI::nazwy_tabel;


void AdminUI::pobierzNazwyTabel(Database& db) {
    nazwy_tabel.clear();
    int licznik = 1; 

    try {
        std::lock_guard<std::mutex> lock(db.dbMutex);
        pqxx::work W(*(db.C));

        std::string sql = "SELECT tablename FROM pg_catalog.pg_tables "
            "WHERE schemaname != 'pg_catalog' AND schemaname != 'information_schema'";

        pqxx::result r = W.exec(sql);

        for (auto const& row : r) {
            nazwy_tabel[licznik] = row[0].c_str();
            licznik++;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Blad pobierania nazw tabel: " << e.what() << std::endl;
    }
}

void AdminUI::wyswietlDostepneTabele() {
    std::cout << "\n--- DOSTEPNE TABELE ---\n";

    for (auto it = AdminUI::nazwy_tabel.begin(); it != AdminUI::nazwy_tabel.end(); ++it){
        std::cout << it->first << ". " << it->second << "\n";
    }
}

void AdminUI::menuAdmina(Database& db) {
    pobierzNazwyTabel(db);

    int nrTabeli = -1;
    int operacja = -1;

    while (true) {
        wyswietlDostepneTabele();
        std::cout << "0. WYJDZ (Powrot do menu glownego)\n";
        std::cout << "Wybierz numer tabeli, na ktorej chcesz pracowac: ";

        if (!(std::cin >> nrTabeli)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }

        if (nrTabeli == 0) break; 

        if (nazwy_tabel.find(nrTabeli) == nazwy_tabel.end()) {
            std::cout << "Bledny numer tabeli! Sprobuj ponownie.\n";
            continue;
        }

        std::string wybranaTabela = nazwy_tabel[nrTabeli];

        while (true) {
            std::cout << "\n--- OPERACJE NA TABELI: " << wybranaTabela << " ---\n";
            std::cout << "1. Wyswietl zawartosc\n";
            std::cout << "2. Dodaj rekord\n";
            std::cout << "3. Modyfikuj rekord (po ID)\n";
            std::cout << "4. Usun rekord\n";
            std::cout << "0. Powrot do listy tabel\n";
            std::cout << "Wybor: ";

            if (!(std::cin >> operacja)) {
                std::cin.clear();
                std::cin.ignore(10000, '\n');
                continue;
            }

            if (operacja == 0) break; 
            wyswietlZawartoscTabeli(db, wybranaTabela);
            switch (operacja) {
            case 1:
                std::cout << "Funkcja wyswietlania w budowie...\n";
                break;
            case 2:
                dodajRekord(db, wybranaTabela);
                break;
            case 3: {
                int id;
                std::cout << "Podaj ID rekordu do modyfikacji: ";
                std::cin >> id;
                modyfikujRekord(db, wybranaTabela, id);
                break;
            }
            case 4:
                std::cout << "Funkcja usuwania w budowie...\n";
                break;
            default:
                std::cout << "Niepoprawny numer operacji!\n";
            }
        }
    }
}
void AdminUI::wyswietlZawartoscTabeli(Database& db, std::string tabela) {
    int limit = 10;
    int offset = 0;
    char akcja;

    while (true) {
        pqxx::result r;

        try {
            std::lock_guard<std::mutex> lock(db.dbMutex);
            pqxx::work W(*(db.C));

            std::string sql = "SELECT * FROM " + tabela +
                " LIMIT " + std::to_string(limit) +
                " OFFSET " + std::to_string(offset);

            r = W.exec(sql);
        }
        catch (const std::exception& e) {
            std::cerr << "Blad odczytu z bazy: " << e.what() << std::endl;
            return;
        }

        std::cout << "\n--- TABELA: " << tabela << " | Wiersze: "
            << offset + 1 << " - " << offset + r.size() << " ---\n";

        if (r.empty()) {
            std::cout << "(Brak rekordow do wyswietlenia na tej stronie)\n";
        }
        else {
            for (size_t col = 0; col < r.columns(); ++col) {
                std::cout << r.column_name(col) << "\t| ";
            }
            std::cout << "\n----------------------------------------------------\n";

            for (auto const& row : r) {
                for (auto const& field : row) {
                    std::cout << field.c_str() << "\t| ";
                }
                std::cout << "\n";
            }
        }

        std::cout << "\n[N] Nastepna | [P] Poprzednia | [0] Wyjdz\nWybor: ";
        std::cin >> akcja;
        akcja = toupper(akcja);

        if (akcja == '0') {
            break;
        }
        else if (akcja == 'N') {
            if (r.size() == limit) {
                offset += limit;
            }
            else {
                std::cout << "To juz jest ostatnia strona!\n";
            }
        }
        else if (akcja == 'P') {
            if (offset >= limit) {
                offset -= limit;
            }
            else {
                std::cout << "To jest pierwsza strona!\n";
            }
        }
        else {
            std::cout << "Nieznana komenda!\n";
        }
    }
}


void AdminUI::modyfikujRekord(Database& db, std::string tabela, int id_rekordu) {
    std::vector<std::string> kolumny;

    try {
        std::lock_guard<std::mutex> lock(db.dbMutex);
        pqxx::work W(*(db.C));

        pqxx::result r = W.exec("SELECT * FROM " + tabela + " LIMIT 0");
        for (size_t i = 1; i < r.columns(); ++i) {
            kolumny.push_back(r.column_name(i));
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Blad dostepu do kolumn: " << e.what() << std::endl;
        return;
    }

    int wybor = -1;
    while (true) {
        std::cout << "\n--- EDYCJA: " << tabela << " [ID: " << id_rekordu << "] ---\n";
        for (size_t i = 0; i < kolumny.size(); ++i) {
            std::cout << i + 1 << ". Zmien " << kolumny[i] << "\n";
        }
        std::cout << "0. WYJDZ\nTwoj wybor: ";

        if (!(std::cin >> wybor)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }
        if (wybor == 0) return;
        if (wybor > 0 && wybor <= kolumny.size()) break;
        std::cout << "Niepoprawny numer!\n";
    }

    std::string wybranaKolumna = kolumny[wybor - 1];
    std::string nowaWartosc;
    std::cout << "Podaj nowa wartosc dla [" << wybranaKolumna << "]: ";
    std::cin.ignore(10000, '\n');
    std::getline(std::cin, nowaWartosc);
    nowaWartosc = trim(nowaWartosc);

    try {
        std::lock_guard<std::mutex> lock(db.dbMutex);
        pqxx::work W(*(db.C));

        // SQL Injection protection via quote()
        std::string sql = "UPDATE " + tabela +
            " SET " + wybranaKolumna + " = " + W.quote(nowaWartosc) +
            " WHERE id = " + std::to_string(id_rekordu);

        W.exec(sql);
        W.commit();
        std::cout << "Zaktualizowano pomyslnie!\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Blad zapisu: " << e.what() << std::endl;
    }
}

void AdminUI::usunRekord(Database& db)
{
}

