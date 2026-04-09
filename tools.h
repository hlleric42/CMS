#ifndef TOOLS_H
#define TOOLS_H

#include <string>
#include <cctype>
#include <vector>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>


inline std::string trim(std::string s) {
    s.erase(s.begin(), 
        std::find_if(s.begin(), s.end(), [](unsigned char ch) {return !std::isspace(ch);}));

    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), 
        s.end());

    return s;
}

inline std::string formatujDate(std::string input, const std::tm* teraz) {
    if (input.length() > 10) input = input.substr(0, 10);
    for (char& c : input) if (!isdigit(c)) c = ' ';

    int d = teraz->tm_mday;
    int m = teraz->tm_mon + 1;
    int r = teraz->tm_year + 1900;

    std::stringstream ss(input);
    std::vector<int> czesci;
    long long n;

    while (ss >> n) {
        if (n > 3000) n = 3000;
        if (n < 0) n = 0;
        czesci.push_back(static_cast<int>(n));
    }

    switch (czesci.size()) {
    case 3: r = czesci[2]; m = czesci[1]; d = czesci[0]; break;
    case 2: m = czesci[1]; d = czesci[0]; break;
    case 1: d = czesci[0]; break;
    default: break;
    }

    std::stringstream wynik;
    wynik << r << "-" << std::setfill('0') << std::setw(2) << m << "-" << std::setw(2) << d;
    return wynik.str();
}

inline std::string formatujGodzine(std::string input, const std::tm* teraz) {
    if (!input.empty()) {
        for (char& c : input) if (!isdigit(c)) c = ' ';
        std::stringstream ss(input);
        int h, min = 0;
        if (ss >> h) {
            if (ss >> min) {
                std::stringstream res;
                res << std::setfill('0') << std::setw(2) << h << ":" << std::setw(2) << min;
                return res.str();
            }
            std::stringstream res;
            res << std::setfill('0') << std::setw(2) << h << ":00";
            return res.str();
        }
    }
    int godzina = teraz->tm_hour;
    if (teraz->tm_min >= 30) godzina++;
    if (godzina >= 24) godzina = 0;

    std::stringstream wynik;
    wynik << std::setfill('0') << std::setw(2) << godzina << ":00";
    return wynik.str();
}

#endif