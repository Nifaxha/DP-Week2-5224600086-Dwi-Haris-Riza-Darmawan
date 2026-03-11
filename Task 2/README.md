# Task 2 — Amazing Card (Balatro-Like Run)
**Mata Kuliah: Design Pattern for Games**

---

## Cara Build & Jalankan

```bash
g++ -std=c++17 main.cpp RunSession.cpp -o amazing_card
./amazing_card
```

## Struktur File

```
Task2/
├── main.cpp
├── Card.h                          ← struct data kartu (tidak butuh interface)
├── RunSession.h / RunSession.cpp   ← pengontrol invariant loop
├── ScoringSystem.h                 ← IScoringStrategy (interface) + 8 strategi konkret
├── ShopSystem.h                    ← kelas konkret (tidak butuh interface)
├── ModifierFactory.h               ← static factory (tidak butuh interface)
└── modifiers/
    └── IModifier.h                 ← interface + FlatBonusModifier + MultiplierModifier
```

---

## Keputusan Penggunaan Interface

| Komponen | Interface? | Alasan |
|---|---|---|
| `IModifier` | ✅ Ya | `RunSession` mengiterasi `vector<IModifier*>` secara polimorfis saat runtime. Modifier baru bisa ditambah tanpa mengubah game loop sama sekali |
| `IScoringStrategy` | ✅ Ya | Ada 8 strategi evaluasi tangan yang dievaluasi polimorfis dari terkuat ke terlemah. Menambah tipe tangan baru cukup dengan membuat class baru |
| `ScoringSystem` | ❌ Tidak | Hanya satu sistem scoring; `RunSession` tidak perlu menggantinya secara polimorfis |
| `ShopSystem` | ❌ Tidak | Hanya satu implementasi toko |
| `ModifierFactory` | ❌ Tidak | Static factory — tidak ada polimorfisme yang dibutuhkan |
| `Card` | ❌ Tidak | Struct data murni, bukan behavior |

---

## Step 1 — Core Loop

Game ini adalah permainan kartu bergaya Balatro di mana pemain memainkan kombinasi kartu
poker selama 3 ronde untuk mencapai target skor.

Core loop per ronde:

1. Pemain menerima 7 kartu
2. Pemain memilih kartu untuk dimainkan atau dibuang
3. Sistem mengevaluasi kombinasi tangan (poker hand)
4. Base score dihitung berdasarkan jenis tangan
5. Modifier aktif diterapkan ke skor secara berantai
6. Skor diakumulasi ke total
7. Cek kondisi menang/kalah ronde
8. Pemain mengunjungi toko dan bisa membeli modifier
9. Ulangi untuk ronde berikutnya dengan target lebih tinggi

---

## Step 2 — Identifikasi Invariant

Urutan berikut **tidak boleh berubah**:

1. Isi kartu (fill hand)
2. Pemain memilih kartu
3. Evaluasi tangan → hitung base score
4. Terapkan modifier ke skor
5. Akumulasi skor, kurangi slot tangan/discard
6. Cek kondisi menang/kalah ronde
7. Fase toko (hanya jika menang)
8. Lanjut ke ronde berikutnya

Jika urutan ini berubah, logika game akan rusak. Contohnya:

1. Jika modifier diterapkan **sebelum** scoring, base score belum ada — modifier menghasilkan nilai salah.
2. Jika toko dibuka **sebelum** cek menang/kalah, pemain bisa beli modifier meski sudah kalah.
3. Jika akumulasi skor dilakukan **setelah** cek menang, sistem tidak mendeteksi apakah target tercapai di tangan terakhir.
4. Jika kartu diisi **setelah** pemain memilih, pemain memilih dari tangan kosong.

**Komponen yang wajib ada:**
1. `RunSession` — pengontrol loop dan urutan fase
2. `Card` — unit data kartu
3. `ScoringSystem` — mengevaluasi tangan dan menghitung base score
4. `IModifier` — interface untuk semua modifier skor
5. `ShopSystem` — pembelian modifier antar ronde

---

## Step 3 — Elemen Mutable

Berikut bagian-bagian yang bersifat mutable:

1. **Formula scoring per jenis tangan**
   Setiap kelas strategi (`PairStrategy`, `StraightStrategy`, dst.) punya formulanya sendiri.
   Angka-angka ini bisa diubah bebas tanpa memengaruhi urutan fase. Loop tetap memanggil
   `evaluateHand()` di posisi yang sama.

