Kumpulan eksperimen dan kode C++ buat belajar dan iseng-iseng. Beberapa mungkin belum selesai—mohon maklum ya! Kalau mainan sama gambar PNG, pastikan udah install libpng sesuai sistem operasimu.

## 🎨 Proyek Terbaru
Prime Art – visualisasi bilangan prima dalam bentuk spiral:
    1. ![](prime-spiral-100-100.png)
    2. ![](prime-spiral-250-250.png)
    3. ![](prime-spiral-500-500.png)
    4. ![](prime-spiral-1000-1000.png)

## 📂 Isi Folder
```
.
├── .github/
│   └── workflows/
├── 3D/
├── NN/
├── basic/
├── electrical/
│   └── component/
├── image/
├── number_system/
├── .gitignore
├── CMakeLists.txt
├── LICENSE
├── init-cmake.sh
├── prime-spiral-100-100.png
├── prime-spiral-250-250.png
├── prime-spiral-500-500.png
└── prime-spiral-1000-1000.png
```

`3D/` – coba-coba grafik 3D
`NN/` – bikin neural network sendiri dari nol
`basic/` – eksperimen dasar dan syntax C++
`electrical/` – simulasi komponen elektronik
`component/include/` – file header komponen
`image/` – utak-atik gambar
`number_system/` – sistem bilangan

⚙️ Cara Kompilasi
Yang dibutuhin:
- Compiler C++20 (g++ atau clang++)
- CMake (opsional)
- libpng (buat proyek gambar)

```bash
g++ -std=gnu++20 ...
```
atau
```bash
clang++ -std=c++20 ...
```
atau pake CMake:
```bash
cmake -S . -B build
cmake --build build
```

## 🤝 Mau Ikut Kontribusi?
Silakan banget! Tapi santai aja juga nggak apa-apa. Proyek ini buat latihan dan seneng-seneng aja.

## 📜 Lisensi
Proyek ini pakai GPL v3 License. Baca selengkapnya [di sini](LICENSE).
