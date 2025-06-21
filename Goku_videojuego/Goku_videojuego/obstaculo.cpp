#include "obstaculo.h"
#include "qgraphicsitem.h"
#include "qtimer.h"
#include <QRandomGenerator>

obstaculo::obstaculo(QGraphicsScene *scene, Tipo tipo, int velocidad, QObject *parent)
    : QObject(parent), velocidad(velocidad), tipo(tipo), scene(scene)
{
    sprite = new QGraphicsPixmapItem();
    scene->addItem(sprite);
    cargarImagenes();

    timerMovimiento = new QTimer(this); // Timer para obstaculos
    connect(timerMovimiento, &QTimer::timeout, this, &obstaculo::mover);

    if (tipo == Ave){
        timerAnimacion = new QTimer(this); // Timer solo para las aves (mover alas)
        connect(timerAnimacion, &QTimer::timeout, this, &obstaculo::actualizar);
    }
}

void obstaculo::iniciar(int x, int y)
{
    // Si no se da una posición, se genera una aleatoria dentro del escenario
    if (x == -1)
        x = QRandomGenerator::global()->bounded(scene->width());  // entre 0 y 4608

    if (y == -1)
        y = (tipo == Ave) ? QRandomGenerator::global()->bounded(100, 400)  // altura entre 100 y 400
                          : scene->height() - sprite->pixmap().height();

    sprite->setPos(x, y);

    timerMovimiento->start(60);

    if (tipo == Ave)
        timerAnimacion->start(100);
}


void obstaculo::cargarImagenes() {
    if (tipo == Ave) {
        fotogWidth = 90;
        fotogHeight = 180;

        QPixmap spriteSheet(":/images/pajaro.png");

        if (spriteSheet.isNull()) {
            qWarning() << "No se pudo cargar pajaro.png";
            return;
        }

        frames.clear();  // en caso de múltiples llamadas

        for (int i = 0; i < 4; ++i) {
            frames.append(spriteSheet.copy(i * fotogWidth, 0, fotogWidth, fotogHeight));
        }

        if (!frames.isEmpty()) {
            sprite->setPixmap(frames[0]);
            frameActual = 0;
        }
    }
    else if (tipo == Montania) {
        sprite->setPixmap(QPixmap(":/images/montania.png"));
    }
    else if (tipo == Roca) {
        sprite->setPixmap(QPixmap(":/images/roca.png"));
    }
}



obstaculo::~obstaculo(){
    scene -> removeItem(sprite);
    delete sprite;
    sprite = nullptr;
}

void obstaculo::mover(){
    sprite -> moveBy(-velocidad, 0);
    if (sprite -> x() + sprite->pixmap().width() < 0){
        timerMovimiento -> stop();
        if (tipo == Ave){
            timerAnimacion -> stop();
            deleteLater();
        }
    }
}

void obstaculo::actualizar() {
    if (frames.isEmpty()) return;
    frameActual = (frameActual + 1) % frames.size();
    sprite->setPixmap(frames[frameActual]);
}


