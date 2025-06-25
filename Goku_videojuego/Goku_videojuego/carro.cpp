#include "carro.h"

Carro::Carro(QGraphicsScene *scene, int velocidad, QObject *parent)
    : obstaculo(scene, obstaculo::Roca, velocidad, parent), anguloActual(0), girando(false)
{
    imagenCarro = QPixmap(":/images/carro_rojo.png"); // sprite sheet del carro desde los recursos

    cuadroActual = 0; // frmae[0]

    anchoCuadro = 400;
    altoCuadro = 251;

    // Extrae el primer cuadro desde la imagen y lo aplica al sprite
    QPixmap cuadro = imagenCarro.copy(cuadroActual * anchoCuadro, 0, anchoCuadro, altoCuadro);
    sprite->setPixmap(cuadro);

    sprite->setData(0, "carro");

    // Temporizador para rotación del carro
    timerRotacion = new QTimer(this);

    // Conecta el temporizador al método animarRotacion
    connect(timerRotacion, &QTimer::timeout, this, &Carro::animarRotacion);
}

void Carro::iniciar(int x, int y)
{
    sprite->setPos(x, y);
}

void Carro::rotar()
{
    // Si el carro no está girando, activa el temporizador para comenzar la animación
    if (!girando) {
        girando = true;                // Marca que ya está girando
        timerRotacion->start(150);    // Cambia el cuadro de animación cada 150 milisegundos
    }
}

bool Carro::estaGirando() const {
    return girando;
}

void Carro::empezarEspiral()
{
    if (girando) return;
    girando = true;
    anguloActual = 0;
    timerRotacion->start(40);   // un poco suave
}

void Carro::animarRotacion()
{
    // frame ya existente:
    cuadroActual = (cuadroActual + 1) % 3;
    QPixmap cuadro = imagenCarro.copy(cuadroActual * anchoCuadro, 0,
                                      anchoCuadro, altoCuadro);
    sprite->setPixmap(cuadro);

}

