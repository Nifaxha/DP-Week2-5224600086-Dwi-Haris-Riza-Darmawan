#ifndef SCORINGSYSTEM_H
#define SCORINGSYSTEM_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include "Reagent.h"

using namespace std;

// ================================================================
// IScoringStrategy — Strategy Pattern (Behavioral)
// ================================================================
// DIBUTUHKAN: setiap reaksi kimia punya aturan evaluasi berbeda.
// Strategy memisahkan logika tiap reaksi dari ScoringSystem.
// Reaksi baru = tambah class baru, ScoringSystem tidak berubah.
// ================================================================
class IScoringStrategy {
public:
    virtual ~IScoringStrategy() = default;
    virtual int    calculateScore(const vector<Reagent>& reagents) = 0;
    virtual string getReactionName() = 0;
};

// ── Helper: hitung jumlah elemen tertentu ───────────────────────
static int countElement(const vector<Reagent>& r, Element e) {
    int n = 0;
    for (const auto& x : r) if (x.element == e) n++;
    return n;
}

// ================================================================
// H2O — 2 Hydrogen + 1 Oxygen
// ================================================================
class WaterReactionStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Reagent>& r) override {
        if (countElement(r, Element::HYDROGEN) >= 2 &&
            countElement(r, Element::OXYGEN)   >= 1) {
            int total = 0;
            for (const auto& x : r) total += x.potency;
            return total + 80;
        }
        return 0;
    }
    string getReactionName() override { return "H2O (Air)"; }
};

// ================================================================
// CO2 — 1 Carbon + 2 Oxygen
// ================================================================
class CarbonDioxideStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Reagent>& r) override {
        if (countElement(r, Element::CARBON) >= 1 &&
            countElement(r, Element::OXYGEN) >= 2) {
            int total = 0;
            for (const auto& x : r) total += x.potency;
            return total + 100;
        }
        return 0;
    }
    string getReactionName() override { return "CO2 (Karbon Dioksida)"; }
};

// ================================================================
// NH3 — 1 Nitrogen + 3 Hydrogen
// ================================================================
class AmmoniaStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Reagent>& r) override {
        if (countElement(r, Element::NITROGEN) >= 1 &&
            countElement(r, Element::HYDROGEN) >= 3) {
            int total = 0;
            for (const auto& x : r) total += x.potency;
            return total + 140;
        }
        return 0;
    }
    string getReactionName() override { return "NH3 (Amonia)"; }
};

// ================================================================
// Fe2O3 — 2 Iron + 3 Oxygen (Karat besi)
// ================================================================
class IronOxideStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Reagent>& r) override {
        if (countElement(r, Element::IRON)   >= 2 &&
            countElement(r, Element::OXYGEN) >= 3) {
            int total = 0;
            for (const auto& x : r) total += x.potency;
            return total + 200;
        }
        return 0;
    }
    string getReactionName() override { return "Fe2O3 (Besi Oksida)"; }
};

// ================================================================
// NaCl — 1 Sodium + 1 Sulfur (garam sederhana, analog)
// ================================================================
class SaltReactionStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Reagent>& r) override {
        if (countElement(r, Element::SODIUM)  >= 1 &&
            countElement(r, Element::SULFUR)  >= 1) {
            int total = 0;
            for (const auto& x : r) total += x.potency;
            return total + 60;
        }
        return 0;
    }
    string getReactionName() override { return "Na2S (Natrium Sulfida)"; }
};

// ================================================================
// CaO — 1 Calcium + 1 Oxygen (Kapur bakar)
// ================================================================
class LimeStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Reagent>& r) override {
        if (countElement(r, Element::CALCIUM) >= 1 &&
            countElement(r, Element::OXYGEN)  >= 1) {
            int total = 0;
            for (const auto& x : r) total += x.potency;
            return total + 50;
        }
        return 0;
    }
    string getReactionName() override { return "CaO (Kapur Bakar)"; }
};

// ================================================================
// Full Spectrum — semua 5+ elemen berbeda (reaksi langka)
// ================================================================
class FullSpectrumStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Reagent>& r) override {
        set<Element> uniqueElems;
        int total = 0;
        for (const auto& x : r) {
            uniqueElems.insert(x.element);
            total += x.potency;
        }
        if ((int)uniqueElems.size() >= 5) return total * 4 + 300;
        return 0;
    }
    string getReactionName() override { return "Full Spectrum (Reaksi Langka!)"; }
};

// ================================================================
// BasicMix — fallback jika tidak ada reaksi yang cocok
// ================================================================
class BasicMixStrategy : public IScoringStrategy {
public:
    int calculateScore(const vector<Reagent>& r) override {
        int total = 0;
        for (const auto& x : r) total += x.potency;
        return total + 10;
    }
    string getReactionName() override { return "Basic Mix (Campuran Biasa)"; }
};

// ================================================================
// ScoringSystem — concrete class (tidak butuh interface)
// ================================================================
class ScoringSystem {
public:
    int evaluateReaction(const vector<Reagent>& reagents, string& outReactionName) {
        // Coba dari reaksi terkuat ke terlemah
        vector<IScoringStrategy*> strategies = {
            new FullSpectrumStrategy(),
            new IronOxideStrategy(),
            new AmmoniaStrategy(),
            new CarbonDioxideStrategy(),
            new WaterReactionStrategy(),
            new SaltReactionStrategy(),
            new LimeStrategy(),
            new BasicMixStrategy()
        };

        int finalScore = 0;
        for (auto s : strategies) {
            int score = s->calculateScore(reagents);
            if (score > 0) {
                outReactionName = s->getReactionName();
                finalScore      = score;
                break;
            }
        }
        for (auto s : strategies) delete s;
        return finalScore;
    }
};

#endif
