QT += core gui
QT += multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    camaralogica.cpp \
    carro.cpp \
    explosion.cpp \
    goku.cpp \
    goku1.cpp \
    goku2.cpp \
    juego.cpp \
    main.cpp \
    nivel.cpp \
    nivel1.cpp \
    nivel2.cpp \
    obstaculo.cpp \
    pocion.cpp \
    progreso.cpp \
    robot.cpp \
    vida.cpp

HEADERS += \
    camaralogica.h \
    carro.h \
    explosion.h \
    goku.h \
    goku1.h \
    goku2.h \
    juego.h \
    nivel.h \
    nivel1.h \
    nivel2.h \
    obstaculo.h \
    pocion.h \
    progreso.h \
    robot.h \
    vida.h

FORMS += \
    juego.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

QMAKE_LFLAGS_USE_RESPONSE_FILE =

