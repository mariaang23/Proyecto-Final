#include "goku.h"
#include "qgraphicsitem.h"
#include "qgraphicsscene.h"
#include "qtimer.h"

Goku::Goku(QGraphicsScene *scene, int _velocidad, int _fotogWidth, int _fotogHeight, QObject *parent)
    : QObject(parent), scene(scene), velocidad(_velocidad), fotogWidth(_fotogWidth), fotogHeight(_fotogHeight)
{
    sprite = new QGraphicsPixmapItem();
    scene->addItem(sprite);
    cargarImagen();

    timerMovimiento = new QTimer(this); // Timer para Goku
    connect(timerMovimiento, &QTimer::timeout, this, &Goku::mover);
}

void Goku::cargarImagen(){
    QPixmap spriteSheet(":/images/GokuSpriter.png");

    if (spriteSheet.isNull()) {
        qWarning() << "No se pudo cargar GokuSpriter.png";
        return;
    }

    frames.clear();  // en caso de múltiples llamadas

    for (int i = 0; i < 4; ++i) {
        frames.append(spriteSheet.copy(i * fotogWidth, 0, fotogWidth, fotogHeight));
    }

    if (!frames.isEmpty()) {
        sprite->setPixmap(frames[0]); // aca se puede cambiar el spriter de Goku (de 0 a 3)
        frameActual = 0;
    }

}

void Goku::iniciar(int x, int y){
    sprite ->setPos(x, y);
    timerMovimiento->start(60);
}

void Goku::mover(){
    sprite -> moveBy(velocidad, 0);
    if (sprite -> x() + sprite->pixmap().width() < 0){
        timerMovimiento -> stop();
    }
}
