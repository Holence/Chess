QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    widget/basemainwindow.cpp \
    widget/board.cpp \
    widget/cellbutton.cpp \
    engine/bishop.cpp \
    engine/engine.cpp \
    engine/king.cpp \
    engine/knight.cpp \
    engine/pawn.cpp \
    engine/piece.cpp \
    engine/position.cpp \
    engine/queen.cpp \
    engine/rook.cpp \
    main.cpp \
    mainwindow.cpp \
    replay.cpp \
    replaywindow.cpp \
    singleplayerwindow.cpp

HEADERS += \
    widget/basemainwindow.h \
    widget/board.h \
    widget/cellbutton.h \
    engine/bishop.h \
    engine/engine.h \
    engine/king.h \
    engine/knight.h \
    engine/pawn.h \
    engine/piece.h \
    engine/position.h \
    engine/queen.h \
    engine/rook.h \
    mainwindow.h \
    replay.h \
    replaywindow.h \
    singleplayerwindow.h

FORMS += \
    widget/basemainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource.qrc

DISTFILES += \
    README.md
