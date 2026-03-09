#ifndef CARD_H
#define CARD_H
#include <string>

struct Card {
    std::string display;
    int value;
    std::string suit; // Tambahan untuk mendeteksi tipe kartu (Heart, Spade, dll)
};

#endif