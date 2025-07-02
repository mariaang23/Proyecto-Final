#include "goku.h"
#include <QKeyEvent>
#include <QDebug>
#include <QCoreApplication>
#include <QThread>
#include <QtMath>

Goku::Goku(QGraphicsScene *scene, int _velocidad, int _fotogWidth, int _fotogHeight, int _nivel, QObject *parent)
    : QObject(parent),
    QGraphicsPixmapItem(),
    scene(scene),
    frameActual(1),
    contadorCaminata(0),
    velocidad(_velocidad),
    fotogWidth(_fotogWidth),
    fotogHeight(_fotogHeight),
    nivel(_nivel),
    mvtoArriba(false),
    mvtoAbajo(false),
    mvtoIzquierda(false),
    mvtoDerecha(false),
    mirandoDerecha(true),
    yaRecibioDanio(false),
    vidaHUD(nullptr),
    enSalto(false),
    tiempoSalto(0.0f),
    gravedad(0.8f),
    sueloY(300.0f),
    velocidadVertical(0.0f)
{
    // Agrega este objeto a la escena para que se dibuje
    scene->addItem(this);
    // Carga los sprites según el nivel
    cargarImagen();

    // Permite que el objeto reciba eventos de teclado
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    // Timer para movimiento horizontal y vertical
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &Goku::mover);

    // Timer para manejar el salto, con una lambda que actualiza la física del salto
    timerSalto = new QTimer(this);
    connect(timerSalto, &QTimer::timeout, this, [this]() {
        /*
            Física del salto (movimiento vertical con aceleración constante):

            Variables:
            y = posición vertical
            v = velocidad vertical
            g = gravedad (constante positiva hacia abajo)

            Ecuaciones discretas por frame:
            v(t+1) = v(t) + g
            y(t+1) = y(t) + v(t+1)

            Al iniciar el salto, se da una velocidad inicial negativa (hacia arriba).
            La gravedad incrementa la velocidad hacia abajo en cada frame,
            haciendo que Goku suba, desacelere, y luego caiga.
        */
        velocidadVertical += gravedad;  // Incrementa velocidad vertical por gravedad (aceleración)

        qreal nuevaY = y() + velocidadVertical;  // Actualiza posición vertical sumando la velocidad

        // Si Goku cae más allá del suelo, se posiciona en el suelo y termina el salto
        if (nuevaY >= sueloY) {
            nuevaY = sueloY;
            enSalto = false;         // Ya no está saltando
            velocidadVertical = 0;   // Velocidad vertical se reinicia
            timerSalto->stop();      // Detiene timer de salto

            actualizarSpriteCaminar(mirandoDerecha);  // Actualiza sprite a caminar cuando aterriza
        }

        setY(nuevaY);
    });

    // Inicializa colisiones
    tocoCarro = false;
    tocoObstaculo = false;

    // --- Protección contra daño continuo de obstáculos (nivel 1) ---
    puedeRecibirDanio = true;  // Goku inicia con posibilidad de recibir daño

    timerDanio = new QTimer(this);  // Timer que habilita el daño tras cierto tiempo
    timerDanio->setSingleShot(true);  // Solo se dispara una vez por activación

    connect(timerDanio, &QTimer::timeout, this, [=]() {
        puedeRecibirDanio = true;  // Habilita recibir daño otra vez después del tiempo
    });
}


void Goku::cargarImagen() {
    frames.clear();

    if (nivel == 1) {
        // Carga el sprite sheet para nivel 1
        QPixmap spriteSheet(":/images/GokuSpriter.png");
        if (spriteSheet.isNull()) spriteSheet.load("imagenes/GokuSpriter.png");

        // Si no se encuentra la imagen, muestra error por consola
        if (spriteSheet.isNull()) {
            qDebug() << "Error: No se encontró GokuSpriter.png";
            return;
        }

        // Extrae frames individuales del sprite sheet
        for (int i = 0; i < 5; ++i) {
            frames.append(spriteSheet.copy(i * fotogWidth, 0, fotogWidth, fotogHeight));
        }

        // Establece frame inicial
        setPixmap(frames[1]);

    } else if (nivel == 2) {
        // Carga imagen de caminar para nivel 2
        QPixmap sprite(":/images/Goku_caminando.png");
        if (sprite.isNull()) sprite.load("imagenes/Goku_caminando.png");

        if (sprite.isNull()) {
            qDebug() << "Error: No se encontró Goku_caminando.png";
            return;
        }

        // Usa fotogWidth y fotogHeight para definir el recorte del sprite
        setPixmap(sprite.copy(0, 0, fotogWidth, fotogHeight));
    }
}

void Goku::iniciar(int x, int y) {
    setPos(x, y);
    sueloY = y;          // Define el nivel del suelo para el salto
    timerMovimiento->start(60);  // Inicia timer que mueve a Goku aprox cada 60 ms
}

