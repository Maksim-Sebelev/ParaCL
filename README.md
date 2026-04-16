# Компилятор ParaCL

## Зависимости

## ОС

Ubuntu 24.04.3 LTS\
По крайней мере на ней точно работает :) Можете попытаться собрать это в других местах, но разработчики ничего не гарантируют.

## Компилятор

`clang++-17`/`clang++-18`

```shell
sudo apt install -y clang++
```

## Система сборки

`Ninja-1.11.1`

```shell
sudo apt install ninja-build
```

## Сторонние зависимости

[Boost-1.70.0+](https://www.boost.org/releases/1.70.0/)

```shell
sudo apt install -y libboost-all-dev
```

[LLVM-17.0.1](https://github.com/llvm/llvm-project/releases/tag/llvmorg-17.0.1)

*Идет вместе с Ubuntu 24.04.3 LTS*

[Bison-3.8.2](https://ftp.gnu.org/gnu/bison/)

```shell
sudo apt install bison
```

[Flex-2.6.4](https://sourceforge.net/projects/gnuwin32/files/flex/)

```shell
sudo apt install flex
```

## Установка

```shell
git clone https://github.com/Maksim-Sebelev/ParaCL.git;
cd ParaCL;
cmake -S . -B build;
cmake --build build;
```

## Использование

```shell
build/paracl <source>.cl [ options ];
./executable;
```

## Тестирование

```shell
ctest --test-dir build;
```

## О языке

Данный компилятор строит AST и переводит его в llvm ir, бекенд - полностью clang

## Что поддерживает язык

все переменные одного типа - `int`\
области видимости\
арифметические операторы `+`, `-`, `*`, `%`, `/`, `+=`, `-=`, `*=`, `/=`, `%=`\
логические операторы: `&&`, `and`, `||`, `or`, `!`, `not`\
так же `+`, `-` могут быть унарными операторами\
цикл `while`\
условные операторы `if`-`else if`-`else`\
функции - всегда возвращают `int`\
оператор ввода с stdin - `?`\
оператор ввывод в stdout - `print`\
`print` принимает аргументы через запятую - строковые константы - `"<text>"` и переменные. в конце всегда печатается перенос строки\
одностровные комментарии - `//`\
многостровные комментари - `/* <text> */`\
`#!/path/to/paracl` - shebang

Я ParaCL использование имен **функций** (на переменные такого ограничения нет) начинающихся с `__` является UB. Это связано с наличием перегрузки функций и соотвественно необходимостью манглировать их имена.
