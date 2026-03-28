# Task 2 — ALCHEMY LAB (Balatro-Like Run)

**Mata Kuliah: Design Pattern for Games**

---

## Cara Build & Jalankan

```bash
cd src
g++ -std=c++17 main.cpp RunSession.cpp -o alchemylab
./alchemylab
```

## Struktur File

```
Task2/
├── src/
│   ├── main.cpp
│   ├── RunSession.h / RunSession.cpp   ← pengontrol invariant loop
│   ├── Reagent.h                       ← struct data reagen (tidak butuh interface)
│   ├── ScoringSystem.h                 ← IScoringStrategy + 8 reaksi kimia
│   ├── ShopSystem.h                    ← toko katalis (konkret)
│   ├── ModifierFactory.h               ← Factory Pattern (konkret)
│   ├── IRewardCalculator.h             ← interface reward koin (mutable)
│   ├── ITargetScaler.h                 ← interface skala target (mutable)
│   └── modifiers/
│       └── IModifier.h                 ← interface + 4 katalis konkret
```

---

## Keputusan Penggunaan Interface

| Komponen            | Interface? | Alasan                                                                                               |
| ------------------- | ---------- | ---------------------------------------------------------------------------------------------------- |
| `IModifier`         | ✅ Ya      | `RunSession` mengiterasi `vector<IModifier*>` polimorfis. Katalis baru ditambah tanpa ubah game loop |
| `IScoringStrategy`  | ✅ Ya      | 8 reaksi kimia berbeda dievaluasi polimorfis. Reaksi baru = tambah class baru saja                   |
| `IRewardCalculator` | ✅ Ya      | Formula reward koin bersifat mutable. Diinjeksikan ke `RunSession`                                   |
| `ITargetScaler`     | ✅ Ya      | Skala target skor bersifat mutable. Bisa flat, eksponensial, dsb                                     |
| `ScoringSystem`     | ❌ Tidak   | Hanya satu sistem evaluasi, tidak perlu diganti polimorfis                                           |
| `ShopSystem`        | ❌ Tidak   | Hanya satu implementasi toko                                                                         |
| `ModifierFactory`   | ❌ Tidak   | Static factory, tidak ada polimorfisme dibutuhkan                                                    |
| `Reagent`           | ❌ Tidak   | Struct data murni, bukan behavior                                                                    |

---

## Step 1 — Core Loop

Game ini adalah **ALCHEMY LAB** — permainan reaksi kimia berbasis run.
Pemain memilih kombinasi reagen (H, O, C, N, Fe, S, Ca, Na) untuk menciptakan
reaksi kimia yang menghasilkan skor, selama 3 ronde dengan target meningkat.

Core loop per ronde:

1. Pemain menerima 7 reagen di meja lab
2. Pemain memilih reagen untuk direaksikan atau dibuang
3. Sistem mengevaluasi kombinasi reagen → menentukan reaksi kimia
4. Base score dihitung berdasarkan jenis reaksi
5. Katalis aktif diterapkan ke skor secara berantai
6. Skor diakumulasi ke total
7. Cek kondisi menang/kalah ronde
8. Pemain mengunjungi toko dan bisa membeli katalis
9. Ulangi untuk ronde berikutnya dengan target lebih tinggi

---

## Step 2 — Identifikasi Invariant

Urutan berikut **tidak boleh berubah**:

1. Isi meja lab (fill lab)
2. Pemain memilih reagen
3. Evaluasi reaksi → hitung base score
4. Terapkan rantai katalis ke skor
5. Akumulasi skor, kurangi slot percobaan/discard
6. Cek kondisi menang/kalah
7. Fase toko (hanya jika menang)
8. Lanjut ke ronde berikutnya

Jika urutan ini berubah, logika game rusak:

1. Katalis diterapkan **sebelum** evaluasi → base score belum ada, katalis hasilkan nilai salah
2. Toko dibuka **sebelum** cek menang → pemain beli katalis meski sudah kalah
3. Akumulasi skor **setelah** cek menang → skor reaksi terakhir tidak terhitung
4. Isi reagen **setelah** pemilihan → pemain memilih dari meja kosong

**Komponen yang wajib ada:**

1. `RunSession` — pengontrol loop dan urutan fase
2. `Reagent` — unit data reagen kimia
3. `ScoringSystem` — mengevaluasi reaksi dan menghitung base score
4. `IModifier` — interface untuk semua katalis
5. `ShopSystem` — pembelian katalis antar ronde

---

## Step 3 — Elemen Mutable

