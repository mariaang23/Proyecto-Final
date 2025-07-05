#include "explosion.h"
#include <QMessageBox>

const QPointF Explosion::posicionDisparo = QPointF(1000, 330);   // boca del arma

Explosion::Explosion(QGraphicsScene *scene, QObject *parent):obstaculo(scene, obstaculo::Explosion, 6, parent) //6 de velocidad
{
    //recortar sprites
    const int anchoFrame = 100;
    const int altoFrame  = 72;

    QPixmap sheet(":/images/explosion.png");

    if (sheet.isNull()) {
        QMessageBox::critical(nullptr, "Error",
                              "No se encontró explosion.png ");
        return;
    }

    const int numFrames = 6; // 6 frames
    frames.clear();
    for (int i = 0; i < numFrames; ++i)
        frames.append(sheet.copy(i * anchoFrame, 0, anchoFrame, altoFrame));

    if (!frames.isEmpty())
        sprite->setPixmap(frames[0]); //inicia con el frame 0


    sprite->setScale(3.0);             // escala
    sprite->setPos(posicionDisparo);   // punto de partida

    //implementacion de mvto parabolico
}

