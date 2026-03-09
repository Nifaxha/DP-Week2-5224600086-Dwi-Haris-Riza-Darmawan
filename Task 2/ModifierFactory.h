#ifndef MODIFIERFACTORY_H
#define MODIFIERFACTORY_H

#include "modifiers/IModifier.h"

class ModifierFactory {
public:
    // Factory Method untuk membuat modifier secara dinamis
    static IModifier* createModifier(int type) {
        switch (type) {
            case 1: return new FlatBonusModifier(30);  // Tipe 1: +30 Skor
            case 2: return new MultiplierModifier(2);  // Tipe 2: Skor x2
            case 3: return new FlatBonusModifier(50);  // Tipe 3: +50 Skor
            default: return new FlatBonusModifier(10);
        }
    }
};

#endif