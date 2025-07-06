#include "goku2.h"
#include "nivel2.h"
#include "pocion.h"
#include <QKeyEvent>
#include <QTimer>
#include <QTransform>
#include <QPixmap>
#include <QDebug>

// Constructor
Goku2::Goku2(QGraphicsScene* scene, int velocidad, int fotogWidth, int fotogHeight, Nivel2* nivel, QObject* parent)
    : Goku(scene, velocidad, fotogWidth, fotogHeight, parent),
    mvtoIzquierda(false),
    mvtoDerecha(false),
    enSalto(false),
    tiempoSalto(0),
    gravedad(0.8f),
    sueloY(300),
    velocidadVertical(0),
    nivel2(nivel)
{
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &Goku2::mover);

    timerSalto = new QTimer(this);
    connect(timerSalto, &QTimer::timeout, this, &Goku2::actualizarSalto);
}

Goku2::~Goku2() {
    // Lógica directa en lugar de llamar a un virtual
    if (timerMovimiento) {
        timerMovimiento->stop();
        delete timerMovimiento;
        timerMovimiento = nullptr;
    }
}

// Cargar sprite inicial
void Goku2::cargarImagen() {
    QPixmap sprite(":/images/Goku_caminando.png");
    if (sprite.isNull()) sprite.load("imagenes/Goku_caminando.png");

    if (sprite.isNull()) {
        qDebug() << "Error: No se encontró Goku_caminando.png";
        return;
    }

    setPixmap(sprite.copy(0, 0, fotogWidth, fotogHeight));
}

// Posicionar en la escena
void Goku2::iniciar(int x, int y) {
    setPos(x, y);
    sueloY = y;
    timerMovimiento->start(60);
}

// Movimiento lateral y animación
void Goku2::mover() {
    qreal nuevaX = x();

    if (mvtoDerecha) nuevaX += velocidad;
    if (mvtoIzquierda) nuevaX -= velocidad;

    nuevaX = qBound<qreal>(0, nuevaX, scene->width() - pixmap().width());
    setX(nuevaX);

    if (!enSalto) {
        if (mvtoDerecha) actualizarSpriteCaminar(true);
        else if (mvtoIzquierda) actualizarSpriteCaminar(false);
    }

    if (y() < 0) setY(0);
    if (y() + pixmap().height() > scene->height())
        setY(scene->height() - pixmap().height());
}

// Salto con física básica
void Goku2::actualizarSalto() {
    velocidadVertical += gravedad;
    qreal nuevaY = y() + velocidadVertical;

    if (nuevaY >= sueloY) {
        nuevaY = sueloY;
        enSalto = false;
        velocidadVertical = 0;
        timerSalto->stop();
        actualizarSpriteCaminar(mirandoDerecha);
    }

    setY(nuevaY);

    detectarPocion();
}

// Detección de colisión con poción
void Goku2::detectarPocion() {
    QList<QGraphicsItem*> colisiones = collidingItems();
    for (int i = 0; i < colisiones.size(); ++i) {
        Pocion* pocion = dynamic_cast<Pocion*>(colisiones[i]);
        if (pocion) {
            scene->removeItem(pocion);
            delete pocion;
            if (nivel2)
                nivel2->pocionRecolectada();
        }
    }
}


// Controles
void Goku2::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_W && !enSalto) {
        enSalto = true;
        velocidadVertical = -15.0f;
        timerSalto->start(16);
        actualizarSpriteSalto();
    } else if (event->key() == Qt::Key_D) {
        mvtoDerecha = true;
    } else if (event->key() == Qt::Key_A) {
        mvtoIzquierda = true;
    }
}

void Goku2::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_D) mvtoDerecha = false;
    else if (event->key() == Qt::Key_A) mvtoIzquierda = false;
}

// Sprite de caminata con espejo si va a la izquierda
void Goku2::actualizarSpriteCaminar(bool derecha) {
    QPixmap sprite(":/images/Goku_caminando.png");
    if (sprite.isNull()) sprite.load("imagenes/Goku_caminando.png");
    if (sprite.isNull()) return;

    static int frameIndex = 0;
    int totalFrames = sprite.width() / fotogWidth;
    frameIndex = (frameIndex + 1) % totalFrames;

    setPixmap(sprite.copy(frameIndex * fotogWidth, 0, fotogWidth, fotogHeight));

    if (mirandoDerecha != derecha) {
        mirandoDerecha = derecha;
        QTransform transform;
        if (!mirandoDerecha) {
            transform.scale(-1, 1);
            transform.translate(-pixmap().width(), 0);
        }
        setTransform(transform);
    }
}

// Sprite durante el salto
void Goku2::actualizarSpriteSalto() {
    QPixmap sprite(":/images/Goku_saltando.png");
    if (sprite.isNull()) sprite.load("imagenes/Goku_saltando.png");
    if (sprite.isNull()) return;

    setPixmap(sprite.copy(0, 0, 200, 256));

    QTransform transform;
    if (!mirandoDerecha) {
        transform.scale(-1, 1);
        transform.translate(-pixmap().width(), 0);
    }
    setTransform(transform);
}

// Setter para altura del suelo
void Goku2::setSueloY(float y) {
    sueloY = y;
}

// Detiene timers
void Goku2::detener() {
    timerMovimiento->stop();
    timerSalto->stop();
}

void Goku2::animarMuerte()
{
    QPixmap spriteSheet(":/images/Goku_muere.png");
    if (spriteSheet.isNull()) {
        qDebug() << "No se encontró Goku_muere.png";
        return;
    }

    const int anchoFrame = 280;
    const int altoFrame  = 298;

    QVector<QPixmap> framesMuerte;
    for (int i = 0; i < 6; ++i) {
        framesMuerte.append(spriteSheet.copy(i * anchoFrame, 0, anchoFrame, altoFrame));
    }

    // Detener movimiento mientras muere
    detener();

    int* index = new int(0);
    QTimer* timerMuerte = new QTimer(this);  // se elimina automáticamente con el parent

    connect(timerMuerte, &QTimer::timeout, this, [=]() mutable {
        if (*index < framesMuerte.size()) {
            this->setPixmap(framesMuerte[*index]);
            (*index)++;
        } else {
            timerMuerte->stop();
            timerMuerte->deleteLater();
            delete index;

            //  Reanudar movimiento después de la muerte
            timerMovimiento->start(60);
        }
    });

    timerMuerte->start(50);  // 100 ms por frame
}

