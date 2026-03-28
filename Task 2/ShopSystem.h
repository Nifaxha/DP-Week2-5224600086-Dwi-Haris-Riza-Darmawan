#ifndef SHOPSYSTEM_H
#define SHOPSYSTEM_H

#include <iostream>
#include <vector>
#include "ModifierFactory.h"

using namespace std;

struct CatalystItem {
    int    modifierType;
    string displayName;
    string description;
    int    price;
};

// ================================================================
// ShopSystem — concrete class (tidak butuh interface)
// Isi katalog bersifat mutable tanpa mengubah RunSession.
// ================================================================
class ShopSystem {
public:
    IModifier* visitShop(int& playerCoins, int elementCount = 0) {
        vector<CatalystItem> catalog = {
            {1, "Potency Booster I",    "Tambah +25 skor reaksi",           3},
            {2, "Potency Booster II",   "Tambah +50 skor reaksi",           5},
            {3, "Reaction Amplifier",   "Kalikan skor x2",                  6},
            {4, "Grand Amplifier",      "Kalikan skor x3",                  9},
            {5, "Exothermic Surge",     "Kuadratkan skor (maks 9999)",      12},
            {6, "Element Catalyst",     "Bonus +20 per elemen unik",        4},
        };

        cout << "\n||==========================================||\n";
        cout << "||      ALCHEMY LAB CATALYST SHOP   ||\n";
        cout << "||==========================================||\n";
        cout << "  Koin: " << playerCoins << "\n\n";

        for (size_t i = 0; i < catalog.size(); i++) {
            cout << "  [" << (i+1) << "] " << catalog[i].displayName
                 << "\n      " << catalog[i].description
                 << " | Harga: " << catalog[i].price << " koin\n";
        }
        cout << "  [0] Lewati toko\n";
        cout << "\n  Pilihan: ";

        int choice;
        cin >> choice;

        if (choice < 1 || choice > (int)catalog.size()) {
            cout << "  >> Kamu meninggalkan lab.\n";
            return nullptr;
        }

        CatalystItem& item = catalog[choice - 1];
        if (playerCoins < item.price) {
            cout << "  >> Koin tidak cukup! (butuh " << item.price << ")\n";
            return nullptr;
        }

        playerCoins -= item.price;
        cout << "  >> [" << item.displayName << "] dibeli! Sisa koin: " << playerCoins << "\n";
        return ModifierFactory::createModifier(item.modifierType, elementCount);
    }
};

#endif
