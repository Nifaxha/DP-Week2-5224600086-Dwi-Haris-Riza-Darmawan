#ifndef RUNSESSION_H
#define RUNSESSION_H

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include "Reagent.h"
#include "ScoringSystem.h"
#include "modifiers/IModifier.h"
#include "ShopSystem.h"
#include "IRewardCalculator.h"
#include "ITargetScaler.h"

using namespace std;

// ================================================================
// RunSession — pengontrol invariant loop
// ================================================================
// Hanya mengontrol urutan fase. Semua logika didelegasikan ke:
//   - ScoringSystem      : evaluasi reaksi (Strategy Pattern)
//   - IModifier chain    : modifikasi skor (Decorator-like)
//   - ShopSystem         : fase toko
//   - IRewardCalculator  : formula reward koin (mutable)
//   - ITargetScaler      : skala target skor (mutable)
// ================================================================
class RunSession {
private:
    int totalScore;
    int targetScore;
    int attemptsRemaining;
    int discardRemaining;
    int roundNumber;
    int coins;

    vector<Reagent>    currentLab;       // reagen di tangan
    vector<IModifier*> activeModifiers;  // katalis aktif (IModifier* — polimorfis)

    ScoringSystem      scoringSystem;    // konkret
    ShopSystem         shopSystem;       // konkret
    IRewardCalculator* rewardCalc_;      // mutable — diinjeksikan
    ITargetScaler*     targetScaler_;    // mutable — diinjeksikan

    void fillLab();
    void displayLab();
    int  applyModifiers(int score, int uniqueElements);
    void displayStatus();

public:
    RunSession(IRewardCalculator* rewardCalc, ITargetScaler* targetScaler);
    ~RunSession();

    void startRun();      // invariant loop
    void playRound();     // inner loop
    void enterShop();
};

#endif
