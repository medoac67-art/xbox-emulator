# Xbox Multi-Generational Emulator (C++20)
A lightweight, high-performance emulator host supporting Xbox Original, 360, One, and Series.

## Build Instructions (Windows 10/11)
1. Open Visual Studio 2022 command prompt (x64 Native Tools).
2. Run: `cmake -B build -G "Visual Studio 17 2022" -A x64`
3. Build: `cmake --build build --config Release`
4. The executable will be located in `build/Release/XboxEmulator.exe`.
