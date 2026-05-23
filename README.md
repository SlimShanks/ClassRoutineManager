# Class Routine Manager

Desktop app for managing class schedules — C++, Qt 6, SQLite.

## First time setup

**Mac:**
```bash
git clone git@github.com:SlimShanks/ClassRoutineManager.git
cd ClassRoutineManager
./setup-mac.sh
```

**Windows (run PowerShell as Administrator):**
```powershell
git clone git@github.com:SlimShanks/ClassRoutineManager.git
cd ClassRoutineManager
.\setup-windows.ps1
```

## Build and Run
```bash
cmake -B build
cmake --build build

# Mac
./build/ClassRoutineManager

# Windows
.\build\ClassRoutineManager.exe
```

## Tech Stack
- C++17
- Qt 6 (Widgets, SQL)
- SQLite
- CMake
