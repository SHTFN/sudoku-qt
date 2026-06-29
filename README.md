# Qt

Текущий статус тестирования GitHub Actions: [![CI/CD](../../actions/workflows/classroom.yml/badge.svg?branch=main&event=workflow_dispatch)](../../actions/workflows/classroom.yml).

## Структура проекта

Лабораторная работа состоит из **двух** частей: *проект* и набор *тестов* ([GoogleTest](https://github.com/google/googletest)). Проект состоит из [`заголовочных файлов`](./include/) и [`файлов с исходным кодом`](./src/), включая [`main.cpp`](src/main.cpp), содержащий точку входа программы. Unit-тесты для тестирования проекта располагаются в директории [`test/`](./test/), точка входа для запуска тестов располагается в файле [`tests.cpp`](test/tests.cpp).

| Директория             | Назначение                                                   |
| ---------------------- | ------------------------------------------------------------ |
| [`include/`](include/) | общие заголовочные файлы проекта (API или интерфейс проекта) |
| [`src/`](src/)         | исходные файлы проекта                                       |
| [`test/`](test/)       | вспомогательные и исходные файлы с тестами                   |

Вам необходимо настроить [QMake](https://doc.qt.io/qt-5/qmake-manual.html) [проект](qt.pro).

*Примечание*. [Файл ресурсов](https://doc.qt.io/qt-5/resources.html) следует разместить в *корне* репозитория.

## Конфигурация и сборка проекта

[Конфигурация](https://doc.qt.io/qt-5/qmake-variable-reference.html#config) выбранной цели на сборку (тесты или основное решение).

```cmd
qmake qt.pro CONFIG+=<build_type> CONFIG+=<sub_target> GTEST_ROOT=<path/to/googletest>
```

где:

* `build_type` - тип сборки (`debug` или `release`)
* `sub_target` - это либо `solution` (GUI), либо `tests` (тесты)
* `GTEST_ROOT` *(необязателен в `solution`)* - *корень* библиотеки GoogleTest:

  * заголовочные файлы лежат в `<path/to/googletest>/include`
  * библиотека лежит в `<path/to/googletest>/lib`

Ключ `CONFIG+=<build_type>` *необязателен* на Windows, см. ниже.

### Windows

На Windows по умолчанию генерируются *Makefile*'ы на Release и Debug сборки.

Сборка:

```powershell
nmake -f Makefile.Debug
# если Debug-сборка 

nmake -f Makefile.Release
# если Release-сборка
```

Запуск:

* `.\debug\qt.exe` - если Debug-сборка
* `.\release\qt.exe` - если Release-сборка

### Linux/MacOS

На Linux/MacOS генерируется *Makefile* заданной `CONFIG+=<build_type>` типа сборки.

Сборка и запуск:

```bash
make && ./qt
```
