#include "Database.h"


Database::Database(const std::string& connection_string)
    : C(std::make_unique<pqxx::connection>(connection_string))
{
    if (C->is_open()) {
        std::cout << "[SYSTEM] Pomyslnie polaczono z baza danych: " << C->dbname() << "\n";
    }
}


void Database::wyswietlSeanse(std::string filtr) {
    std::lock_guard<std::mutex> lock(dbMutex);
    try {
        pqxx::work W(*C);
        pqxx::result r = W.exec("SELECT * FROM seanse WHERE dzien_godzina >= " + W.quote(filtr));

        for (auto const& row : r) {
            std::cout << row[0].c_str() << " | " << row[1].c_str() << std::endl;
        }
        W.commit();
    }
    catch (const std::exception& e) {
        std::cerr << "Blad wyswietlania seansow: " << e.what() << std::endl;
    }
}