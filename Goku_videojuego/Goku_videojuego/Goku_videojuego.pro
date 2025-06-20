QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    camaralogica.cpp \
    carro.cpp \
    juego.cpp \
    main.cpp \
    nivel.cpp \
    obstaculo.cpp

HEADERS += \
    camaralogica.h \
    carro.h \
    juego.h \
    nivel.h \
    obstaculo.h

FORMS += \
    juego.ui

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
