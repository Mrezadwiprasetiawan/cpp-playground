# cpp-playground

Kumpulan eksperimen dan kode kode C++ hasil gabut dan latihan.
Beberapa masih belum selesai sih, tapi ya begini adanya. Untuk project image png perlu libpng, jadi install dulu sesuai OS masing masing

## Struktur Direktori

```
.
├── .github/
│   └── workflows/
├── 3D/
├── NN/
├── basic/
├── electrical/
│   └── component/
│       └── include/
├── image/
├── number_system/
├── .gitignore
├── CMakeLists.txt
├── LICENSE
├── output.png
├── output100-100.png
├── output1000-1000.png
└── struct.txt // ini sebenernya buat struktur directory, taoi karena udah ada README.md nanti yw hapus
```

## Deskripsi Folder

* `3D/`                     Eksperimen 3D
* `NN/`                     Implementasi neural network dari nol
* `basic/`                  sintaks dasar C++
* `electrical/`             Simulasi elemen elektronik nyata

  * `component/include/`    Header file untuk komponen elektronik
* `image/`                  Pengolahan dan manipulasi gambar
* `number_system/`          Sistem bilangan

## Menjalankan Contoh

Prerequisites:

* Compiler C++20 (g++, clang++)
* CMake (opsional)
* libpng untuk build projek image/PNG

Kompilasi dan cara jalanin:

```bash
  g++ -std=gnu++20 ...
```

Atau
```bash
  clang++ -std=c++20 ...
```

Atau pake CMake:

```bash
  cmake -S . -B build
  cmake --build build
```

## Kontribusi

Mau kontribusi silahkan, ga ya silahkan. Orang projek latihan doang

## Lisensi

GPL V3 License.![CEK DISINI](LICENSE).

