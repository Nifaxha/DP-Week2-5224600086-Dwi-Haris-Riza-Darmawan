#ifndef IMODIFIER_H
#define IMODIFIER_H

#include <string>

// ================================================================
// IModifier — interface untuk semua katalis (modifier)
// ================================================================
// DIBUTUHKAN: RunSession mengiterasi vector<IModifier*> secara
// polimorfis. Katalis baru bisa ditambah tanpa mengubah game loop.
// Berperilaku seperti Decorator: setiap katalis membungkus skor
// sebelumnya dan mengembalikan skor baru secara berantai.
// ================================================================
class IModifier {
public:
    virtual ~IModifier() = default;
    virtual int         applyModification(int currentScore) = 0;
    virtual std::string getName()        = 0;
    virtual std::string getDescription() = 0;
    virtual IModifier*  clone()          = 0;  // Prototype Pattern
};

// ================================================================
// CatalystA: PotencyBooster
// Efek: tambah nilai flat ke skor reaksi
// ================================================================
class PotencyBooster : public IModifier {
    int bonus_;
public:
    explicit PotencyBooster(int bonus) : bonus_(bonus) {}
    int applyModification(int score) override { return score + bonus_; }
    std::string getName()        override { return "Potency Booster (+" + std::to_string(bonus_) + ")"; }
    std::string getDescription() override { return "Menambahkan " + std::to_string(bonus_) + " poin ke skor reaksi"; }
    IModifier*  clone()          override { return new PotencyBooster(bonus_); }
};

// ================================================================
// CatalystB: ReactionAmplifier
// Efek: kalikan skor reaksi dengan faktor pengali
// ================================================================
class ReactionAmplifier : public IModifier {
    int factor_;
public:
    explicit ReactionAmplifier(int factor) : factor_(factor) {}
    int applyModification(int score) override { return score * factor_; }
    std::string getName()        override { return "Reaction Amplifier (x" + std::to_string(factor_) + ")"; }
    std::string getDescription() override { return "Mengalikan skor reaksi dengan " + std::to_string(factor_); }
    IModifier*  clone()          override { return new ReactionAmplifier(factor_); }
};

// ================================================================
// CatalystC: ExothermicSurge (SquareScoreModifier equivalent)
// Efek: reaksi eksotermik — mengkuadratkan skor (capped 9999)
// Ditambahkan sebagai bukti extensibility
// ================================================================
class ExothermicSurge : public IModifier {
public:
    int applyModification(int score) override {
        int result = score * score;
        return (result > 9999) ? 9999 : result;
    }
    std::string getName()        override { return "Exothermic Surge (Score^2)"; }
    std::string getDescription() override { return "Reaksi eksotermik: kuadratkan skor (maks 9999)"; }
    IModifier*  clone()          override { return new ExothermicSurge(); }
};

// ================================================================
// CatalystD: ElementBonusCatalyst
// Efek: bonus berdasarkan jumlah reagen unik yang digunakan
// ================================================================
class ElementBonusCatalyst : public IModifier {
    int bonusPerElement_;
    int elementCount_;
public:
    ElementBonusCatalyst(int bonusPerElement, int elementCount)
        : bonusPerElement_(bonusPerElement), elementCount_(elementCount) {}
    int applyModification(int score) override {
        return score + (bonusPerElement_ * elementCount_);
    }
    std::string getName()        override {
        return "Element Catalyst (+" + std::to_string(bonusPerElement_) + "/elemen)";
    }
    std::string getDescription() override {
        return "Bonus +" + std::to_string(bonusPerElement_) + " per elemen unik dalam reaksi";
    }
    IModifier*  clone()          override {
        return new ElementBonusCatalyst(bonusPerElement_, elementCount_);
    }
};

#endif
