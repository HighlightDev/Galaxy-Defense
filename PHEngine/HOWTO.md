# PHEngine / GalaxyDefense — Build HOWTO

How to build the engine (`PHEngine`), the game (`GalaxyDefense`), and the test
scene (`EngineTest`) on **Linux** and **Windows**.

The build is CMake-based. Third-party libraries are shipped **prebuilt** inside
`PHEngine/Dependencies/` (one folder per platform: `lib/Unix/…` and
`lib/Win/x64/…` | `lib/Win/x86/…`), so you normally do **not** need to build
them yourself.

Game resources (shaders / models / textures / lua / config) are **copied 
automatically** into `<output>/res` as a post-build step.

---

## 1. Prerequisites

Common to both platforms:

- **CMake ≥ 3.16.3**
- A **C++20** compiler (GCC 13+, Clang, or MSVC v143 / Visual Studio 2022)
- An **OpenGL**-capable GPU + drivers

---

## 2. Build on Linux

### 2.1 Install system packages

The engine needs OpenGL, GLU, GLUT, X11, threads, and **OpenAL** development
headers/libs (OpenAL is resolved on Linux via `find_package(OpenAL)`):

```bash
sudo apt update
sudo apt install -y \
    build-essential cmake \
    libgl1-mesa-dev libglu1-mesa-dev freeglut3-dev \
    libopenal-dev libx11-dev
```

> If you build with `ENABLE_LIBUNWIND=ON` (the default), also install
> `libunwind-dev`. The commands below turn it **off**, matching CI.

### 2.2 Configure & build

From this directory (the one containing the top-level `CMakeLists.txt`):

```bash
cmake -B build -S . \
    -DBUILD_GALAXY_DEFENSE=ON \
    -DBUILD_ENGINE_TEST=ON \
    -DENABLE_LIBUNWIND=OFF \
    -DENABLE_PIC=ON

cmake --build build -j$(nproc)
```

### 2.3 Run

Resources are copied automatically next to the binary during the build.

```bash
cd build/GalaxyDefense
./GalaxyDefense
```

The test scene, if built: `build/EngineTest/EngineTest`.

> If you edit assets under `PHEngine/res/` after a build, you can re-sync them
> without a full rebuild using `./copy_resources.sh -a` (copies **a**ll; other
> flags: `-m` models, `-l` lua, `-s` shaders, `-t` textures, `-c` config).
> Note: `copy_resources.sh` has a hard-coded `PATH_TO_ROOT` — adjust it if your
> checkout lives elsewhere.

---

## 3. Build on Windows

Prebuilt Windows libs live in `PHEngine/Dependencies/*/lib/Win/x64/` (and
`x86/`). Use the **x64** architecture — that's what the shipped libs target.

### 3.1 Configure & build (Visual Studio 2022)

From this directory, in a *Developer* PowerShell / Command Prompt:

```bat
cmake -B build -S . -G "Visual Studio 17 2022" -A x64 ^
    -DBUILD_GALAXY_DEFENSE=ON ^
    -DBUILD_ENGINE_TEST=ON ^
    -DENABLE_LIBUNWIND=OFF

cmake --build build --config Debug
```

`ENABLE_LIBUNWIND` must be **OFF** on Windows (libunwind is Unix-only).
`/bigobj`, `/utf-8` and `/MP` are set automatically for MSVC.

### 3.2 Runtime DLLs

A post-build step **automatically copies** these DLLs next to the exe:

- `assimp-vc143-mtd.dll`
- `freetype.dll`
- `glew32.dll` (x64)
- `glfw3.dll`

### 3.3 ⚠️ Install the OpenAL DLL manually (required)

**`OpenAL32.dll` is NOT bundled and NOT copied by the build** — the repo ships
only the `OpenAL32.lib` import library. Without the DLL the game will fail to
start with a "OpenAL32.dll not found" error.

You must place `OpenAL32.dll` **in the same directory as `GalaxyDefense.exe`**:

```
build\GalaxyDefense\Debug\OpenAL32.dll   <-- put it here (next to the .exe)
```

Where to get it:

- Download **OpenAL Soft** (https://openal-soft.org/) — grab the Windows binary
  ZIP, take `bin\Win64\soft_oal.dll`, and **rename it to `OpenAL32.dll`**.
- Or install the official OpenAL runtime (`oalinst.exe`) and copy the
  `OpenAL32.dll` it installs into `System32`.

Use the **64-bit** DLL to match the x64 build. Do the same for the `EngineTest`
output directory if you run that target.

> If you also hit missing `lua54.dll` or `sndfile.dll` at runtime (these are not
> auto-copied for x64 either), copy them next to the exe the same way — from
> `PHEngine/Dependencies/Lua54/…` and `PHEngine/Dependencies/SndFile/…`, or a
> matching x64 build of each.

## 4. CMake options reference

| Option | Default | Purpose |
|---|---|---|
| `BUILD_GALAXY_DEFENSE` | ON | Build the game executable |
| `BUILD_ENGINE_TEST` | ON | Build the engine test scene |
| `BUILD_UBO_TEST` | OFF | Build the UBO test target |
| `ENABLE_LIBUNWIND` | ON | Compile libunwind-backed stack tracing (Linux only — set **OFF** on Windows) |
| `ENABLE_PIC` | OFF | Position-independent code |
| `DEBUG_BUILD` | ON | Define `DEBUG` (debug logging etc.) |
| `ENABLE_ASAN` | OFF | Address + UB sanitizer |
| `ENABLE_TSAN` | OFF | Thread sanitizer (ignored if ASAN on) |
| `PROFILER_BUILD` | OFF | Debug symbols, no optimizations |

Example — sanitized Linux debug build:

```bash
cmake -B build -S . -DENABLE_ASAN=ON -DENABLE_LIBUNWIND=OFF
cmake --build build -j$(nproc)
```

---