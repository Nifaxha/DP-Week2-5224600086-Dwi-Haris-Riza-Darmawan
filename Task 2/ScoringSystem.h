#ifndef SCORINGSYSTEM_H
#define SCORINGSYSTEM_H

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "Card.h"
using namespace std;

class IScoringStrategy {
public:
    virtual ~IScoringStrategy() = default;
    virtual int calculateScore(const vector<Card>& cards) = 0;
    virtual string getHandName() = 0;
};

class StraightFlushStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        if (cards.size() < 5) return 0;
        vector<int> vals;
        string firstSuit = cards[0].suit;
        for (const auto& c : cards) {
            if (c.suit != firstSuit) return 0; // Bukan Flush
            vals.push_back(c.value);
        }
        sort(vals.begin(), vals.end());
        for (size_t i = 1; i < vals.size(); i++) {
            if (vals[i] != vals[i-1] + 1) return 0; // Bukan Straight
        }
        return 800; // Base score
    }
    string getHandName() override { return "Straight Flush"; }
};

class FullHouseStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        if (cards.size() != 5) return 0;
        map<int, int> counts;
        for (const auto& c : cards) counts[c.value]++;
        bool has3 = false, has2 = false;
        for (auto const& [val, count] : counts) {
            if (count == 3) has3 = true;
            if (count == 2) has2 = true;
        }
        if (has3 && has2) return 400;
        return 0;
    }
    string getHandName() override { return "Full House"; }
};

class FlushStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        if (cards.size() < 5) return 0;
        string firstSuit = cards[0].suit;
        for (const auto& c : cards) {
            if (c.suit != firstSuit) return 0;
        }
        return 300;
    }
    string getHandName() override { return "Flush"; }
};

class StraightStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        if (cards.size() < 5) return 0;
        vector<int> vals;
        for (const auto& c : cards) vals.push_back(c.value);
        sort(vals.begin(), vals.end());
        for (size_t i = 1; i < vals.size(); i++) {
            if (vals[i] != vals[i-1] + 1) return 0;
        }
        return 250;
    }
    string getHandName() override { return "Straight"; }
};

class ThreeOfAKindStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        map<int, int> counts;
        for (const auto& c : cards) counts[c.value]++;
        for (auto const& [val, count] : counts) {
            if (count >= 3) return (val * 3) + 150;
        }
        return 0;
    }
    string getHandName() override { return "Three of a Kind"; }
};

class TwoPairStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        map<int, int> counts;
        int pairCount = 0;
        int score = 0;
        for (const auto& c : cards) counts[c.value]++;
        for (auto const& [val, count] : counts) {
            if (count >= 2) {
                pairCount++;
                score += (val * 2);
            }
        }
        if (pairCount >= 2) return score + 100;
        return 0;
    }
    string getHandName() override { return "Two Pair"; }
};

class PairStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        map<int, int> counts;
        for (const auto& c : cards) counts[c.value]++;
        for (auto const& [val, count] : counts) {
            if (count >= 2) return (val * 2) + 50;
        }
        return 0;
    }
    string getHandName() override { return "Pair"; }
};

class HighCardStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        int total = 0;
        for (const auto& c : cards) total += c.value;
        return total + 10; 
    }
    string getHandName() override { return "High Card"; }
};

class ScoringSystem {
public:
    int evaluateHand(const vector<Card>& cards, string& outHandName) {
        // Daftar strategi dievaluasi dari yang paling kuat ke paling lemah
        vector<IScoringStrategy*> strategies = {
            new StraightFlushStrategy(),
            new FullHouseStrategy(),
            new FlushStrategy(),
            new StraightStrategy(),
            new ThreeOfAKindStrategy(),
            new TwoPairStrategy(),
            new PairStrategy(),
            new HighCardStrategy()
        };

        int finalScore = 0;
        for (auto strat : strategies) {
            int score = strat->calculateScore(cards);
            if (score > 0) {
                outHandName = strat->getHandName();
                finalScore = score;
                break; // Hentikan jika kombinasi tertinggi sudah ditemukan
            }
        }

        for (auto strat : strategies) delete strat; // Bersihkan memori
        return finalScore;
    }
};

#endif