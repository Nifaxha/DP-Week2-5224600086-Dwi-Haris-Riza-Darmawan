#ifndef IREWARD_CALCULATOR_H
#define IREWARD_CALCULATOR_H

// Interface DIBUTUHKAN: formula reward koin bersifat mutable.
// RunSession tidak perlu tahu cara reward dihitung.
class IRewardCalculator {
public:
    virtual ~IRewardCalculator() = default;
    virtual int computeReward(int attemptsRemaining, int roundNumber) = 0;
};

// Default: 3 koin + sisa attempt
class BaseRewardCalculator : public IRewardCalculator {
public:
    int computeReward(int attemptsRemaining, int roundNumber) override {
        return 3 + attemptsRemaining;
    }
};

// Alternatif: bonus koin di ronde genap
class BonusRewardCalculator : public IRewardCalculator {
public:
    int computeReward(int attemptsRemaining, int roundNumber) override {
        int base = 3 + attemptsRemaining;
        return (roundNumber % 2 == 0) ? base * 2 : base;
    }
};

#endif