Berikut bagian yang bersifat mutable:

1. **Formula scoring per jenis reaksi**
   Setiap `IScoringStrategy` punya aturan dan bobot skor sendiri (H2O: +80, CO2: +100, Fe2O3: +200).
   Angka ini bisa diubah bebas tanpa memengaruhi urutan fase loop.

2. **Perilaku katalis**
   `PotencyBooster`, `ReactionAmplifier`, `ExothermicSurge`, dan `ElementBonusCatalyst` bisa
   diganti atau ditambah tipe baru. `RunSession` hanya memanggil `applyModifiers(score)` —
   game loop tidak berubah sama sekali saat katalis baru ditambah.

3. **Isi toko dan harga**
   `ShopSystem` bisa menawarkan katalis berbeda atau mengubah harga tanpa memengaruhi
   posisi fase toko dalam loop.

4. **Formula reward koin**
   `IRewardCalculator` diinjeksikan ke `RunSession`. Ganti ke `BonusRewardCalculator`
   di `main.cpp` tanpa menyentuh `RunSession`.

5. **Skala target skor**
   `ITargetScaler` diinjeksikan ke `RunSession`. Ganti ke `ExponentialTargetScaler`
   di `main.cpp` tanpa menyentuh `RunSession`.

Semua elemen ini mutable karena mengubah **isi atau nilai**, bukan **urutan struktural** fase.

---

## Step 4 — C++ Core Loop Skeleton

Berikut struktur minimal yang mencerminkan invariant loop.

### RunSession.h — skeleton pengontrol loop

```cpp
class RunSession {
private:
    // State per ronde
    int totalScore, targetScore;
    int attemptsRemaining, discardRemaining;
    int roundNumber, coins;

    vector<Reagent>    currentLab;       // reagen di tangan
    vector<IModifier*> activeModifiers;  // rantai katalis (polimorfis)

    ScoringSystem  scoringSystem;   // konkret
    ShopSystem     shopSystem;      // konkret
    IRewardCalculator* rewardCalc_; // mutable — diinjeksikan
    ITargetScaler*  targetScaler_;  // mutable — diinjeksikan

    void fillLab();
    void displayLab();
    int  applyModifiers(int score, int uniqueElements); // rantai katalis

public:
    RunSession(IRewardCalculator*, ITargetScaler*);
    void startRun();   // invariant loop
    void playRound();  // inner loop
    void enterShop();
};
```

### startRun() — invariant loop utama

```cpp
void RunSession::startRun() {
    for (int round = 0; round < 3; round++) {
        // Reset state
        totalScore = 0; attemptsRemaining = 4; discardRemaining = 3;

        // Phase 1-6: jalankan ronde
        playRound();

        if (totalScore >= targetScore) {
            // Phase 7: hitung reward (IRewardCalculator — mutable)
            int reward = rewardCalc_->computeReward(attemptsRemaining, roundNumber);
            coins += reward;

            // Phase 8: toko katalis
            enterShop();

            // Phase 9: advance ronde (ITargetScaler — mutable)
            roundNumber++;
            targetScore = targetScaler_->nextTarget(targetScore, roundNumber);
        } else {
            return; // game over
        }
    }
}
```

### playRound() — inner loop dengan fase terjaga

```cpp
void RunSession::playRound() {
    fillLab();  // Phase 1: isi meja lab

    while (attemptsRemaining > 0 && totalScore < targetScore) {
        displayLab();

        // Phase 2: pemain pilih reagen
        vector<Reagent> selected = getPlayerSelection();

        if (playerChoosesToReact) {
            // Phase 3: evaluasi reaksi (IScoringStrategy — mutable)
            string reactionName;
            int baseScore = scoringSystem.evaluateReaction(selected, reactionName);

            // Phase 4: terapkan katalis (IModifier chain — mutable)
            int finalScore = applyModifiers(baseScore, uniqueCount);

            // Phase 5: akumulasi skor
            totalScore += finalScore;
            attemptsRemaining--;
        }
        // Phase 6: cek menang/kalah dijaga oleh kondisi while
        fillLab(); // isi ulang reagen
    }
}
```

### applyModifiers() — rantai katalis terpisah dari loop

```cpp
// Game loop hanya memanggil applyModifiers() — tidak perlu diubah
// saat katalis baru ditambah. Cukup tambah class IModifier baru.
int RunSession::applyModifiers(int score, int uniqueElements) {
    for (auto mod : activeModifiers) {
        score = mod->applyModification(score);
    }
    return score;
}
```

### Cara menambah katalis baru (tanpa mengubah game loop)

