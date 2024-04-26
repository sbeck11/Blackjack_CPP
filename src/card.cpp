#include "headers/card.h"
#include <iostream>

// Default constructor with initialization list
Card::Card() : number(0), suit('\0'), block(false) {
}

// Parameterized constructor with initialization list
Card::Card(int no, char s) : number(no), suit(s), block(false) {
}

int Card::getNumber() {
    return number;
}

char Card::getSuit() {
    return suit;
}

bool Card::getBlock() {
    return block;
}

void Card::setNumber(int no) {
    number = no;
}

void Card::setSuit(char c) {
    suit = c;
}

void Card::setBlock(bool b) {
    block = b;
}

char Card::getPrintNumber() {
    switch(number) {
        case 1: return 'A';
        case 10: return 'X';
        case 11: return 'J';
        case 12: return 'Q';
        case 13: return 'K';
        default: return '0' + static_cast<char>(number); // Simplified from an array for clarity
    }
}

void Card::printCardL1() {
    switch(suit) {
        case 'C': std::cout << "| :(): |"; break;
        case 'H': std::cout << "| (\\/) |"; break;
        case 'D':
        case 'S': std::cout << "| :/\\: |"; break;
        default:  std::cout << "|  //  |";
    }
}

void Card::printCardL2() {
    switch(suit) {
        case 'C': std::cout << "| ()() |"; break;
        case 'H':
        case 'D': std::cout << "| :\\/: |"; break;
        case 'S': std::cout << "| (__) |"; break;
        default:  std::cout << "|  //  |";
    }
}
