#include "goku2.h"
#include "nivel2.h"
#include "pocion.h"
#include <QKeyEvent>
#include <QTimer>
#include <QTransform>
#include <QPixmap>
#include <QDebug>
#include <stdexcept>  // Excepciones estándar

// Constructor: inicializa Goku2 y sus timers
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
    if (!nivel2) {
        throw std::invalid_argument("Goku2: el puntero a Nivel2 no puede ser nulo.");
    }

    // Timer para movimiento lateral continuo
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &Goku2::mover);

    // Timer para física del salto
    timerSalto = new QTimer(this);
    connect(timerSalto, &QTimer::timeout, this, &Goku2::actualizarSalto);

    // Timer que controla inmunidad temporal tras recibir daño
    timerDanio = new QTimer(this);
    connect(timerDanio, &QTimer::timeout, this, [=]() {
        puedeRecibirDanio = true;
    });
}

// Destructor: detiene y libera timers
Goku2::~Goku2()
{
    qDebug() << "Destructor de Goku2 llamado";

    if (timerMovimiento) {
        timerMovimiento->stop();
        disconnect(timerMovimiento, nullptr, this, nullptr);
        delete timerMovimiento;
        timerMovimiento = nullptr;
    }

    if (timerSalto) {
        timerSalto->stop();
        disconnect(timerSalto, nullptr, this, nullptr);
        delete timerSalto;
        timerSalto = nullptr;
    }

    if (timerDanio) {
        timerDanio->stop();
        disconnect(timerDanio, nullptr, this, nullptr);
        delete timerDanio;
        timerDanio = nullptr;
    }
}

// Carga sprite inicial desde recursos
void Goku2::cargarImagen() {
    QPixmap sprite(":/images/Goku_caminando.png");
    if (sprite.isNull()) sprite.load("imagenes/Goku_caminando.png");

    if (sprite.isNull()) {
        throw std::runtime_error("Goku2::cargarImagen - No se encontró Goku_caminando.png.");
    }

    setPixmap(sprite.copy(0, 0, fotogWidth, fotogHeight));
}

// Posiciona a Goku y activa movimiento
void Goku2::iniciar(int x, int y) {
    setPos(x, y);
    sueloY = y;

    if (!timerMovimiento) {
        throw std::runtime_error("Goku2::iniciar - Timer de movimiento no inicializado.");
    }

    timerMovimiento->start(60);
}

// Mueve a Goku según teclas presionadas y detecta daño
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

    // Corrección de límites verticales
    if (y() < 0) setY(0);
    if (y() + pixmap().height() > scene->height())
        setY(scene->height() - pixmap().height());

    // Colisión con explosión: daño temporal
    const QList<QGraphicsItem*> colisiones = collidingItems();
    for (int i = 0; i < colisiones.size(); ++i) {
        QGraphicsItem* item = colisiones[i];
        QString etiqueta = item->data(0).toString();
        if (etiqueta == "explosion" && puedeRecibirDanio) {
            recibirDanio(20);
            puedeRecibirDanio = false;
            timerDanio->start(1000);  // 1 segundo de inmunidad
        }
    }
}

// Física del salto con gravedad
void Goku2::actualizarSalto() {
    velocidadVertical += gravedad;
    qreal nuevaY = y() + velocidadVertical;

    if (nuevaY >= sueloY) {
        nuevaY = sueloY;
        enSalto = false;
        velocidadVertical = 0;

        if (timerSalto) timerSalto->stop();

        actualizarSpriteCaminar(mirandoDerecha);
    }

    setY(nuevaY);
    detectarPocion();
}

// Detección y recolección de pociones
void Goku2::detectarPocion() {
    QList<QGraphicsItem*> colisiones = collidingItems();
    for (int i = 0; i < colisiones.size(); ++i) {
        Pocion* pocion = dynamic_cast<Pocion*>(colisiones[i]);
        if (pocion) {
            scene->removeItem(pocion);
            delete pocion;

            if (!nivel2) {
                throw std::runtime_error("Goku2::detectarPocion - nivel2 no está inicializado.");
            }
            nivel2->pocionRecolectada();
        }
    }
}

// Interacción con teclas (WASD)
void Goku2::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_W && !enSalto) {
        enSalto = true;
        velocidadVertical = -15.0f;

        if (!timerSalto) {
            throw std::runtime_error("Goku2::keyPressEvent - timerSalto no está inicializado.");
        }

        timerSalto->start(16);
        actualizarSpriteSalto();
    } else if (event->key() == Qt::Key_D) {
        mvtoDerecha = true;
    } else if (event->key() == Qt::Key_A) {
        mvtoIzquierda = true;
    }
}

// Fin de movimiento al soltar teclas
void Goku2::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_D) mvtoDerecha = false;
    else if (event->key() == Qt::Key_A) mvtoIzquierda = false;
}

