#include "RunSession.h"
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <iomanip>
using namespace std;

RunSession::RunSession() {
    totalScore = 0;
    targetScore = 300;
    handsRemaining = 4;
    discardsRemaining = 3;
    roundNumber = 1;
    coins = 4; 
    srand(static_cast<unsigned int>(time(0))); 
}

RunSession::~RunSession() {
    for (auto mod : activeModifiers) delete mod;
}

void RunSession::fillHand() {
    vector<string> suits = {"Heart", "Spade", "Club", "Diamond"};
    vector<Card> deckPool;
    
    for (const auto& suit : suits) {
        for (int i = 2; i <= 10; i++) deckPool.push_back({to_string(i), i, suit});
        deckPool.push_back({"J", 10, suit});
        deckPool.push_back({"Q", 10, suit});
        deckPool.push_back({"K", 10, suit});
        deckPool.push_back({"A", 11, suit});
    }

    while (currentHand.size() < 7) {
        int randomIndex = rand() % deckPool.size();
        currentHand.push_back(deckPool[randomIndex]);
    }
}

void RunSession::displayHand() {
    cout << "\nKartu di tangan anda:\n";
    
    for (const auto& c : currentHand) {
        string cardStr = c.display + "-" + c.suit[0];
        cout << setw(6) << left << cardStr;
    }
    cout << "\n";
    
    for (size_t i = 1; i <= currentHand.size(); ++i) {
        string indexStr = "(" + to_string(i) + ")";
        cout << setw(6) << left << indexStr;
    }
    cout << "\n";
}

void RunSession::playHand() {
    currentHand.clear();
    fillHand(); 
    
    while (handsRemaining > 0 && totalScore < targetScore) {
        cout << "\n==================================================\n";
        cout << " Round: " << roundNumber << " | Target Score: " << targetScore << " | Coins: " << coins << "\n";
        cout << " Current: " << totalScore << " | Hands: " << handsRemaining << " | Discards: " << discardsRemaining << "\n";
        cout << "==================================================\n";

        displayHand();
        
        cout << "\nPilih kartu (masukkan urutan angka, pisahkan spasi): ";
        string inputLine;
        // ws digunakan untuk membersihkan sisa 'Enter' dari input sebelumnya
        getline(cin >> ws, inputLine); 
        
        stringstream ss(inputLine);
        int choice;
        vector<int> selectedIndices;
        vector<Card> selectedCards;
        
        // ss >> choice akan otomatis berhenti membaca jika sudah tidak ada angka di baris tersebut
        while (ss >> choice) {
            if (choice > 0 && choice <= currentHand.size()) {
                if (find(selectedIndices.begin(), selectedIndices.end(), choice) == selectedIndices.end()) {
                    selectedIndices.push_back(choice);
                    selectedCards.push_back(currentHand[choice - 1]);
                }
            }
        }
        // ------------------------------------------------

        if (selectedCards.empty()) {
            cout << "Kamu belum memilih kartu apa pun!\n";
            continue;
        }

        cout << "\nApakah kamu ingin:\n";
        cout << "[1] Mainkan Kartu Ini\n";
        cout << "[2] Buang (Discard) Kartu Ini\n";
        cout << "Pilih (1/2): ";
        int action;
        cin >> action;

        if (action == 2) {
            if (discardsRemaining > 0) {
                discardsRemaining--;
                cout << "\n>> Membuang " << selectedIndices.size() << " kartu...\n";
            } else {
                cout << "\n>> GAGAL: Kesempatan Discard kamu sudah habis!\n";
                continue; 
            }
        } 
        else if (action == 1) {
            string handName;
            int handScore = scoringSystem.evaluateHand(selectedCards, handName);
            cout << "\n>> KOMBINASI: " << handName << " | Base Score: " << handScore << "\n";

            for (auto mod : activeModifiers) {
                handScore = mod->applyModification(handScore);
                cout << ">> " << mod->getName() << " diterapkan! Skor menjadi: " << handScore << "\n";
            }

            totalScore += handScore;
            handsRemaining--;
        }

        sort(selectedIndices.rbegin(), selectedIndices.rend());
        for (int idx : selectedIndices) {
            currentHand.erase(currentHand.begin() + idx - 1);
        }

        fillHand();
    }
}

void RunSession::enterShop() {
    IModifier* newMod = shopSystem.visitShop(coins);
    if (newMod != nullptr) activeModifiers.push_back(newMod);
}

void RunSession::startRun() {
    cout << "Selamat Datang di AMAZING CARD!\n";
    for (int i = 0; i < 3; i++) { 
        totalScore = 0;
        handsRemaining = 4;
        discardsRemaining = 3; 
        
        playHand();
        
        if (totalScore >= targetScore) {
            int earnedCoins = 3 + handsRemaining; 
            coins += earnedCoins;

            cout << "\n*** KAMU MEMENANGKAN ROUND " << roundNumber << "! ***\n";
            cout << ">> Reward Round Base   : 3 Coins\n";
            cout << ">> Reward Sisa Hands   : " << handsRemaining << " Coins\n";
            cout << ">> Total Pendapatan    : " << earnedCoins << " Coins\n";
            
            enterShop();
            roundNumber++;
            targetScore += 400; 
        } else {
            cout << "\n*** GAME OVER! Target tidak tercapai. ***\n";
            return;
        }
    }
    cout << "\n*** SELAMAT! KAMU MEMENANGKAN GAME INI! ***\n";
}