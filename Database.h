#pragma once
#include <pqxx/pqxx>
#include <mutex>
#include <memory>
#include <string>
#include <iostream>

class AdminUI;

class Database {
    friend class AdminUI;
private:
    Database(const std::string& connection_string);
    std::unique_ptr<pqxx::connection> C;
    std::mutex dbMutex;
public:
    void wyswietlSeanse(std::string filtr);
};