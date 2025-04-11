# Tugas Kecil 2 IF2211 Strategi Algoritma
## Penyelesaian Word Search Puzzle dengan Algoritma Branch and Bound

## 📌 Deskripsi Singkat
Program ini menyelesaikan masalah pencarian kata pada Word Search Puzzle menggunakan algoritma Branch and Bound. Algoritma ini memastikan efisiensi dengan memotong cabang pencarian yang tidak relevan, sehingga mempercepat proses pencarian kata.

## 🗂️ Struktur Direktori

```
Tucil2_13523009/
├── bin/         
├── doc/           
│   └── Tucil2_13523009.pdf
├── images/            # if you want to test the program
│   ├── input/
│   └── output/    
├── src/     
│   ├── external/
│   │   ├── gif.hpp
│   │   └── stb_image.hpp
│   ├── include/                  # header files
│   │    ├── QuadtreeNode.hpp
│   │    ├── ErrorMeasurement.hpp
│   │    ├── ImageProcessor.hpp
│   │    ├── QuadtreeCompressor.hpp
│   │    ├── GifGenerator.hpp      
│   │    └── Utils.hpp
│   ├── QuadtreeNode.cpp
│   ├── ErrorMeasurement.cpp
│   ├── ImageProcessor.cpp
│   ├── QuadtreeCompressor.cpp
│   ├── GifGenerator.cpp      
│   ├── Utils.cpp
│   └── main.cpp 
├── test/         
│   ├── input/
│   └── output/
├── .gitignore     
└── README.md      
```

## ⚙️ Cara Kompilasi

Untuk mengompilasi program, jalankan perintah berikut di terminal:

```bash
  g++ -std=c++17 -o bin/quadtree_compressor.exe src/*.cpp -Isrc/include -Isrc/external
```

## ▶️ Cara Menjalankan Program

Untuk menjalankan program, gunakan perintah berikut:

```bash
  ./bin/quadtree_compressor --input {FILE_INPUT_DIRECTORY} --error-method {METHOD} --threshold {VALUE} --min-block {SIZE} --output {FILE_OUTPUT_DIRECTORY}
```

### Penjelasan Argumen:
- `--input`: Path file gambar input.
- `--error-method`: Metode pengukuran error (contoh: `VAR`, `MAD`, `MPD`, `ENT`, `SSIM`).
- `--threshold`: Nilai ambang batas error.
- `--min-block`: Ukuran blok minimum.
- `--output`: Path file gambar output.

Contoh:
```bash
  ./bin/quadtree_compressor --input images/input/image.jpg --error-method VAR --threshold 10.0 --min-block 4 --output images/output/compressed_image.jpg
```

## 📄 Format File Input

Format file input berupa gambar dengan ekstensi berikut:
- `.jpg`
- `.jpeg`
- `.png`

## 🧾 Format Output

Output berupa gambar terkompresi dengan format yang sama seperti input. Jika format input tidak didukung, output akan disimpan dalam format `.png`.

Contoh output:
```
images/output/compressed_image.png
```

## 📈 Fitur Tambahan
- **GIF Animation**: Program dapat menghasilkan animasi GIF yang menunjukkan proses kompresi quadtree. Gunakan argumen `--gif` untuk menentukan path file GIF output.
- **Penyesuaian Threshold Otomatis**: Program dapat menyesuaikan threshold untuk mencapai rasio kompresi target menggunakan argumen `--target-ratio`.

## 👤 Author
- **Nama:** M Hazim R Prajoda
- **NIM:** 13523009