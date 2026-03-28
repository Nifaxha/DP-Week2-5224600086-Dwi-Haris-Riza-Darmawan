#include "RunSession.h"
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

// ── Pool reagen yang tersedia ────────────────────────────────────
static vector<Reagent> buildReagentPool() {
    return {
        {"H",  "Hydrogen",  Element::HYDROGEN, 5},
        {"H",  "Hydrogen",  Element::HYDROGEN, 5},
        {"H",  "Hydrogen",  Element::HYDROGEN, 5},
        {"H",  "Hydrogen",  Element::HYDROGEN, 5},
        {"O",  "Oxygen",    Element::OXYGEN,   8},
        {"O",  "Oxygen",    Element::OXYGEN,   8},
        {"O",  "Oxygen",    Element::OXYGEN,   8},
        {"C",  "Carbon",    Element::CARBON,   10},
        {"C",  "Carbon",    Element::CARBON,   10},
        {"N",  "Nitrogen",  Element::NITROGEN, 9},
        {"N",  "Nitrogen",  Element::NITROGEN, 9},
        {"Fe", "Iron",      Element::IRON,     15},
        {"Fe", "Iron",      Element::IRON,     15},
        {"S",  "Sulfur",    Element::SULFUR,   7},
        {"S",  "Sulfur",    Element::SULFUR,   7},
        {"Ca", "Calcium",   Element::CALCIUM,  12},
        {"Ca", "Calcium",   Element::CALCIUM,  12},
        {"Na", "Sodium",    Element::SODIUM,   6},
        {"Na", "Sodium",    Element::SODIUM,   6},
    };
}

// ── Constructor & Destructor ─────────────────────────────────────
RunSession::RunSession(IRewardCalculator* rewardCalc, ITargetScaler* targetScaler)
    : totalScore(0), targetScore(250),
      attemptsRemaining(4), discardRemaining(3),
      roundNumber(1), coins(5),
      rewardCalc_(rewardCalc), targetScaler_(targetScaler) {
    srand(static_cast<unsigned int>(time(0)));
}

RunSession::~RunSession() {
    for (auto mod : activeModifiers) delete mod;
}

// ── fillLab ──────────────────────────────────────────────────────
void RunSession::fillLab() {
    auto pool = buildReagentPool();
    while ((int)currentLab.size() < 7) {
        int idx = rand() % pool.size();
        currentLab.push_back(pool[idx]);
    }
}

// ── displayLab ───────────────────────────────────────────────────
void RunSession::displayLab() {
    cout << "\n  Reagen di meja lab:\n  ";
    for (const auto& r : currentLab)
        cout << setw(7) << left << (r.symbol + "(+" + to_string(r.potency) + ")");
    cout << "\n  ";
    for (size_t i = 1; i <= currentLab.size(); i++)
        cout << setw(7) << left << ("(" + to_string(i) + ")");
    cout << "\n";
}

// ── displayStatus ────────────────────────────────────────────────
void RunSession::displayStatus() {
    cout << "\n|===========================================|\n";
    cout << "|  Ronde: " << setw(3) << roundNumber
         << "  | Target: " << setw(4) << targetScore
         << " | Koin: " << setw(2) << coins << "    |\n";
    cout << "|  Skor : " << setw(3) << totalScore
         << "  | Coba : " << setw(4) << attemptsRemaining
         << "  | Buang: " << setw(2) << discardRemaining << "   |\n";
    if (!activeModifiers.empty()) {
        cout << "|  Katalis aktif:                           |\n";
        for (auto mod : activeModifiers)
            cout << "|     " << setw(37) << left << mod->getName() << "|\n";
    }
    cout << "|===========================================|\n";
}

// ── applyModifiers ───────────────────────────────────────────────
// Game loop hanya memanggil applyModifiers() — tidak perlu diubah
// saat katalis baru ditambah.
int RunSession::applyModifiers(int score, int uniqueElements) {
    if (activeModifiers.empty()) return score;
    cout << "\n  [Rantai Katalis]\n";
    for (auto mod : activeModifiers) {
        int before = score;
        score = mod->applyModification(score);
        cout << "   " << mod->getName()
             << "  " << before << " -> " << score << "\n";
    }
    return score;
}

// ── enterShop ────────────────────────────────────────────────────
void RunSession::enterShop() {
    int uniqueElems = (int)currentLab.size(); // estimasi
    IModifier* mod = shopSystem.visitShop(coins, uniqueElems);
    if (mod) activeModifiers.push_back(mod);
}

