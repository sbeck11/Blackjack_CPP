#include "headers/statistics.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

//////////////* Default Constructor *////

PlayerSet::PlayerSet() : name("N/A"), cash(1000), wins(0), loses(0) {}

//////////////* Getter Functions *////

// Returns name of Player
std::string PlayerSet::getName(){
    return name;
}

// Returns cash of Player
int PlayerSet::getCash(){
    return cash;
}

// Returns wins of Player
int PlayerSet::getWins(){
    return wins;
}

// Returns loses of Player
int PlayerSet::getLoses(){
    return loses;
}

//////////////* Setter Function *////

void PlayerSet::setValues(std::string nm, int c, int w, int l){
    name = nm;
    cash = c;
    wins = w;
    loses = l;
}

//////////////////////////////////////////////////////////////////


//////////////* Default Constructor *////

Statistics::Statistics(){
    std::fstream temp;
    temp.open("data/statistics.bin", std::ios::in | std::ios::binary);
    if(temp.fail()){
        saveStats();
    }
    else{
        temp.close();
        loadStats();
    }
}

//////////////* Checks for High Score & Saves *////

bool Statistics::check(Player pl){
    bool rewrite = false;
    if(pl.getCash()>p[0].getCash()){
        p[0].setValues(pl.getName(), pl.getCash(), pl.getWins(), pl.getLoses());
        rewrite = true;
    }
    if(pl.getWins()>p[1].getWins()){
        p[1].setValues(pl.getName(), pl.getCash(), pl.getWins(), pl.getLoses());
        rewrite = true;
    }
    if(pl.getLoses()>p[2].getLoses()){
        p[2].setValues(pl.getName(), pl.getCash(), pl.getWins(), pl.getLoses());
        rewrite = true;
    }
    if(rewrite){
        saveStats();
    }
    return rewrite;
}

//////////////* Printing *////

void Statistics::print() {
    int maxlength = std::max({p[0].getName().length(), p[1].getName().length(), p[2].getName().length()});
    for (int i = 0; i < 3; i++) {
        switch (i) {
            case 0: std::cout << "MAX CASH  ||||||||| "; break;
            case 1: std::cout << "MAX WINS  ||||||||| "; break;
            case 2: std::cout << "MAX LOSES ||||||||| "; break;
            default: std::cerr << "Unexpected index: " << i << std::endl; break;  
        }
        std::cout << std::setw(maxlength + 1) << p[i].getName() << "\t | \t" << "Cash: " << std::setw(7) << p[i].getCash() << "\t | \t" << "Wins: " << std::setw(5) << p[i].getWins() << "\t | \t" << "Loses: " << std::setw(5) << p[i].getLoses() << "\n";
    }
}

//////////////* File Handling *////

void Statistics::saveStats() {
    std::fstream f1;
    f1.open("data/statistics.bin", std::ios::out | std::ios::binary);
    for (int i = 0; i < 3; i++) {
        std::string sName = p[i].getName();
        int nameSize = sName.size();
        int sCash = p[i].getCash();
        int sWins = p[i].getWins();
        int sLoses = p[i].getLoses();
        f1.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
        f1.write(sName.c_str(), static_cast<std::streamsize>(sName.size())); 
        f1.write(reinterpret_cast<const char*>(&sCash), sizeof(sCash));
        f1.write(reinterpret_cast<const char*>(&sWins), sizeof(sWins));
        f1.write(reinterpret_cast<const char*>(&sLoses), sizeof(sLoses));
    }
    f1.close();
}

void Statistics::loadStats() {
    std::fstream f1;
    f1.open("data/statistics.bin", std::ios::in | std::ios::binary);
    if (!f1.is_open()) {
        std::cerr << "Failed to open file for reading." << std::endl;
        return;
    }

    for (int i = 0; i < 3; i++) {
        std::string sName;
        int nameSize;
        int sCash;
        int sWins;
        int sLoses;

        f1.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));

        if (nameSize < 0) {
            std::cerr << "Invalid size read from file; must be non-negative." << std::endl;
            continue;  
        }

        sName.resize(static_cast<std::string::size_type>(nameSize)); 

        f1.read(&sName[0], static_cast<std::streamsize>(sName.size()));  
        f1.read(reinterpret_cast<char*>(&sCash), sizeof(sCash));
        f1.read(reinterpret_cast<char*>(&sWins), sizeof(sWins));
        f1.read(reinterpret_cast<char*>(&sLoses), sizeof(sLoses));

        p[i].setValues(sName, sCash, sWins, sLoses);
    }
    f1.close();
}