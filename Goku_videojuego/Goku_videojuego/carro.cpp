#include "carro.h"

Carro::Carro(QGraphicsScene *scene, int velocidad, QObject *parent)
    : obstaculo(scene, obstaculo::Roca, velocidad, parent), anguloActual(0), girando(false)
{
    // Cargar el sprite
    imagenCarro = QPixmap(":/images/carro_rojo.png");

    cuadroActual = 0;
    anchoCuadro = 400;
    altoCuadro = 251;

    // Cargar el primer cuadro del sprite
    QPixmap cuadro = imagenCarro.copy(cuadroActual * anchoCuadro, 0, anchoCuadro, altoCuadro);
    sprite->setPixmap(cuadro);

    timerRotacion = new QTimer(this);
    connect(timerRotacion, &QTimer::timeout, this, &Carro::animarRotacion);
}

void Carro::iniciar(int x, int y)
{
    sprite->setPos(x, y);
}

void Carro::rotar()
{
    if (!girando) {
        girando = true;
        timerRotacion->start(150);
    }
}

void Carro::animarRotacion()
{
    cuadroActual = (cuadroActual + 1) % 3;
    QPixmap cuadro = imagenCarro.copy(cuadroActual * anchoCuadro, 0, anchoCuadro, altoCuadro);
    sprite->setPixmap(cuadro);
}

bool Carro::estaGirando() const {
    return girando;
}


