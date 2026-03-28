#ifndef MODIFIERFACTORY_H
#define MODIFIERFACTORY_H

#include "modifiers/IModifier.h"

// ================================================================
// ModifierFactory — Factory Pattern (Creational)
// ================================================================
// Memusatkan pembuatan semua katalis di satu tempat.
// ShopSystem tidak perlu tahu nama kelas konkret katalis.
// Modifier baru ditambah: hanya file ini yang berubah.
// ================================================================
class ModifierFactory {
public:
    static IModifier* createModifier(int type, int elementCount = 0) {
        switch (type) {
            case 1: return new PotencyBooster(25);
            case 2: return new PotencyBooster(50);
            case 3: return new ReactionAmplifier(2);
            case 4: return new ReactionAmplifier(3);
            case 5: return new ExothermicSurge();
            case 6: return new ElementBonusCatalyst(20, elementCount);
            default: return new PotencyBooster(10);
        }
    }
};

#endif
