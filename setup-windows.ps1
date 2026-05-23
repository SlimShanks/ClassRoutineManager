Write-Host "Installing Qt, CMake, Ninja..." -ForegroundColor Green

# Install winget packages
winget install -e --id Kitware.CMake
winget install -e --id Ninja-build.Ninja
winget install -e --id Qt.QtCreator

Write-Host "Done! Now run:" -ForegroundColor Green
Write-Host "cmake -B build" -ForegroundColor Yellow
Write-Host "cmake --build build" -ForegroundColor Yellow
