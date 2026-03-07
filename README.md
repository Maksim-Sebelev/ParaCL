# Компилятор и интепретор ParaCL

## Зависимости

[Boost](https://www.boost.org/)\
[LLVM](https://www.llvm.org/)

## Установка

```shell
git clone https://github.com/Maksim-Sebelev/ParaCL.git;
cd ParaCL;
cmake -S . -B build;
cmake --build build;
```

## Использование

компилятора:

```shell
build/paraclc <source>.cl [ -o <executbale> ];
./executable;
```

Использование интепретатора:

```shell
build/paracli <source>.cl
```

## Тестирование

```shell
ctest --test-dir build;
```

Если нужен полный отчет о каждом тесте:

```shell
ctest --test-dir build/subprojects/Compiler;
ctest --test-dir build/subprojects/Interpreter;
```

## Бенчмарк

```shell
build/benchmark
```

## О языке

Компилятор и интерпретатор, по крайней мере по ожиданиям разработчиков, ведут себя совершенно одинаково.

## Что поддерживает язык

1 единственный тип для переменных - `int`\
области видимости\
арифметические операторы `+`, `-`, `*`, `%`, `/`, `+=`, `-=`, `*=`, `/=`, `%=`\
логические операторы: `&&`, `and`, `||`, `or`, `!`, `not`\
так же `+`, `-` могут быть унарными операторами\
цикл `while`\
условные операторы `if`-`else if`-`else`\
оператор ввода с stdin - `?`\
оператор ввывод в stdout - `print`\
`print` принимает аргументы через запятую - строковые константы - `"<text>"` и переменные. в конце всегда печатается перенос строки\
одностровные комментарии - `//`\
многостровные комментари - `/* <text> */`\
`#!/path/to/paracl` - shebang
