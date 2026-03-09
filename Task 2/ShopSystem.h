#ifndef SHOPSYSTEM_H
#define SHOPSYSTEM_H

#include <iostream>
#include "ModifierFactory.h"
using namespace std;

class ShopSystem {
public:
    // Menerima referensi koin pemain agar bisa dikurangi saat membeli
    IModifier* visitShop(int& playerCoins) {
        cout << "\n--- AMAZING CARD SHOP ---\n";
        cout << "Coins Anda: " << playerCoins << "\n";
        cout << "Pilih Enhancer untuk dibeli:\n";
        cout << "1. Flat Bonus Enhancer (+30 Score) - Harga: 4 Coins\n";
        cout << "2. Multiplier Enhancer (x2 Score)  - Harga: 6 Coins\n";
        cout << "0. Lewati Shop\n";
        cout << "Pilihan: ";
        
        int choice;
        cin >> choice;
        
        if (choice == 1) {
            if (playerCoins >= 4) {
                playerCoins -= 4;
                cout << ">> Enhancer berhasil dibeli! Sisa koin: " << playerCoins << "\n";
                return ModifierFactory::createModifier(1);
            } else {
                cout << ">> Koin tidak cukup!\n";
            }
        } else if (choice == 2) {
            if (playerCoins >= 6) {
                playerCoins -= 6;
                cout << ">> Enhancer berhasil dibeli! Sisa koin: " << playerCoins << "\n";
                return ModifierFactory::createModifier(2);
            } else {
                cout << ">> Koin tidak cukup!\n";
            }
        }
        
        cout << ">> Anda meninggalkan shop.\n";
        return nullptr;
    }
};

#endif