2. **Perilaku modifier**
   `FlatBonusModifier` dan `MultiplierModifier` bisa diganti atau ditambah dengan tipe baru
   (misalnya modifier yang hanya aktif untuk kartu Heart). `RunSession` hanya memanggil
   `applyModifiers(score)` — **game loop tidak berubah sama sekali** saat modifier baru ditambah.

3. **Isi toko dan harga**
   `ShopSystem` bisa menawarkan modifier berbeda, mengubah harga, atau menambah item baru.
   Posisi fase toko dalam loop tidak berubah.

4. **Skala target skor**
   `targetScore += 400` per ronde adalah angka yang bisa diganti. Kondisi cek menang
   (`totalScore >= targetScore`) tetap struktural sama.

5. **Formula reward koin**
   `earnedCoins = 3 + handsRemaining` bisa diubah formulanya. Posisi fase pemberian koin
   tidak bergerak.

Semua elemen ini bersifat mutable karena mengubah **perilaku numerik atau konten**, bukan **urutan struktural**.

---

## Cara Menambah Modifier Baru (Tanpa Mengubah Game Loop)

Ini adalah bukti bahwa game loop benar-benar terlindungi:

**1. Buat class baru yang implement `IModifier`:**
```cpp
// modifiers/SuitBonusModifier.h
class SuitBonusModifier : public IModifier {
public:
    int applyModification(int score) override { return score + 75; }
    string getName() override { return "Suit Bonus (+75)"; }
};
```

**2. Daftarkan di `ModifierFactory`:**
```cpp
case 4: return new SuitBonusModifier();
```

**3. Expose di `ShopSystem`:**
```cpp
cout << "3. Suit Bonus (+75 Score) - 5 Coins\n";
```

**`RunSession` dan game loop (`playHand`) tidak berubah sama sekali.**
Baris `score = applyModifiers(score)` sudah menangani semua modifier secara otomatis.

---

## Refleksi

### 1. Apa struktur invariant dalam game ini?

Struktur invariant adalah **urutan fase ronde yang dijaga di dalam `RunSession`**:

```
Isi Kartu → Pilih Kartu → Evaluasi Tangan → Terapkan Modifier →
Akumulasi Skor → Cek Menang/Kalah → Toko (jika menang) → Lanjut Ronde → Ulangi
```

Urutan ini tidak boleh berubah. Jika ada fase yang digeser, game state menjadi tidak
konsisten — modifier diterapkan ke skor yang belum ada, toko dibuka di waktu yang salah,
atau kondisi menang diperiksa sebelum skor terakhir dihitung.

### 2. Bagian mana yang bersifat mutable?

Semua yang mengubah **apa** yang terjadi tanpa mengubah **kapan** terjadi adalah mutable:
formula scoring per jenis tangan, efek modifier, katalog dan harga toko, jumlah reward koin,
skala target skor, dan jumlah tangan atau discard awal. Semua ini hidup di dalam kelas
masing-masing (`ScoringSystem`, subkelas `IModifier`, `ShopSystem`) dan bisa diganti tanpa
menyentuh `RunSession`.

### 3. Jika ingin menambah fitur baru, kelas mana yang berubah?

- **Tipe tangan baru** (misal, Five of a Kind): tambah subkelas `IScoringStrategy` baru dan
  daftarkan di `ScoringSystem::evaluateHand()`. `RunSession` tidak berubah.
- **Modifier baru** (misal, bonus suit): tambah subkelas `IModifier` baru dan expose melalui
  `ModifierFactory`. `RunSession` tidak berubah.
- **Item toko baru**: ubah `ShopSystem::visitShop()` saja.
- **Formula koin baru**: ubah aritmatika di dalam blok reward di `startRun()` — posisi fase
  tetap sama.

### 4. Jika urutan loop diubah, apa yang akan rusak?

- Memindahkan **penerapan modifier sebelum scoring**: modifier menerima nilai 0 karena
  `evaluateHand()` belum dijalankan.
- Memindahkan **toko sebelum cek menang**: pemain bisa membeli upgrade meski sudah kalah.
- Memindahkan **akumulasi skor setelah cek menang**: kondisi menang terpicu sebelum skor
  tangan terakhir dihitung — pemain bisa menang tanpa tangan terakhirnya tercatat.
- Memindahkan **fillHand setelah pemilihan kartu**: pemain memilih dari tangan kosong atau
  tangan lama, merusak input sepenuhnya.

**Arsitektur melindungi ritme permainan.**
