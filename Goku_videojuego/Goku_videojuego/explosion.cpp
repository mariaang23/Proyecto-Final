#include "explosion.h"
#include "goku2.h"
#include <QMessageBox>
#include <QTimer>
#include <QGraphicsItem>
#include <QPixmap>
#include <QDebug>
#include <QtMath>
#include <QPointer>
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

    // 1. Detener y liberar temporizadores
    if (timerMovimiento) {
        disconnect(timerMovimiento, nullptr, this, nullptr);
        timerMovimiento->stop();
        delete timerMovimiento;
        timerMovimiento = nullptr;
    }

    if (timerFrames) {  // ¡Nuevo!
        disconnect(timerFrames, nullptr, this, nullptr);
        timerFrames->stop();
        delete timerFrames;
        timerFrames = nullptr;
    }
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
void Explosion::lanzar() {
    // Configuración inicial
    sprite->setPos(posicionInicial);
    tiempo = 0;
    frameActual = 0;
    sprite->setPixmap(frames[0]);

    // Parámetros de movimiento
    if (tipoMovimiento == Parabolico) {
        velocidadX = -10;
        velocidadY = -15;
        gravedad = 1.2;
    } else {  // MRU
        velocidadX = -12;
        velocidadY = 6;
        gravedad = 0;
    }

    // Temporizador de MOVIMIENTO FÍSICO
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
            y += self->velocidadY;
        }

        self->sprite->setPos(x, y);

        // Detección de COLISIONES
        QList<QGraphicsItem*> colisiones = self->sprite->collidingItems();
        for (int i = 0; i < colisiones.size(); ++i) {
            QGraphicsItem* item = colisiones.at(i);  // Acceso seguro por índice
            if (Goku2* goku = dynamic_cast<Goku2*>(item)) {
                goku->recibirDanio(20);
                goku->animarMuerte();
                self->timerMovimiento->stop();
                self->sprite->hide();
                return;
            }
        }

        // Límites de la pantalla
        if (self->sprite->y() >= self->scene->height() - 50 ||  // Toca el suelo
            self->sprite->x() < -100 ||                          // Sale por izquierda
            self->sprite->x() > self->scene->width() + 100) {    // Sale por derecha
            self->timerMovimiento->stop();
            self->sprite->hide();
        }
    });

    timerMovimiento->start(30);  // Ejecuta cada 30ms

    // Temporizador de ANIMACIÓN VISUAL
    QTimer* timerFrames = new QTimer(this);
    connect(timerFrames, &QTimer::timeout, this, [=]() {

        //qDebug() << "timer frames explosion llamado  "<<contador++;
        if (frameActual < frames.size() - 1) {
            frameActual++;
            sprite->setPixmap(frames[frameActual]);  // Avanza animación
        } else {
            timerFrames->stop();
            timerFrames->deleteLater();
        }
    });

    timerFrames->start(300);  // Cambia frame cada 300ms
}
