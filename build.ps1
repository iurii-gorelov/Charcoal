$files = (Get-ChildItem -Recurse -Filter "*.cpp" | ForEach-Object { $_.FullName })
g++ -std=c++17 -Ilibraries -Isource -o main.exe $files -lgdi32 -w
if ($?) { Start-Process ./main.exe }