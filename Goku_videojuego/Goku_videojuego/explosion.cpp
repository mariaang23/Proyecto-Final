#include "explosion.h"
#include "nivel2.h"
#include "goku2.h"
#include <QMessageBox>
#include <QTimer>
#include <QGraphicsItem>
#include <QPixmap>
#include <QDebug>
#include <QtMath>

const QPointF Explosion::posicionDisparo = QPointF(1000, 330);

Explosion::Explosion(QGraphicsScene *scene, QObject *parent)
    : obstaculo(scene, obstaculo::Explosion, 6, parent),
    velocidadX(-10),
    velocidadY(-15),
    gravedad(1.2),
    tiempo(0),
    frameActual(0),
    tipoMovimiento(Parabolico),
    posicionInicial(posicionDisparo)
{
    const int anchoFrame = 100;
    const int altoFrame  = 72;

    QPixmap sheet(":/images/explosion.png");
    if (sheet.isNull()) {
        QMessageBox::critical(nullptr, "Error", "No se encontró explosion.png");
        return;
    }

    const int numFrames = 6;
    frames.clear();
    for (int i = 0; i < numFrames; ++i)
        frames.append(sheet.copy(i * anchoFrame, 0, anchoFrame, altoFrame));

    if (!frames.isEmpty())
        sprite->setPixmap(frames[0]);

    sprite->setScale(1.8);  // tamaño reducido
    sprite->setData(0, "explosion");
}

void Explosion::setTipoMovimiento(TipoMovimiento tipo) {
    tipoMovimiento = tipo;
}

void Explosion::setPosicionInicial(QPointF pos) {
    posicionInicial = pos;
}

void Explosion::lanzar()
{
    sprite->setPos(posicionInicial);
    tiempo = 0;
    frameActual = 0;
    sprite->setPixmap(frames[0]);

    // Configurar velocidades
    if (tipoMovimiento == Parabolico) {
        velocidadX = -10;
        velocidadY = -15;
        gravedad   = 1.2;
    } else {  // MRU en diagonal
        velocidadX = -12;
        velocidadY = 6;
        gravedad   = 0;
    }

    // Timer de movimiento
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, [=]() {
        float x = sprite->x();
        float y = sprite->y();

        // Movimiento
        if (tipoMovimiento == Parabolico) {
            y += velocidadY + gravedad * tiempo;
            tiempo += 0.5;
        } else {
            y += velocidadY;
        }

        x += velocidadX;
        sprite->setPos(x, y);

        // Colisión con Goku2
        QList<QGraphicsItem*> colisiones = sprite->collidingItems();
        for (int i = 0; i < colisiones.size(); ++i) {
            Goku2* goku = dynamic_cast<Goku2*>(colisiones[i]);
            if (goku) {
                goku->recibirDanio(20);

                goku->animarMuerte();  // Llama la animación de muerte

                timerMovimiento->stop();
                sprite->setVisible(false);  // ocultar la explosión
                return;
            }
        }

        // Toca suelo
        if (sprite->y() >= scene->height() - 50) {
            sprite->setPixmap(frames.last());
            timerMovimiento->stop();
            sprite->setVisible(false);  // ocultar pero no eliminar aquí
        }

        // Fuera de pantalla
        if (sprite->x() < -100 || sprite->x() > scene->width() + 100) {
            timerMovimiento->stop();
            sprite->setVisible(false);  // ocultar pero no eliminar aquí
        }
    });

    timerMovimiento->start(30);  // fluido

    // Timer de frames (más lento)
    QTimer* timerFrames = new QTimer(this);
    connect(timerFrames, &QTimer::timeout, this, [=]() {
        if (frameActual < frames.size() - 1) {
            frameActual++;
            sprite->setPixmap(frames[frameActual]);
        } else {
            timerFrames->stop();
            timerFrames->deleteLater();
        }
    });
    timerFrames->start(300);  // animación más lenta
}
