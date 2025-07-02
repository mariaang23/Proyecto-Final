#include "goku2.h"
#include <QKeyEvent>
#include <QTimer>
#include <QTransform>
#include <QPixmap>
#include <QDebug>

// Constructor de Goku2
Goku2::Goku2(QGraphicsScene* scene, int velocidad, int fotogWidth, int fotogHeight, QObject* parent)
    : Goku(scene, velocidad, fotogWidth, fotogHeight, parent),
    mvtoIzquierda(false),
    mvtoDerecha(false),
    enSalto(false),
    tiempoSalto(0),
    gravedad(0.8f),
    sueloY(300),
    velocidadVertical(0)
{
    // Timer para movimiento horizontal
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &Goku2::mover);

    // Timer para salto
    timerSalto = new QTimer(this);
    connect(timerSalto, &QTimer::timeout, this, &Goku2::actualizarSalto);
}

// Método que carga el sprite de caminata
void Goku2::cargarImagen() {
    QPixmap sprite(":/images/Goku_caminando.png");
    if (sprite.isNull()) sprite.load("imagenes/Goku_caminando.png");

    if (sprite.isNull()) {
        qDebug() << "Error: No se encontró Goku_caminando.png";
        return;
    }

    setPixmap(sprite.copy(0, 0, fotogWidth, fotogHeight));
}

// Posiciona a Goku2 en escena y lo activa
void Goku2::iniciar(int x, int y) {
    setPos(x, y);
    sueloY = y;
    timerMovimiento->start(60);
}

// Movimiento lateral limitado por la vista
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

// Aplica física de salto
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
}

// Movimiento y salto por teclado
void Goku2::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_W && !enSalto) {
        enSalto = true;
        velocidadVertical = -15.0f;
        timerSalto->start(16);
        actualizarSpriteSalto();
    } else if (event->key() == Qt::Key_D) mvtoDerecha = true;
    else if (event->key() == Qt::Key_A) mvtoIzquierda = true;
}

void Goku2::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_D) mvtoDerecha = false;
    else if (event->key() == Qt::Key_A) mvtoIzquierda = false;
}

// Sprite caminando (con reflejo si mira izquierda)
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

// Sprite de salto
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

// Define la altura del suelo
void Goku2::setSueloY(float y) {
    sueloY = y;
}

// Destructor
Goku2::~Goku2() {
    delete timerMovimiento;
    delete timerSalto;
}
