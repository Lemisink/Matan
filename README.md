# Build & Run

## Requirements
- CMake ≥ 3.16, Ninja (recommended) or MSBuild/Make.
- C++20 compiler (GCC/Clang on Linux, MSVC 2022 on Windows).
- OpenMP is optional: if found, differentiation will be parallelized automatically.
- UI is built manually: Node.js + npm, Rust toolchain (stable), WebView2 Runtime (Windows), Tauri deps.

## C++ core only
UI is not wired into CMake; it builds only the code in `core/`.

### Linux
```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
./dist/bin/matan_app            # run
```

### Windows (MSVC)
In “x64 Native Tools for VS 2022”:
```bat
cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
cmake --build build
dist\bin\matan_app.exe          # run
```

## UI (Tauri) — manual build
Run separately from CMake:
```bash
cd apps/desktop
npm install
# Linux: AppImage
npm run tauri build -- --bundles appimage
# Windows: NSIS (or MSI if preferred)
npm run tauri build -- --bundles nsis
```
Artifacts end up in `apps/desktop/src-tauri/target/release/bundle/<format>/`.

## Ready-made builds
Prebuilt images (AppImage/NSIS, etc.) are placed in the `done/` folder.
