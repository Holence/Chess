QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cellbutton.cpp \
    engine/board.cpp \
    engine/engine.cpp \
    engine/king.cpp \
    engine/piece.cpp \
    engine/position.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    cellbutton.h \
    engine/board.h \
    engine/engine.h \
    engine/king.h \
    engine/piece.h \
    engine/position.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc

DISTFILES += \
    README.md