// Cambia sprite de caminata y voltea si es necesario
void Goku2::actualizarSpriteCaminar(bool derecha) {
    QPixmap sprite(":/images/Goku_caminando.png");
    if (sprite.isNull()) sprite.load("imagenes/Goku_caminando.png");
    if (sprite.isNull()) {
        throw std::runtime_error("Goku2::actualizarSpriteCaminar - No se encontró el sprite.");
    }

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

// Sprite para cuando está saltando
void Goku2::actualizarSpriteSalto() {
    QPixmap sprite(":/images/Goku_saltando.png");
    if (sprite.isNull()) sprite.load("imagenes/Goku_saltando.png");
    if (sprite.isNull()) {
        throw std::runtime_error("Goku2::actualizarSpriteSalto - No se encontró el sprite.");
    }

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

// Detiene todos los timers activos
void Goku2::detener() {
    if (timerMovimiento) timerMovimiento->stop();
    if (timerSalto) timerSalto->stop();
}

// Animación de muerte de Goku usando 6 frames
void Goku2::animarMuerte() {
    QPixmap spriteSheet(":/images/Goku_muere.png");
    if (spriteSheet.isNull()) {
        throw std::runtime_error("Goku2::animarMuerte - No se encontró Goku_muere.png.");
    }

    QVector<QPixmap> framesMuerte;
    const int anchoFrame = 280;
    const int altoFrame = 298;

    for (int i = 0; i < 6; ++i) {
        framesMuerte.append(spriteSheet.copy(i * anchoFrame, 0, anchoFrame, altoFrame));
    }

    detener();  // Detener movimiento mientras muere

    int* index = new int(0);  // Índice dinámico para animación
    QTimer* timerMuerte = new QTimer(this);  // Se destruye automáticamente

    connect(timerMuerte, &QTimer::timeout, this, [=]() mutable {
        if (*index < framesMuerte.size()) {
            this->setPixmap(framesMuerte[*index]);
            (*index)++;
        } else {
            timerMuerte->stop();
            timerMuerte->deleteLater();
            delete index;

            // Reanudar movimiento después de morir (opcional)
            timerMovimiento->start(60);
        }
    });

    timerMuerte->start(50);  // 50 ms entre frames para una animación fluida
}

// Inicia animación del Kamehameha y ataque al robot
void Goku2::iniciarKamehameha(float xObjetivo, Robot* robotObjetivo) {
    detener();  // Detener cualquier movimiento previo

    QVector<QPixmap> framesSalto;
    QPixmap spriteSalto(":/images/Goku_kam1.png");
    const int w1 = 200, h1 = 262;

    for (int i = 0; i < 6; ++i)
        framesSalto.append(spriteSalto.copy(i * w1, 0, w1, h1));

    int* index = new int(0);
    QTimer* animSalto = new QTimer(this);

    connect(animSalto, &QTimer::timeout, this, [=]() mutable {
        if (*index < framesSalto.size()) {
            setPixmap(framesSalto[*index]);
            setTransform(QTransform());
            setScale(1.0);
            (*index)++;
        } else {
            animSalto->stop();
            animSalto->deleteLater();
            delete index;

            // Camina hacia el robot después del salto
            caminarHaciaRobot(xObjetivo, robotObjetivo);
        }
    });

    animSalto->start(100);  // 100 ms por frame
}

// Movimiento de Goku hacia el robot antes del ataque
void Goku2::caminarHaciaRobot(float xObjetivo, Robot* robotObjetivo) {
    QTimer* avance = new QTimer(this);

    connect(avance, &QTimer::timeout, this, [=]() mutable {
        qreal xActual = this->x();
        if (xActual + velocidad < xObjetivo - 50) {
            setX(xActual + velocidad);
            actualizarSpriteCaminar(true);
        } else {
            avance->stop();
            avance->deleteLater();
            atacarRobot(robotObjetivo);  // Comienza ataque una vez cerca
        }
    });

    avance->start(60);
}

// Animación de ataque Kamehameha
void Goku2::atacarRobot(Robot* robotObjetivo) {
    QVector<QPixmap> framesAtaque;
    QPixmap spriteAtaque(":/images/Goku_kam2.png");
    const int w2 = 325, h2 = 347;

    for (int i = 0; i < 8; ++i)
        framesAtaque.append(spriteAtaque.copy(i * w2, 0, w2, h2));

    int* index = new int(0);
    QTimer* animAtaque = new QTimer(this);

    connect(animAtaque, &QTimer::timeout, this, [=]() mutable {
        if (*index < framesAtaque.size()) {
            setPixmap(framesAtaque[*index]);
            setTransform(QTransform());
            setScale(1.0);
            (*index)++;
        } else {
            animAtaque->stop();
            animAtaque->deleteLater();
            delete index;

            if (robotObjetivo) {
                // Llama método de muerte del robot después del ataque
                QTimer::singleShot(1300, this, [=]() {
                    robotObjetivo->murioRobot();
                });
            }

            // Goku retrocede después del ataque
            float destino = qMax(0.0, this->x() - 300.0);
            caminarHaciaIzquierda(destino);
        }
    });

    animAtaque->start(80);  // 80 ms por frame
}

// Regreso de Goku hacia la izquierda después del ataque
void Goku2::caminarHaciaIzquierda(float xDestino) {
    QTimer* regreso = new QTimer(this);

    connect(regreso, &QTimer::timeout, this, [=]() mutable {
        qreal xActual = this->x();
        if (xActual - velocidad > xDestino) {
            setX(xActual - velocidad);
            actualizarSpriteCaminar(false);
        } else {
            regreso->stop();
            regreso->deleteLater();
            setX(xDestino);
            actualizarSpriteCaminar(false);
        }
    });

    regreso->start(60);
}
