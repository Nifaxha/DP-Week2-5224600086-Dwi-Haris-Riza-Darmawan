#ifndef ITARGET_SCALER_H
#define ITARGET_SCALER_H

// Interface DIBUTUHKAN: skala kenaikan target skor bersifat mutable.
// RunSession tidak perlu tahu angka kenaikannya.
class ITargetScaler {
public:
    virtual ~ITargetScaler() = default;
    virtual int nextTarget(int currentTarget, int roundNumber) = 0;
};

// Default: naik flat +350 setiap ronde
class FlatTargetScaler : public ITargetScaler {
public:
    int nextTarget(int currentTarget, int roundNumber) override {
        return currentTarget + 350;
    }
};

// Alternatif: naik eksponensial x1.5
class ExponentialTargetScaler : public ITargetScaler {
public:
    int nextTarget(int currentTarget, int roundNumber) override {
        return static_cast<int>(currentTarget * 1.5);
    }
};

#endif
