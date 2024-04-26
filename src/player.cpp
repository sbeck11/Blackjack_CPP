#include "headers/player.hpp"
#include <string>


Player::Player() : name("Unknown"), cash(1000), bet(0), wins(0), loses(0) {
}

std::string Player::getName() {
    return name;
}

int Player::getBet() {
    return bet;
}

int Player::getCash() {
    return cash;
}

int Player::getWins() {
    return wins;
}

int Player::getLoses() {
    return loses;
}

void Player::setName(std::string nm) {
    name = nm;
}

void Player::setBet(int b) {
    cash -= b;
    bet += b;
}

void Player::addCash(int c) {
    cash += c;
}

void Player::incrementWins() {
    wins += 1;
}

void Player::incrementLoses() {
    loses += 1;
}

void Player::clearCards() {
    Human::clearCards();  // Assuming Human has a clearCards method
    bet = 0;
}