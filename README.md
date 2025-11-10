# ParaCL (младший брат С, калека, интерпретируемый язык программирования)

![C++](https://img.shields.io/badge/C++-23-blue?style=for-the-badge&logo=cplusplus)
![CMake](https://img.shields.io/badge/CMake-3.17+-green?style=for-the-badge&logo=cmake)
![Testing](https://img.shields.io/badge/Google_Test-Framework-red?style=for-the-badge&logo=google)

# По поводу Windows и MSVC
![нельзя](./assets/no-msvc.PNG)

# Зависимости:
[flex](https://github.com/westes/flex)\
[bison]()
 Зависимости есть для unit-тестирования и некоторых дебажных функций, об этом читайте далее.\
Однако для удобной работы понадобятся следующие инструменты:

## Для комплиляции:
[clang++](https://clang.org/)\
[g++](https://gcc.gnu.org/)\
`clang` рекомендуется для данного проекта, так как на момент написания проекта он лучше умеет работать с `.cppm` модулями.

## Для сборки:
[cmake](https://cmake.org/ "ссылка на официальный сайт cmake")\
[ninja](https://ninja-build.org "ссылка на оффициальный сайт ninja")\
`Ninja` рекомендуется для данного проекта, так как в большистве пакетных менеджеров поставляется версия, для которой `cmake` умеет работать с модулями (`.cppm`).

Вот что про это говорит сам `cmake`:
```bash
Modules are supported only by Ninja, Ninja Multi-Config, and Visual Studio
generators for VS 17.4 and newer.  See the cmake-cxxmodules(7) manual for
details.  Use the CMAKE_CXX_SCAN_FOR_MODULES variable to enable or disable
scanning.
```


Тот самый младший брат языка C78, который только только начинает свой путь.  

```bash
cd front
bison -d src/parser.y -o src/parser.tab.cpp
flex -o src/lexer.yy.cpp src/lexer.l
g++ src/main.cpp src/lexer.yy.cpp src/parser.tab.cpp -Iinc -o out
```
