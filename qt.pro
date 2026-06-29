# Template.
TARGET = qt
TEMPLATE = app

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

# Configs.
unix {
  QMAKE_CXXFLAGS += -std=c++20
}

win32 {
  QMAKE_CXXFLAGS += /std:c++20
}

# Qt common modules.
QT += core

INCLUDEPATH += .
INCLUDEPATH += \
    include \
    include/core \
    include/game \
    include/solver \
    include/storage \
    include/ui

# Common headers.
#HEADERS += \
#  include/mainwindow.h

## Common sources.
#SOURCES += \
#  src/mainwindow.cpp

# Add resources, if you're really need that.
 RESOURCES += resources.qrc

tests {
  # Tests - is commandline tool.
  CONFIG += cmdline

  # TODO: Add specific headers/sources for tests.
  HEADERS +=
  SOURCES += \
    test/tests.cpp

  GTEST_INCLUDE = $$GTEST_ROOT/include
  GTEST_LIBPATH = $$GTEST_ROOT/lib

  INCLUDEPATH += $$GTEST_INCLUDE
  LIBS += -L$$GTEST_LIBPATH -lgtest
}

solution {
QT += gui widgets

    TRANSLATIONS += \
        translations/app_ru.ts \
        translations/app_ro.ts

    SOURCES += \
        src/main.cpp \
        src/core/sudokuboard.cpp \
        src/core/sudokustack.cpp \
        src/core/sudokutimer.cpp \
        src/game/sudokugamemanager.cpp \
        src/solver/sudokusolver.cpp \
        src/solver/sudokuvalidator.cpp \
        src/storage/sudokufilemanager.cpp \
        src/ui/mainwindow.cpp \
        src/ui/sudokuboardwidget.cpp

    HEADERS += \
        include/core/sudokuboard.h \
        include/core/sudokustack.h \
        include/core/sudokutimer.h \
        include/core/sudokuerrors.h \
        include/game/sudokugamemanager.h \
        include/solver/sudokusolver.h \
        include/solver/sudokuvalidator.h \
        include/storage/sudokufilemanager.h \
        include/ui/mainwindow.h \
        include/ui/sudokuboardwidget.h \
        include/ui/sudokucolors.h
}