void Goku::mover() {
    /*
        Movimiento nivel 1:

        Movimiento vertical con teclas W/S:
        - Si W se presiona, mvtoArriba = true => mueve hacia arriba (y disminuye)
        - Si S se presiona, mvtoAbajo = true => mueve hacia abajo (y aumenta)

        Movimiento horizontal constante hacia la derecha con velocidad fija.

        Para limitar el movimiento dentro de la escena, se usan límites en x e y.

        No hay aceleración, solo velocidad constante: Δx = velocidad, Δy = ±velocidad.
    */
    if (nivel == 1) {
        // Movimiento vertical según teclas W y S
        int movimientoVertical = 0;
        if (mvtoArriba) movimientoVertical = -velocidad;
        else if (mvtoAbajo) movimientoVertical = velocidad;

        qreal nuevaX = x() + velocidad;  // Movimiento horizontal fijo a la derecha
        qreal limiteX = scene->width() - pixmap().width();

        // Verifica que no se pase del límite derecho
        if (nuevaX <= limiteX) {
            moveBy(velocidad, movimientoVertical);
        } else {
            setX(limiteX);
        }

        // Limita verticalmente la posición dentro de la escena
        QPointF posicionActual = this->pos();
        const int altoEscena = scene->height();
        const int altoSprite = pixmap().height() - 80;

        qreal nuevaY = qBound<qreal>(70.0, posicionActual.y(), static_cast<qreal>(altoEscena - altoSprite));
        posicionActual.setY(nuevaY);
        setPos(posicionActual);

        // Reinicia colisiones
        tocoCarro = false;
        tocoObstaculo = false;

        // Detecta colisiones con carros y obstáculos
        const QList<QGraphicsItem*> &items = collidingItems();
        for (QGraphicsItem *item : items) {
            QString etiqueta = item->data(0).toString();
            if (etiqueta == "carro") {
                tocoCarro = true;
                break;
            } else if (etiqueta == "obstaculo") {
                tocoObstaculo = true;
            }
        }

        if (tocoCarro) {
            yaRecibioDanio = false; // Resetea el estado si toca el carro
        } else if (tocoObstaculo && puedeRecibirDanio) {
            recibirDanio(20);
            puedeRecibirDanio = false;
            timerDanio->start(1000); // 1 segundo de inmunidad
        }

        // Actualiza sprite si toca obstáculo
        mientrasTocaObstaculo();
    }

    else if (nivel == 2) {
        /*
        Movimiento nivel 2:

        Movimiento horizontal controlado por teclas A y D:
        - A mueve hacia izquierda (x disminuye)
        - D mueve hacia derecha (x aumenta)

        Velocidad constante, sin aceleración.

        El salto se maneja en timerSalto con física de movimiento uniformemente acelerado.
        */
        qreal nuevaX = x();
        qreal limiteIzq = 0;
        qreal limiteDer = scene->width() - pixmap().width();

        if (mvtoDerecha) {
            nuevaX += velocidad;
        } else if (mvtoIzquierda) {
            nuevaX -= velocidad;
        }

        // Limita el movimiento dentro de la escena horizontalmente
        if (nuevaX < limiteIzq) nuevaX = limiteIzq;
        if (nuevaX > limiteDer) nuevaX = limiteDer;

        setX(nuevaX);

        // Actualiza sprite de caminar si no está saltando
        if (!enSalto) {
            if (mvtoDerecha)
                actualizarSpriteCaminar(true);
            else if (mvtoIzquierda)
                actualizarSpriteCaminar(false);
        }

        // Limita la posición vertical para no salirse de la escena
        if (y() < 0)
            setY(0);
        if (y() + pixmap().height() > scene->height())
            setY(scene->height() - pixmap().height());
    }
}


void Goku::keyPressEvent(QKeyEvent *event) {
    if (nivel == 1) {
        // Movimiento vertical con W y S, cambia frame para animar
        if (event->key() == Qt::Key_W) mvtoArriba = true, actualizarFrame(1);
        else if (event->key() == Qt::Key_S) mvtoAbajo = true, actualizarFrame(2);
    } else if (nivel == 2) {
        /*
            Salto:
            Cuando se presiona W y Goku no está saltando,
            se asigna una velocidad inicial vertical negativa (impulso hacia arriba)
            y se activa el timer que actualiza posición vertical y velocidad.

            Física del salto:
            v0 = -15 (velocidad inicial hacia arriba)
            g = 0.8 (gravedad)

            El timer actualiza velocidad y posición con:
            v(t+1) = v(t) + g
            y(t+1) = y(t) + v(t+1)
        */
        if (event->key() == Qt::Key_W && !enSalto) {
            enSalto = true;
            velocidadVertical = -15.0f;  // Impulso hacia arriba
            timerSalto->start(16);        // Aproximadamente 60 FPS para física salto
            actualizarSpriteSalto();
        } else if (event->key() == Qt::Key_D) mvtoDerecha = true;
        else if (event->key() == Qt::Key_A) mvtoIzquierda = true;
    }
}

