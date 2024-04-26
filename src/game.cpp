#include "headers/game.hpp"
#include "headers/compatible.hpp"
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cctype> 
#include <filesystem>

bool isValidFilename(const std::string& filename) {
    // Check if the filename contains any invalid characters
    for (char ch : filename) {
        if (!(isalnum(ch) || ch == '_' || ch == '-')) {
            return false; 
        }
    }
    return true;
}

void sanitizeInput(std::string& input) {
    // Trim leading and trailing whitespace
    input.erase(input.begin(), std::find_if(input.begin(), input.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    input.erase(std::find_if(input.rbegin(), input.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), input.end());

    std::transform(input.begin(), input.end(), input.begin(), ::tolower);

    // Check for directory traversal
    if (input.find("..") != std::string::npos) {
        throw std::runtime_error("Invalid input: directory traversal is not allowed.");
    }
}

//////////////* Default Constructor *////

Game::Game() : player(), dealer(), deck(), s() {
    deck.initializeDeck();
}
//////////////* Deals dealer towards the end *////

bool Game::dealDealer(){
    if(dealer.getSum()<player.getSum()){
        while (dealer.getSum() < 17){
            dealer.addCard(deck.deal());
            if (checkWins()){
                return false;
            }
        }
        return true;
    }
    else{
        if(checkWins()){
            return false;
        }
        return true;
    }
}

//////////////* Checkers *////

char Game::compareSum(){
    if(player.getSum()>dealer.getSum()){
        printTop();
        std::cout<<lightYellow<<Print::you_win()<<def<<"\n    (Dealer has "<<dealer.getSum()<<")\n";
        return 'p';
    }
    else if(dealer.getSum()>player.getSum()){
        printTop();
        std::cout<<lightRed<<Print::dealer_wins()<<def<<"\n    ("<<dealer.getSum()<<")\n";
        return 'd';
    }
    else{
        printTop();
        std::cout<<lightMagenta<<Print::draw()<<def;
        return 'n';
    }
}

bool Game::checkWins(){
    switch (checkEnd()) {
        case 'f': return false;
        case 'd': player.incrementLoses(); return true;
        case 'p': player.incrementWins(); player.addCash(player.getBet() * 2); return true;
        default:  // Default case added to handle unexpected values
            std::cerr << "Unhandled case in checkEnd()." << std::endl;
            return false;
    }
}

char Game::checkEnd(){
    if(dealer.getSum()>21 || player.getSum()>21){
        printTop();
        std::cout<<red<<Print::bust()<<def<<"\n    [Dealer : "<<dealer.getSum()<<" | "<<player.getName()<<" : "<<player.getSum()<<"]\n";
        if(dealer.getSum()>21){
            return 'p';
        }
        else if(player.getSum()>21){
            return 'd';
        }
    }
    else if(dealer.getSum()==21 || player.getSum()==21){
        printTop();
        std::cout<<lightGreen<<Print::blackjack()<<def<<"\n    [Dealer : "<<dealer.getSum()<<" | "<<player.getName()<<" : "<<player.getSum()<<"]\n";
        if(dealer.getSum()==21){
            return 'd';
        }
        else if(player.getSum()==21){
            return 'p';
        }
    }
    return 'f';
}

//////////////* Game Starters *////

bool Game::startBet() {
    if(player.getCash() > 0) {
        while(true) {
            printTop();
            std::cout << "Place your bet! $ " << green << player.getBet() << def 
                      << "\n[W = Raise Bet | S = Decrease Bet | R = Done]\n";
            char input = getch();
            int c = toupper(input); // Only allow uppercase processing
            if (!isalpha(input)) {  // Validate that the input is a letter
                std::cerr << "Invalid input. Please enter W, S, or R." << std::endl;
                continue;
            }
            switch(c) {
                case 'W':
                    if(player.getCash() >= 5) {
                        player.setBet(5);
                    }
                    break;
                case 'S':
                    if(player.getBet() >= 5) {
                        player.setBet(-5);
                    }
                    break;
                case 'R':
                    return true;
                default:
                    std::cerr << "Unhandled betting input: " << static_cast<char>(c) << std::endl;
                    break;
            }
        }
    } else {
        std::cout << "Insufficient funds to place any bets.\n";
        return false;
    }
}

bool Game::startGame(){
    player.addCard(deck.deal());
    dealer.addCard(deck.deal());
    player.addCard(deck.deal());
    dealer.addCard(deck.deal());
    printBody();
    if(checkWins()){
        return false;
    }
    while(true){
        std::cout << lightYellow << "\n\nH : Hit | S : Stand\n"<<def;
        int c = toupper(getch());
        if(c==72){
            player.addCard(deck.deal());
            printBody();
            if(checkWins()) return false;
        }
        else if(c==83){
            break;
        }
    }
    return true;
}

void Game::beginGame(){
    char cont;
    do{
        if(deck.getSize()<36){
                deck.initializeDeck();
        }
        player.clearCards();
        dealer.clearCards();
        if(!startBet()){
            std::cout<<lightRed<<"\nBankrupt! Game over.\n"<<def;
            break;
        }
        if (startGame()){
            if (dealDealer()){
                switch (compareSum()) {
                    case 'p': player.incrementWins(); player.addCash(player.getBet() * 2); break;
                    case 'd': player.incrementLoses(); break;
                    case 'n': player.addCash(player.getBet()); break;
                    default:
                        std::cerr << "Unexpected result from compareSum()" << std::endl;
                        break;
                }
            }
        }
        std::cout<<lightRed<<Print::dealer_border()<<def;
        dealer.printCards();
        std::cout<<lightCyan<<Print::player_border()<<def;
        player.printCards();
        std::cout << yellow << "\nYour wins: " << player.getWins()<< lightRed <<"\nYour loses: "<<player.getLoses()<<def<<"\n";
        if(s.check(player)){
            std::cout<< lightYellow << "High Score!\n"<<def;
        }
        std::cout<<"\nContinue playing? [Y/N]: ";
        cont = getValidCharInput();
    } while (cont != 'N' && cont != 'n');
    char saveChoice;
    std::cout<<"\nSave game? [Y/N]: ";
    std::cin>>saveChoice;
    if(saveChoice == 'Y' || saveChoice == 'y'){
        saveGame();
    }
}

//////////////* Main Method to be Called *////

void Game::beginMenu(bool rep, std::string message){
    clearscr();
    std::cout<<yellow<<Print::title_blackjack()<<def<<"\n";
    std::cout<<Print::begin_menu()<<"\n";
    if(rep){
        std::cout<<red<<message<<def<<"\n";
    }
    char c;
    std::cout<<"Input : ";
    std::cin>>c;
    switch(c){
        case '1': char nm[100];
                  std::cout<<"Enter player name: ";
                  std::cin>>nm;
                  player.setName(nm);
                  beginGame();
                  break;
        case '2': loadGame();
                  beginGame();
                  break;
        case '3': printStatistics();
                  beginMenu(false, "");
                  break;
        case '4': printInstructions();
                  beginMenu(false, "");
                  break;
        case '5': exit(0);
                  break;
        default: beginMenu(true, "Invalid input.");
    }
}

//////////////* Data File Handling *////

void Game::saveGame() {
    std::fstream f1;
    std::string filename, path = "data/";
    bool valid = false;

    do {
        std::cout << "Enter filename: ";
        std::cin >> filename;

        try {
            sanitizeInput(filename);
        } catch (const std::runtime_error& e) {
            std::cout << e.what() << std::endl;
            continue;
        }

        if (!isValidFilename(filename)) {
            std::cout << "Error: Invalid filename entered." << std::endl;
        } else {
            valid = true;
        }
    } while (!valid);

    path += filename + ".bin";

    if (std::filesystem::exists(path)) {
        char choice;
        std::cout << "File already exists. Do you want to overwrite it? [Y/N]: ";
        std::cin >> choice;
        if (tolower(choice) == 'n') {
            return;
        }
    }

    f1.open(path, std::ios::out | std::ios::binary);
    if (!f1.is_open()) {
        std::cerr << "Failed to open file for writing." << std::endl;
        return;
    }

    // Assume fetching data from a 'player' object
    std::string sName = player.getName();
    int nameSize = sName.size();
    int sCash = player.getCash();
    int sWins = player.getWins();
    int sLoses = player.getLoses();

    // Write data
    f1.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
    f1.write(sName.c_str(), static_cast<std::streamsize>(sName.size()));
    f1.write(reinterpret_cast<const char*>(&sCash), sizeof(sCash));
    f1.write(reinterpret_cast<const char*>(&sWins), sizeof(sWins));
    f1.write(reinterpret_cast<const char*>(&sLoses), sizeof(sLoses));
    f1.close();
}

void Game::loadGame() {
    std::fstream f1;
    std::string filename;
    std::cout << "Enter filename: ";
    std::cin >> filename;

    sanitizeInput(filename);

    if (!isValidFilename(filename)) {
        std::cerr << "Error: Invalid filename entered." << std::endl;
    }    
    std::string path = "data/";
    std::cout << "Enter filename: ";
    std::cin >> filename;
    std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);
    path += filename + ".bin";
    f1.open(path, std::ios::in | std::ios::binary);
    if (f1.is_open()) {
        std::string sName;
        int nameSize;
        f1.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
        sName.resize(static_cast<std::string::size_type>(std::max(nameSize, 0)));
        f1.read(&sName[0], static_cast<std::streamsize>(sName.size()));
        int sCash, sWins, sLoses;
        f1.read(reinterpret_cast<char*>(&sCash), sizeof(sCash));
        f1.read(reinterpret_cast<char*>(&sWins), sizeof(sWins));
        f1.read(reinterpret_cast<char*>(&sLoses), sizeof(sLoses));
        f1.close();
        player.setName(sName);
        player.addCash(sCash - player.getCash());
        while (player.getWins() != sWins) {
            player.incrementWins();
        }
        while (player.getLoses() != sLoses) {
            player.incrementLoses();
        }
    } else {
        std::cout << "File does not exist. Please try again." << std::endl;
    }
}

//////////////* Printing Stuff *////

void Game::printStatistics(){
    clearscr();
    std::cout<<yellow<<Print::title_blackjack()<<def<<"\n";
    std::cout<<"\n"<<lightGreen<<Print::statistics()<<def<<"\n";
    s.print();
    std::cout<<"\n\n\t(Press any key to continue)\n";
    getch();
}

void Game::printInstructions(){
    clearscr();
    std::cout<<yellow<<Print::title_blackjack()<<def<<"\n";
    std::cout<<"\n"<<lightGreen<<Print::instructions()<<def<<"\n";
    getch();
}

void Game::printTop(){
    clearscr();
    std::cout<<yellow<<Print::title_blackjack()<<def<<"\n";
    std::cout<<lightRed<<"\t\tCards: "<<deck.getSize()<<lightGreen<<" \tCash: "<<player.getCash()<<lightMagenta
             <<" \tBet: "<<player.getBet()<<lightBlue<<" \tName: "<<player.getName()<<def<<"\n\n\n";
}

void Game::printBody(){
    printTop();
    std::cout<<lightRed<<Print::dealer_border()<<def;
    dealer.printFirstCard();
    std::cout<<lightCyan<<Print::player_border()<<def;
    player.printCards();
    std::cout << lightGreen<< "\nSum: "<<lightRed<< player.getSum()<<def<<"\n";
}

char Game::getValidCharInput() {
    char input;
    while (true) {
        std::cin >> input;
        input = toupper(input);
        if (input == 'Y' || input == 'N') {
            return input;
        } else {
            std::cout << "Invalid input. Please enter Y or N: ";
        }
    }
}

