# Game Concept: Amazing Card (Balatro-Like Run)

---

## Step 1 — Core Loop

My game is a Balatro-inspired card game where the player plays poker hands across 3 rounds to
reach a target score.

The core loop is:

1. Player receives a hand of 7 cards
2. Player selects cards to play or discard
3. System evaluates the selected hand (poker combination)
4. Base score is calculated from the hand type
5. Active modifiers are applied to the score (chain)
6. Score is accumulated toward the target
7. If hands run out or target is reached, the round ends
8. Player visits the shop and may buy a modifier
9. Repeat for the next round with a higher target

This loop repeats every round until all 3 rounds are completed or the player fails.

---

## Step 2 — Invariant Identification

The following order must **never** change:

1. Generate / fill hand
2. Player selects cards (play or discard)
3. Evaluate hand type → compute base score
4. Apply modifiers to score
5. Accumulate score, consume hand/discard slot
6. Check round win/lose condition
7. Shop phase (only on round win)
8. Advance round

If this order changes, gameplay logic breaks.

For example:

1. If modifiers are applied before scoring, the base score does not exist yet — modifier
   produces a wrong result.
2. If the shop phase runs before the round win check, the player can buy upgrades even after
   losing a round.
3. If the score accumulation happens after the win check, the system may not detect that the
   target was reached on the final hand.
4. If hand generation is skipped or placed after player selection, the player has nothing
   to choose from.

These steps define the structural rhythm of each round.

### Required Components

The game must always have:

1. A `RunSession` — controls the round loop and phase order
2. A `Card` — the data unit representing a playing card
3. A `ScoringSystem` — evaluates hand type and computes base score
4. An `IModifier` — interface for all score modifiers
5. A `ShopSystem` — allows purchasing modifiers between rounds

Without these, the loop cannot function. These are structural necessities.

---

## Step 3 — Mutable Elements

The following parts are mutable:

1. **Scoring formula per hand type**
   Each strategy class (`PairStrategy`, `StraightStrategy`, etc.) has its own score formula.
   These numbers can change freely without affecting the phase order. The loop still calls
   `evaluateHand()` the same way.

2. **Modifier behavior**
   `FlatBonusModifier` and `MultiplierModifier` can be replaced with new types (e.g., a
   conditional modifier that only activates on Flush hands). The modifier chain loop in
   `playHand()` remains structurally the same.

3. **Shop offerings and prices**
   The `ShopSystem` can offer different modifiers, change prices, or expand the catalog.
   None of this changes the position of the shop phase in the loop — it still runs after
   a round win, before the next round begins.

4. **Target score scaling**
   `targetScore += 400` per round is a mutable number. It can scale faster or slower, scale
   based on purchased upgrades, or follow a formula. The win-check condition (`totalScore >= targetScore`)
   stays the same structurally.

5. **Coin reward formula**
   `earnedCoins = 3 + handsRemaining` is mutable. It can factor in round number, hand type
   used, or purchased bonuses. The phase that awards coins does not change position.

These are mutable because they change **numeric behavior or content**, not structural sequence.

---

## Step 4 — C++ Core Loop Skeleton

Below is the minimal OOP skeleton reflecting the invariant loop.

### File Structure

```
Task2/
├── main.cpp
├── Card.h
├── RunSession.h
├── RunSession.cpp
├── ScoringSystem.h
├── ShopSystem.h
├── ModifierFactory.h
└── modifiers/
    └── IModifier.h
```

### How to Build

```bash
g++ -std=c++17 main.cpp RunSession.cpp -o amazing_card
./amazing_card
```

### Explanation of Architectural Discipline

1. `RunSession` controls the phase order only — it does not contain scoring logic, modifier
   logic, or shop logic.
2. Hand evaluation is delegated entirely to `ScoringSystem`.
3. Score modification is handled by the `IModifier` chain — `RunSession` just iterates it.
4. Shop logic is encapsulated inside `ShopSystem` — `RunSession` only calls `enterShop()`.
5. If I want to change the scoring formula for a Straight, I only modify `StraightStrategy`.
6. If I want to add a new modifier type (e.g., suit-based bonus), I create a new class
   implementing `IModifier` — `RunSession` does not change.
7. If I want to change shop prices or add new items, I only modify `ShopSystem`.

The invariant loop remains stable across all of these changes.

---

## Final Reflection

### 1. What is the invariant structure of your game?

The invariant is the **round phase order** enforced inside `RunSession`:

**Fill Hand → Player Selects → Evaluate Hand → Apply Modifiers → Accumulate Score →
Check Win/Lose → Shop (if win) → Advance Round → Repeat**

This sequence must never change. It is the structural skeleton that every round follows.
If any phase is reordered, the game state becomes inconsistent — modifiers would fire on
non-existent scores, shops would open at wrong times, and win conditions would be checked
before damage is applied.

### 2. What parts are mutable?

Everything that changes **what** happens without changing **when** it happens is mutable:
scoring formulas per hand type, modifier effects, shop item catalog and prices, coin reward
amounts, target score scaling, and the number of starting hands or discards. All of these
live inside their respective classes (`ScoringSystem`, `IModifier` subclasses, `ShopSystem`)
and can be swapped or modified without touching `RunSession`.

### 3. If you wanted to add a new feature, which class would change?

- **New hand type** (e.g., Five of a Kind): add a new `IScoringStrategy` subclass and
  register it in `ScoringSystem::evaluateHand()`. `RunSession` does not change.
- **New modifier** (e.g., suit bonus): add a new `IModifier` subclass and expose it in
  `ModifierFactory`. `RunSession` does not change.
- **New shop item**: modify `ShopSystem::visitShop()` only.
- **New coin formula**: modify the reward block inside `startRun()` — the phase position
  stays the same, only the arithmetic changes.

### 4. If you changed the loop order, what would break?

- Moving **modifier application before scoring**: modifiers would receive 0 or garbage as
  input because `evaluateHand()` hasn't run yet.
- Moving **shop before win-check**: player could buy upgrades even after losing, which makes
  no gameplay sense.
- Moving **score accumulation after win-check**: the win condition fires before the last
  hand's score is counted — the player could win without their final hand being recorded.
- Moving **fillHand after player selection**: the player selects from an empty or stale hand,
  breaking input entirely.

Architecture protects the rhythm. The invariant ensures the game always makes sense.
