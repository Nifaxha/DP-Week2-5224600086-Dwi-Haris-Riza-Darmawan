#ifndef SCORINGSYSTEM_H
#define SCORINGSYSTEM_H

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "Card.h"

using namespace std;

// Interface NEEDED: 8 concrete hand-evaluation strategies share one contract.
// ScoringSystem iterates them polymorphically from strongest to weakest.
// Adding a new hand type = add one new class, nothing else changes.
class IScoringStrategy {
public:
    virtual ~IScoringStrategy() = default;
    virtual int    calculateScore(const vector<Card>& cards) = 0;
    virtual string getHandName() = 0;
};

// ── Concrete strategies ──────────────────────────────────────────

class StraightFlushStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        if (cards.size() < 5) return 0;
        string firstSuit = cards[0].suit;
        vector<int> vals;
        for (const auto& c : cards) {
            if (c.suit != firstSuit) return 0;
            vals.push_back(c.value);
        }
        sort(vals.begin(), vals.end());
        for (size_t i = 1; i < vals.size(); i++)
            if (vals[i] != vals[i-1] + 1) return 0;
        return 800;
    }
    string getHandName() override { return "Straight Flush"; }
};

class FullHouseStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        if (cards.size() != 5) return 0;
        map<int,int> counts;
        for (const auto& c : cards) counts[c.value]++;
        bool has3 = false, has2 = false;
        for (auto& [v, cnt] : counts) {
            if (cnt == 3) has3 = true;
            if (cnt == 2) has2 = true;
        }
        return (has3 && has2) ? 400 : 0;
    }
    string getHandName() override { return "Full House"; }
};

class FlushStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        if (cards.size() < 5) return 0;
        string firstSuit = cards[0].suit;
        for (const auto& c : cards)
            if (c.suit != firstSuit) return 0;
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
        for (size_t i = 1; i < vals.size(); i++)
            if (vals[i] != vals[i-1] + 1) return 0;
        return 250;
    }
    string getHandName() override { return "Straight"; }
};

class ThreeOfAKindStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        map<int,int> counts;
        for (const auto& c : cards) counts[c.value]++;
        for (auto& [v, cnt] : counts)
            if (cnt >= 3) return (v * 3) + 150;
        return 0;
    }
    string getHandName() override { return "Three of a Kind"; }
};

class TwoPairStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        map<int,int> counts;
        int pairs = 0, score = 0;
        for (const auto& c : cards) counts[c.value]++;
        for (auto& [v, cnt] : counts)
            if (cnt >= 2) { pairs++; score += v * 2; }
        return (pairs >= 2) ? score + 100 : 0;
    }
    string getHandName() override { return "Two Pair"; }
};

class PairStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Card>& cards) override {
        map<int,int> counts;
        for (const auto& c : cards) counts[c.value]++;
        for (auto& [v, cnt] : counts)
            if (cnt >= 2) return (v * 2) + 50;
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

// ── ScoringSystem — concrete class, NO interface needed ──────────
// Only one scoring system exists; RunSession never swaps it polymorphically.
class ScoringSystem {
public:
    int evaluateHand(const vector<Card>& cards, string& outHandName) {
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
            int s = strat->calculateScore(cards);
            if (s > 0) {
                outHandName = strat->getHandName();
                finalScore = s;
                break;
            }
        }
        for (auto strat : strategies) delete strat;
        return finalScore;
    }
};

#endif