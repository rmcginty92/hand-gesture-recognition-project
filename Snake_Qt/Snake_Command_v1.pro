######################################################################
# Automatically generated by qmake (2.01a) Fri May 29 14:27:41 2015
######################################################################

TEMPLATE = app
TARGET = qtsnake
DEPENDPATH += .
INCLUDEPATH += .

# qt class
QT += widgets core
# Linux path
#INCLUDEPATH += /usr/include/
#LIBS += "/usr/local/lib/*.so.*"

# mac path
INCLUDEPATH += /usr/local/include/
LIBS += "/usr/local/lib/libopencv*3.0.0.dylib"

SOURCES += \
    main.cpp \
    board.cpp \
    handSeg.cpp \
    highscores.cpp \
    save.cpp \
    window.cpp

RESOURCES =  icons.qrc


HEADERS += \
    board.h \
    handSeg.h \
    save.h \
    window.h \
    highscores.h
