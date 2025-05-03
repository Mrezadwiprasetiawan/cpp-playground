🇺🇸 English

Just a bunch of C++ experiments and random code from my learning and tinkering. Some stuff might be unfinished—thanks for your patience! For anything with PNG images, make sure you've got libpng installed for your OS.

🎨 Latest Experiments

Prime Art – visualizing prime numbers in a spiral:

1. 100 × 100 


2. 250 × 250 


3. 500 × 500 


4. 1000 × 1000 




📂 What’s in the folders?

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

3D/ – messing around with 3D graphics

NN/ – homemade neural network stuff

basic/ – basic C++ tests and syntax

electrical/ – simulating electronic components

component/include/ – headers for those components


image/ – image editing or processing stuff

number_system/ – number systems, conversions, etc.


⚙️ How to Build

You’ll need:

A C++20 compiler (like g++ or clang++)

CMake (if you want)

libpng (for image projects)


g++ -std=gnu++20 ...

or

clang++ -std=c++20 ...

or with CMake:

cmake -S . -B build
cmake --build build

🤝 Wanna Help?

Feel free to pitch in! No pressure—it’s mainly just for fun and practice.

📜 License

This project uses the GPL v3 License. See LICENSE for details.