// ── playRound ────────────────────────────────────────────────────
void RunSession::playRound() {
    currentLab.clear();
    fillLab();  // Phase 1: isi meja lab

    while (attemptsRemaining > 0 && totalScore < targetScore) {
        displayStatus();
        displayLab();

        // Phase 2: pemain pilih reagen
        cout << "\n  Pilih reagen (nomor dipisah spasi): ";
        string line;
        getline(cin >> ws, line);
        stringstream ss(line);
        int idx;
        vector<int>     indices;
        vector<Reagent> selected;

        while (ss >> idx) {
            if (idx >= 1 && idx <= (int)currentLab.size()) {
                if (find(indices.begin(), indices.end(), idx) == indices.end()) {
                    indices.push_back(idx);
                    selected.push_back(currentLab[idx - 1]);
                }
            }
        }
        if (selected.empty()) { cout << "  X Belum pilih reagen!\n"; continue; }

        cout << "\n  [1] Reaksikan  [2] Buang\n  Pilih: ";
        int action; cin >> action;

        if (action == 2) {
            if (discardRemaining > 0) {
                discardRemaining--;
                cout << "  >> Membuang " << selected.size() << " reagen...\n";
            } else {
                cout << "  X Kesempatan buang habis!\n";
                continue;
            }
        } else if (action == 1) {
            // Phase 3: evaluasi reaksi — delegasi ke ScoringSystem (Strategy)
            string reactionName;
            int baseScore = scoringSystem.evaluateReaction(selected, reactionName);

            // Hitung elemen unik
            set<Element> uniqueSet;
            for (const auto& r : selected) uniqueSet.insert(r.element);
            int uniqueCount = (int)uniqueSet.size();

            cout << "\n    Reaksi: " << reactionName << "\n";
            cout << "  Base Score : " << baseScore << "\n";

            // Phase 4: terapkan rantai katalis — delegasi ke applyModifiers()
            // Menambah katalis baru TIDAK mengubah baris ini sama sekali
            int finalScore = applyModifiers(baseScore, uniqueCount);
            cout << "  Final Score: " << finalScore << "\n";

            // Phase 5: akumulasi skor
            totalScore += finalScore;
            attemptsRemaining--;
            cout << "  Total: " << totalScore << " / " << targetScore << "\n";
        }

        // Hapus reagen yang dipilih dan isi ulang
        sort(indices.rbegin(), indices.rend());
        for (int i : indices)
            currentLab.erase(currentLab.begin() + i - 1);
        fillLab();
    }
}

// ── startRun — INVARIANT loop ─────────────────────────────────────
void RunSession::startRun() {
    cout << "\n";
    cout << "||==========================================||\n";
    cout << "||             ALCHEMY LAB                  ||\n";
    cout << "||    Chemical Reaction Run-Based Game      ||\n";
    cout << "||==========================================||\n";
    cout << "  Kombinasikan reagen untuk menciptakan reaksi kimia!\n";
    cout << "  Beli katalis di toko untuk memperkuat reaksimu.\n\n";

    for (int round = 0; round < 3; round++) {
        // Reset state per ronde
        totalScore        = 0;
        attemptsRemaining = 4;
        discardRemaining  = 3;

        cout << "\n==========================================\n";
        cout << "  RONDE " << roundNumber << "  |  Target: " << targetScore << "\n";
        cout << "==========================================\n";

        // Phase: jalankan ronde
        playRound();

        // Phase: cek menang/kalah
        if (totalScore >= targetScore) {
            // Phase: hitung reward — delegasi ke IRewardCalculator (mutable)
            int reward = rewardCalc_->computeReward(attemptsRemaining, roundNumber);
            coins += reward;

            cout << "\n  RONDE " << roundNumber << " SELESAI! \n";
            cout << "  Skor: " << totalScore << " / " << targetScore << "\n";
            cout << "  Reward: " << reward << " koin | Total koin: " << coins << "\n";

            // Phase: toko katalis
            enterShop();

            // Phase: advance ronde — target naik via ITargetScaler (mutable)
            roundNumber++;
            targetScore = targetScaler_->nextTarget(targetScore, roundNumber);

        } else {
            cout << "\nX GAME OVER - Target tidak tercapai.\n";
            cout << "  Skor: " << totalScore << " / " << targetScore << "\n";
            return;
        }
    }

    cout << "\n||==========================================||\n";
    cout << "||    SELAMAT! KAMU KUASAI ALCHEMY LAB!   ||\n";
    cout << "||==========================================||\n";
}
