QT       += core gui widgets

# 支持C++17
QMAKE_CXXSTANDARD = 17
QMAKE_CXXSTANDARD_REQUIRED = YES

SOURCES += \
    rawcleaner.cpp \
    main.cpp

HEADERS += \
    rawcleaner.h

FORMS += \
    rawcleaner.ui

# 输出程序名称
TARGET = RawCleaner
TEMPLATE = app

RESOURCES +=
