#include "headers/deck.hpp"
#include <iostream>
#include <algorithm>
#include <random>

void Deck::initializeDeck(){
    deck.clear();
    char suits[4] = {'S', 'H', 'D', 'C'};
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 13; j++) {
            Card c(j+1, suits[i]);
            deck.push_back(c);
        }
    }
    std::random_device rd; // Non-deterministic random number generator
    std::mt19937 g(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::shuffle(deck.begin(), deck.end(), g);
}

// Getter Function for size of deck
int Deck::getSize(){
    return deck.size();
}

// Deals by returning one card from the deck
Card Deck::deal(){
    if (deck.empty()) {
        throw std::out_of_range("Cannot deal from an empty deck"); // It's good to check this
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<size_t> dis(0, deck.size() - 1);
    size_t val = dis(g); // size_t to match the type returned by deck.size()
    Card t = deck[val];
    deck.erase(deck.begin() + static_cast<std::vector<Card>::difference_type>(val));
    return t;
}