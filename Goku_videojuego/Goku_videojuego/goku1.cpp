#include "goku1.h"
#include <QKeyEvent>
#include <QCoreApplication>
#include <QThread>
#include <QDebug>

// Constructor de Goku1
Goku1::Goku1(QGraphicsScene* scene, int velocidad, int fotogWidth, int fotogHeight, QObject* parent)
    : Goku(scene, velocidad, fotogWidth, fotogHeight, parent),
    frameActual(1),
    contadorCaminata(0),
    mvtoArriba(false),
    mvtoAbajo(false),
    yaRecibioDanio(false),
    puedeRecibirDanio(true),
    tocoCarro(false),
    tocoObstaculo(false)
{
    // Timer para mover a Goku
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &Goku1::mover);

    // Timer para controlar el tiempo entre daños
    timerDanio = new QTimer(this);
    timerDanio->setSingleShot(true);
    connect(timerDanio, &QTimer::timeout, this, [=]() {
        puedeRecibirDanio = true;
    });
}

// Método que carga los sprites desde el sprite sheet
void Goku1::cargarImagen() {
    frames.clear();
    QPixmap spriteSheet(":/images/GokuSpriter.png");
    if (spriteSheet.isNull()) spriteSheet.load("imagenes/GokuSpriter.png");

    if (spriteSheet.isNull()) {
        qDebug() << "Error: No se encontró GokuSpriter.png";
        return;
    }

    for (int i = 0; i < 5; ++i) {
        frames.append(spriteSheet.copy(i * fotogWidth, 0, fotogWidth, fotogHeight));
    }

    setPixmap(frames[1]); // Frame inicial
}

// Método que posiciona a Goku en la escena e inicia su movimiento
void Goku1::iniciar(int x, int y) {
    setPos(x, y);
    timerMovimiento->start(60); // Intervalo de actualización
}

// Movimiento automático hacia la derecha y detección de colisiones
void Goku1::mover()
{
    // 1. Movimiento horizontal y vertical
    int movimientoVertical = 0;
    if (mvtoArriba) movimientoVertical = -velocidad;
    else if (mvtoAbajo) movimientoVertical = velocidad;

    qreal nuevaX = x() + velocidad;
    qreal limiteX = scene->width() - pixmap().width();

    // 2. Aplicar movimiento
    if (nuevaX <= limiteX) {
        moveBy(velocidad, movimientoVertical);
    } else {
        setX(limiteX);
    }

    // 3. Limitar posición vertical
    QPointF posicionActual = this->pos();
    qreal nuevaY = qBound<qreal>(70.0, posicionActual.y(), scene->height() - pixmap().height() + 80);
    posicionActual.setY(nuevaY);
    setPos(posicionActual);

    // 5. Detección de colisiones
    tocoCarro = false;
    tocoObstaculo = false;

    const QList<QGraphicsItem*> items = collidingItems();
    for (QGraphicsItem* const& item : items) {
        QString etiqueta = item->data(0).toString();
        if (etiqueta == "carro") {
            tocoCarro = true;
            break;
        } else if (etiqueta == "obstaculo") {
            tocoObstaculo = true;
        }
    }

    // 6. Manejo de daños
    if (tocoCarro) {
        yaRecibioDanio = false;
    } else if (tocoObstaculo && puedeRecibirDanio) {
        recibirDanio(20);
        puedeRecibirDanio = false;
        timerDanio->start(1000); // espera 1s antes de volver a recibir daño
    }

    // 7. Actualizar sprite según estado
    mientrasTocaObstaculo();
}

// Control del movimiento con W y S
void Goku1::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_W) {
        mvtoArriba = true;
        actualizarFrame(1);
    } else if (event->key() == Qt::Key_S) {
        mvtoAbajo = true;
        actualizarFrame(2);
    }
}

void Goku1::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_W) mvtoArriba = false;
    else if (event->key() == Qt::Key_S) {
        mvtoAbajo = false;
        actualizarFrame(1);
    }
}

// Cambia el sprite a un frame específico
void Goku1::actualizarFrame(int i) {
    if (i >= 0 && i < frames.size())
        setPixmap(frames[i]);
}

// Método que actualiza el sprite si está tocando obstáculo
void Goku1::mientrasTocaObstaculo() {
    if (tocoObstaculo) {
        if (pixmap().cacheKey() != frames[0].cacheKey())
            actualizarFrame(0); // sprite de daño
        return;
    }
    if (mvtoAbajo) actualizarFrame(2);
    else actualizarFrame(1);
}

// Animación de patada (usada solo en nivel 1)
void Goku1::patadaGokuNivel1() {
    actualizarFrame(3); // patada frame 1
    QCoreApplication::processEvents();
    QThread::msleep(200);
    actualizarFrame(4); // patada frame 2
    QCoreApplication::processEvents();
    QThread::msleep(200);
    detener();
}

// Detiene a Goku y reinicia su sprite
void Goku1::detener() {
    if (timerMovimiento && timerMovimiento->isActive())
        timerMovimiento->stop();

    mvtoArriba = mvtoAbajo = false;
    actualizarFrame(1);
}

// Devuelve el nombre del objeto con el que colisiona
QString Goku1::detectarColision() const {
    const QList<QGraphicsItem*> items = collidingItems();
    for (QGraphicsItem* const& item : items) {
        QString etiqueta = item->data(0).toString();
        if (etiqueta == "carro" || etiqueta == "obstaculo")
            return etiqueta;
    }
    return "";
}



// Getters
bool Goku1::haTocadoCarro() const { return tocoCarro; }
bool Goku1::haTocadoObstaculo() const { return tocoObstaculo; }

// Destructor
Goku1::~Goku1()
{
    qDebug() << "Destructor de Goku1 llamado";

    if (timerMovimiento) {
        timerMovimiento->stop();
        disconnect(timerMovimiento, nullptr, this, nullptr);
        delete timerMovimiento;
        timerMovimiento = nullptr;
    }

    if (timerDanio) {
        timerDanio->stop();
        disconnect(timerDanio, nullptr, this, nullptr);
        delete timerDanio;
        timerDanio = nullptr;
    }
}
