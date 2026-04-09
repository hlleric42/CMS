#pragma once
#include <iostream>
#include <vector>
#include "tools.h"
#include <mutex>      
#include <pqxx/pqxx>  
#include <string>
#include "Database.h"

class AdminUI
{
public:
    static std::map<int, std::string> nazwy_tabel;
    static void pobierzNazwyTabel(Database& db);
    static void wyswietlDostepneTabele();
    static void menuAdmina(Database& db);
    static void wyswietlZawartoscTabeli(Database& db, std::string tabela);

    static void modyfikujRekord(Database& db, std::string tabela, int id_rekordu);
    static void usunRekord(Database& db);
    static void dodajRekord(Database& db, std::string tabela);
};