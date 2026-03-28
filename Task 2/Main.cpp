#include "RunSession.h"
#include "IRewardCalculator.h"
#include "ITargetScaler.h"

int main() {
    // Elemen mutable diinjeksikan dari luar
    // Ganti ke BonusRewardCalculator atau ExponentialTargetScaler
    // tanpa menyentuh RunSession sama sekali
    BaseRewardCalculator rewardCalc;
    FlatTargetScaler     targetScaler;

    RunSession game(&rewardCalc, &targetScaler);
    game.startRun();

    return 0;
}
