QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    network/client.cpp \
    network/peer.cpp \
    network/server.cpp \
    peerwindow.cpp \
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
    singleplayerwindow.cpp \
    widget/chatbox.cpp \
    widget/timer.cpp

HEADERS += \
    network/client.h \
    network/peer.h \
    network/server.h \
    peerwindow.h \
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
    singleplayerwindow.h \
    widget/chatbox.h \
    widget/timer.h

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

VERSION = 1.1.1.0
RC_ICONS = ico.ico
RC_CODEPAGE = 1200
RC_LANG = 0x0409
QMAKE_TARGET_PRODUCT = "Chess"
QMAKE_TARGET_DESCRIPTION = "Chess Game"
QMAKE_TARGET_COPYRIGHT = "Copyright (C) 2024 DongliTeahouse. All rights reserved."
QMAKE_TARGET_COMPANY = "DongliTeahouse"
QMAKE_TARGET_ORIGINAL_FILENAME = "Chess.exe"
QMAKE_TARGET_INTERNALNAME = "Chess"
QMAKE_TARGET_COMMENTS = "Chess Game"
QMAKE_TARGET_TRADEMARKS = "DongliTeahouse"
