#include "explosion.h"
#include "goku2.h"
#include <QMessageBox>
#include <QTimer>
#include <QGraphicsItem>
#include <QPixmap>
#include <QDebug>
#include <QtMath>
#include <stdexcept>  // Excepciones estándar

// Inicialización del contador
int Explosion::contador = 0;

// Posición predeterminada desde donde se lanza la explosión
const QPointF Explosion::posicionDisparo = QPointF(1000, 330);

// Constructor: crea una explosión como obstáculo animado
Explosion::Explosion(QGraphicsScene* scene, QObject* parent)
    : obstaculo(scene, obstaculo::Explosion, 6, parent),  // Tipo Explosion con 6 frames
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

    // Carga hoja de sprites desde recursos
    QPixmap sheet(":/images/explosion.png");
    if (sheet.isNull()) {
        throw std::runtime_error("Explosion: No se pudo cargar explosion.png.");
    }

    const int numFrames = 6;
    frames.clear();
    for (int i = 0; i < numFrames; ++i)
        frames.append(sheet.copy(i * anchoFrame, 0, anchoFrame, altoFrame));  // Extrae cada frame

    if (frames.isEmpty()) {
        throw std::runtime_error("Explosion: No se pudieron extraer los frames desde la hoja de sprites.");
    }

    // Frame inicial de la animación
    sprite->setPixmap(frames[0]);
    sprite->setScale(1.8);                     // Escala pequeña
    sprite->setData(0, "explosion");           // Identificador del objeto
}

Explosion::~Explosion() {
    qDebug() << "Destructor de Explosion llamado";

    if (timerMovimiento && timerMovimiento->isActive()) {
        timerMovimiento->stop();
    }
    // No necesita delete: Qt lo elimina porque su parent es this
}

// Establece el tipo de movimiento (parabólico o MRU)
void Explosion::setTipoMovimiento(TipoMovimiento tipo) {
    tipoMovimiento = tipo;
}

// Establece la posición inicial desde la cual se lanza la explosión
void Explosion::setPosicionInicial(QPointF pos) {
    posicionInicial = pos;
}

// Inicia la animación y el movimiento de la explosión
void Explosion::lanzar()
{
    sprite->setPos(posicionInicial);  // Posición inicial del disparo
    tiempo = 0;
    frameActual = 0;
    sprite->setPixmap(frames[0]);

    // Configura velocidades según tipo de movimiento
    if (tipoMovimiento == Parabolico) {
        velocidadX = -10;
        velocidadY = -15;
        gravedad   = 1.2;
    } else {  // Movimiento rectilíneo uniforme (diagonal)
        velocidadX = -12;
        velocidadY = 6;
        gravedad   = 0;
    }

    // Temporizador para animar el movimiento físico
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, [=]() {

        //qDebug() << "timer mvto explosion llamado  "<<contador++;
        float x = sprite->x();
        float y = sprite->y();

        // Actualiza posición vertical según movimiento
        if (tipoMovimiento == Parabolico) {
            y += velocidadY + gravedad * tiempo;
            tiempo += 0.5;
        } else {
            y += velocidadY;
        }

        x += velocidadX;
        sprite->setPos(x, y);  // Aplica nueva posición en la escena

        // Verifica colisiones con Goku
        QList<QGraphicsItem*> colisiones = sprite->collidingItems();
        for (int i = 0; i < colisiones.size(); ++i) {
            QGraphicsItem* item = colisiones[i];  // Acceso por índice, evita copia
            Goku2* goku = dynamic_cast<Goku2*>(item);
            if (goku) {
                goku->recibirDanio(20);
                goku->animarMuerte();

                timerMovimiento->stop();
                sprite->setVisible(false);
                return;
            }
        }

        // Verifica si la explosión toca el suelo
        if (sprite->y() >= scene->height() - 50) {
            sprite->setPixmap(frames.last());  // Muestra frame final
            timerMovimiento->stop();
            sprite->setVisible(false);
        }

        // Verifica si sale de la pantalla horizontalmente
        if (sprite->x() < -100 || sprite->x() > scene->width() + 100) {
            timerMovimiento->stop();
            sprite->setVisible(false);
        }
    });

    timerMovimiento->start(30);  // Movimiento fluido

    // Temporizador para animación de frames visuales
    QTimer* timerFrames = new QTimer(this);
    connect(timerFrames, &QTimer::timeout, this, [=]() {

        //qDebug() << "timer frames explosion llamado  "<<contador++;
        if (frameActual < frames.size() - 1) {
            frameActual++;
            sprite->setPixmap(frames[frameActual]);  // Avanza animación
        } else {
            timerFrames->stop();         // Finaliza animación
            timerFrames->deleteLater();  // Se autodestruye
        }
    });

    timerFrames->start(300);  // Cambia frame cada 300 ms
}
