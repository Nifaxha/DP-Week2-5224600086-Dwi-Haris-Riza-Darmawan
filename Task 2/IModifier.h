#ifndef IMODIFIER_H
#define IMODIFIER_H
#include <string>
using namespace std;

class IModifier {
public:
    virtual ~IModifier() = default;
    // Fungsi untuk memodifikasi skor (berantai)
    virtual int applyModification(int currentScore) = 0;
    virtual string getName() = 0;
};

// Concrete Modifier A: Tambahan Flat Bonus
class FlatBonusModifier : public IModifier {
private:
    int bonus;
public:
    FlatBonusModifier(int b) : bonus(b) {}
    int applyModification(int currentScore) override {
        return currentScore + bonus;
    }
    string getName() override { return "Flat Bonus Enhancer (+" + to_string(bonus) + ")"; }
};

// Concrete Modifier B: Pengali (Multiplier)
class MultiplierModifier : public IModifier {
private:
    int multiplier;
public:
    MultiplierModifier(int m) : multiplier(m) {}
    int applyModification(int currentScore) override {
        return currentScore * multiplier;
    }
    string getName() override { return "Multiplier Enhancer (x" + to_string(multiplier) + ")"; }
};

#endif