void Goku::keyReleaseEvent(QKeyEvent *event) {
    if (nivel == 1) {
        // Detiene movimiento vertical cuando suelta W o S
        if (event->key() == Qt::Key_W) mvtoArriba = false;
        else if (event->key() == Qt::Key_S) mvtoAbajo = false, actualizarFrame(1);
    } else if (nivel == 2) {
        // Detiene movimiento horizontal al soltar A o D
        if (event->key() == Qt::Key_D) mvtoDerecha = false;
        else if (event->key() == Qt::Key_A) mvtoIzquierda = false;
    }
}

void Goku::actualizarFrame(int indice) {
    // Cambia el frame actual de nivel 1
    if (indice >= 0 && indice < frames.size())
        setPixmap(frames[indice]);
}

void Goku::actualizarSpriteCaminar(bool derecha) {
    // Carga sprite sheet de caminar para nivel 2
    QPixmap sprite(":/images/Goku_caminando.png");
    if (sprite.isNull()) sprite.load("imagenes/Goku_caminando.png");

    if (sprite.isNull()) {
        qDebug() << "Error: No se encontró Goku_caminando.png";
        return;
    }

    int totalFrames = sprite.width() / fotogWidth;

    // Animación simple que avanza un frame en cada llamada
    static int frameIndex = 0;
    frameIndex = (frameIndex + 1) % totalFrames;

    // Actualiza el pixmap con el frame correspondiente
    setPixmap(sprite.copy(frameIndex * fotogWidth, 0, fotogWidth, fotogHeight));

    // Si cambia de dirección, aplica transformación para voltear la imagen
    if (mirandoDerecha != derecha) {
        mirandoDerecha = derecha;

        QTransform transform;
        if (!mirandoDerecha) {
            transform.scale(-1, 1); // Voltea horizontalmente
            transform.translate(-pixmap().width(), 0);
        }
        setTransform(transform);
    }
}

void Goku::actualizarSpriteSalto() {
    // Carga el sprite de salto para nivel 2
    QPixmap sprite(":/images/Goku_saltando.png");
    if (sprite.isNull()) sprite.load("imagenes/Goku_saltando.png");

    if (sprite.isNull()) return;

    // Actualiza pixmap con la imagen de salto
    setPixmap(sprite.copy(0, 0, 200, 256));

    // Aplica transformación horizontal si está mirando hacia la izquierda
    QTransform transform;
    if (!mirandoDerecha) {
        transform.scale(-1, 1);
        transform.translate(-pixmap().width(), 0);
        setTransform(transform);
    } else {
        setTransform(QTransform());
    }
}

void Goku::mientrasTocaObstaculo() {
    // Si toca obstáculo, fija un frame especial para mostrar daño o bloqueo
    if (tocoObstaculo) {
        if (pixmap().cacheKey() != frames[0].cacheKey())
            actualizarFrame(0);
        return;
    }

    // Si no toca obstáculo, actualiza el frame según movimiento vertical
    if (mvtoAbajo) actualizarFrame(2);
    else actualizarFrame(1);
}

QString Goku::detectarColision() const {
    // Retorna etiqueta del primer objeto con el que colisiona que sea "carro" u "obstaculo"
    const QList<QGraphicsItem*> &items = collidingItems();
    for (QGraphicsItem *item : items) {
        QString etiqueta = item->data(0).toString();
        if (etiqueta == "carro" || etiqueta == "obstaculo")
            return etiqueta;
    }
    return "";
}

bool Goku::haTocadoCarro() const { return tocoCarro; }
bool Goku::haTocadoObstaculo() const { return tocoObstaculo; }

void Goku::setBarraVida(Vida* barra) { vidaHUD = barra; }

void Goku::recibirDanio(int cantidad) {
    // Resta vida al HUD y avisa si la vida llegó a cero o menos
    if (vidaHUD) {
        vidaHUD->restar(cantidad);
        if (vidaHUD->obtenerVida() <= 0)
            qDebug() << "Goku ha perdido toda la vida";
    }
}

void Goku::patadaGokuNivel1() {
    // Animación simple para patada nivel 1: cambia frames, espera, y luego detiene movimiento
    actualizarFrame(3);
    QCoreApplication::processEvents();
    QThread::msleep(200);
    actualizarFrame(4);
    QCoreApplication::processEvents();
    QThread::msleep(200);
    detener();
}

void Goku::detener() {
    // Detiene el timer de movimiento y resetea movimientos verticales
    if (timerMovimiento && timerMovimiento->isActive())
        timerMovimiento->stop();

    mvtoArriba = mvtoAbajo = false;
    actualizarFrame(1);
}

void Goku::setSueloY(float y) { sueloY = y; }

Goku::~Goku() {
    // Limpieza de timers para evitar fugas de memoria
    qDebug() << "Destructor de Goku llamado";
    delete timerMovimiento;
    delete timerSalto;
    delete timerDanio;
    vidaHUD = nullptr;
}