```cpp
// 1. Buat class baru di modifiers/IModifier.h
class FrostCatalyst : public IModifier {
public:
    int applyModification(int score) override { return score + 100; }
    string getName() override { return "Frost Catalyst (+100)"; }
    string getDescription() override { return "Reaksi beku: tambah 100 poin"; }
    IModifier* clone() override { return new FrostCatalyst(); }
};

// 2. Daftarkan di ModifierFactory.h
case 7: return new FrostCatalyst();

// 3. Expose di ShopSystem.h
// Tambah item baru di catalog

// RunSession, playRound(), applyModifiers() TIDAK BERUBAH SAMA SEKALI
```

---

## Refleksi

### 1. Apa struktur invariant dalam game ini?

Struktur invariant adalah **urutan fase yang dijaga ketat di dalam `RunSession`**, yaitu:

```
Isi Lab → Pilih Reagen → Evaluasi Reaksi → Terapkan Katalis →
Akumulasi Skor → Cek Menang/Kalah → Toko (jika menang) → Lanjut Ronde → Ulangi
```

Urutan ini tidak boleh diubah dalam kondisi apapun. `RunSession` hanya bertugas menjaga
ritme urutan ini — ia tidak menyimpan logika evaluasi reaksi, tidak menghitung efek
katalis, dan tidak menentukan isi toko. Semua itu didelegasikan ke kelas lain.

Alasan urutan ini bersifat invariant adalah karena setiap fase bergantung pada hasil
fase sebelumnya. Katalis hanya bisa diterapkan setelah base score ada. Toko hanya boleh
dibuka setelah kondisi menang dikonfirmasi. Skor hanya bisa diakumulasi setelah katalis
selesai diterapkan. Jika satu fase digeser, game state menjadi tidak konsisten dan hasilnya
tidak bisa diprediksi.

### 2. Bagian mana yang bersifat mutable?

Semua yang mengubah **apa** yang terjadi tanpa mengubah **kapan** terjadi adalah mutable.
Dalam ALCHEMY LAB, bagian mutable meliputi: formula scoring setiap reaksi kimia (nilai
H2O, CO2, Fe2O3 bisa diubah di `IScoringStrategy`), efek setiap katalis (PotencyBooster,
ReactionAmplifier, ExothermicSurge bisa diganti di `IModifier`), katalog dan harga toko
(diubah di `ShopSystem` saja), formula reward koin (diubah via `IRewardCalculator` yang
diinjeksikan dari `main.cpp`), dan skala kenaikan target skor (diubah via `ITargetScaler`).

Semua ini hidup di dalam kelas masing-masing dan tidak menyentuh `RunSession` saat diubah.
Ini adalah bukti nyata bahwa invariant dan mutable sudah dipisahkan dengan benar.

### 3. Jika ingin menambah fitur baru, kelas mana yang berubah?

Jika ingin menambah **reaksi kimia baru** (misalnya H2SO4), tambah subkelas `IScoringStrategy`
baru dan daftarkan di `ScoringSystem::evaluateReaction()`. `RunSession` tidak berubah.
Jika ingin menambah **katalis baru** (misalnya FrostCatalyst), tambah subkelas `IModifier`
baru, daftarkan di `ModifierFactory`, dan expose di `ShopSystem`. `RunSession` dan
`applyModifiers()` tidak berubah sama sekali — karena loop hanya memanggil
`mod->applyModification(score)` secara polimorfis. Jika ingin mengubah **formula reward**,
ganti implementasi `IRewardCalculator` di `main.cpp`. Jika ingin mengubah **skala target**,
ganti implementasi `ITargetScaler` di `main.cpp`.

### 4. Jika urutan loop diubah, apa yang akan rusak?

Memindahkan **penerapan katalis sebelum evaluasi reaksi** akan membuat katalis menerima
nilai 0 karena `evaluateReaction()` belum dipanggil. Memindahkan **toko sebelum cek menang**
akan memungkinkan pemain membeli katalis meski gagal memenuhi target — ini merusak
keseimbangan ekonomi game. Memindahkan **akumulasi skor setelah cek menang** akan membuat
kondisi menang terpicu sebelum skor reaksi terakhir dihitung, sehingga pemain bisa menang
tanpa reaksi terakhirnya tercatat. Memindahkan **fill lab setelah pemilihan reagen** akan
membuat pemain memilih dari meja yang kosong atau berisi reagen sisa ronde sebelumnya.

**Arsitektur melindungi ritme permainan. Invariant memastikan game selalu masuk akal.**
