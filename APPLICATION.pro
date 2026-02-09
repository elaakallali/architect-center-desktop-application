QT += core gui sql widgets printsupport charts
CONFIG   += c++11
QT += network
TARGET = ArchiTechProject
TEMPLATE = app
QT       += sql
QT += printsupport
QT += charts
QT += core network
QT += serialport
SOURCES += \
    ai_dialog.cpp \
    arduino.cpp \
    client.cpp \
    equipment.cpp \
    equipment_bluetooth.cpp \
    geminiai.cpp \
    keycardmanager.cpp \
    login_dialog.cpp \
    main.cpp \
    mainwindow.cpp \
    connection.cpp \
    employee.cpp \
    qrcode.cpp \
    qrcodegeneratordemo.cpp \
    qrcodegeneratorworker.cpp \
    smtpclient.cpp \
    sponsor.cpp \
    stats.cpp


HEADERS += \
    ../c++ - Copy/keycardmanager.h \
    ai_dialog.h \
    arduino.h \
    client.h \
    equipment.h \
    equipment_bluetooth.h \
    geminiai.h \
    keycardmanager.h \
    login_dialog.h \
    mainwindow.h \
    connection.h \
    employee.h\
    qrcode.h \
    smtpclient.h \
    sponsor.h \
    stats.h


FORMS += \
    ai_dialog.ui \
    login_dialog.ui \
    mainwindow.ui \
    stats.ui

# If using MySQL, ensure you have the necessary MySQL plugin
# For SQLite:
# SQLPLUGIN = sqlite

# Include paths for headers
INCLUDEPATH += $$PWD

# Library paths for linking

# Ensure .cpp files aren't included twice
# Ensure qmake generates a new Makefile each time by adding the following:
QMAKE_CXXFLAGS += -fPIC
