#include "goku.h"
#include "qgraphicsitem.h"
#include "qgraphicsscene.h"
#include "qtimer.h"
#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>
#include <QThread>
#include <QCoreApplication>

// Constructor de Goku
Goku::Goku(QGraphicsScene *scene, int _velocidad, int _fotogWidth, int _fotogHeight, int _nivel, QObject *parent)
    : QObject(parent),
    QGraphicsPixmapItem(),
    scene(scene),
    frameActual(1),
    velocidad(_velocidad),
    fotogWidth(_fotogWidth),
    fotogHeight(_fotogHeight),
    nivel(_nivel),
    mvtoArriba(false),
    mvtoAbajo(false),
    yaRecibioDanio(false),
    vidaHUD(nullptr) // No se crea aquí, se recibirá desde fuera
{
    scene->addItem(this);    // Añadir a la escena
    cargarImagen();          // Cargar los sprites del personaje

    setFlag(QGraphicsItem::ItemIsFocusable); // Para que reciba teclas
    setFocus();                              // Toma el foco inmediatamente

    // Timer para controlar el movimiento con el tiempo
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &Goku::mover);

    // Inicializar banderas de colisión
    tocoCarro = false;
    tocoObstaculo = false;
}

// Carga los frames de Goku desde un sprite sheet
void Goku::cargarImagen() {

    QPixmap spriteSheet(":/images/GokuSpriter.png");

    // Intentar ruta alternativa si falla el recurso
    if (spriteSheet.isNull()) {
        spriteSheet.load("imagenes/GokuSpriter.png");
    }
    if (spriteSheet.isNull()) {
        QMessageBox::critical(nullptr, "Error", "No se encontró GokuSpriter.png; verifica el .qrc o la ruta.");
        return;
    }

    // Cortar el sprite en frames individuales (4 en este caso)
    frames.clear();
    for (int i = 0; i < 5; ++i) {
        frames.append(spriteSheet.copy(i * fotogWidth, 0, fotogWidth, fotogHeight));
    }

    // Mostrar el primer frame
    if (!frames.isEmpty()) {
        setPixmap(frames[1]);
        setScale(0.8); // Escalar al 80% del tamaño original
    }
}

// Posiciona a Goku y arranca su movimiento
void Goku::iniciar(int x, int y) {
    setPos(x, y);
    timerMovimiento->start(60); // Actualizar posición cada 60 ms
}

// Método principal de movimiento
void Goku::mover() {
    if (nivel == 1) {
        int movimientoVertical = 0;

        // Detectar teclas presionadas
        if (mvtoArriba) movimientoVertical = -velocidad;
        else if (mvtoAbajo) movimientoVertical = velocidad;

        // Calcular nueva posición en X
        qreal nuevaX = x() + velocidad;

        // Limitar X para no pasarse del borde derecho
        qreal limiteX = scene->width() - pixmap().width() * scale();

        if (nuevaX <= limiteX) {
            moveBy(velocidad, movimientoVertical);
        } else {
            setX(limiteX);
        }

        // Corregir Y para no salirse de la escena
        QPointF posicionActual = this->pos();
        const int altoEscena = scene->height();
        const int altoSprite = pixmap().height() - 80;

        qreal nuevaY = qBound<qreal>(70.0, posicionActual.y(), static_cast<qreal>(altoEscena - altoSprite));
        posicionActual.setY(nuevaY);
        setPos(posicionActual);

        // Reiniciar colisiones
        tocoCarro = false;
        tocoObstaculo = false;

        // Revisar colisiones
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

        // Reacciones a colisiones
        if (tocoCarro) {
            // TODO: lógica de fin de nivel

            yaRecibioDanio = false; // Reiniciar para próximas colisiones
        } else if (tocoObstaculo) {
            if (!yaRecibioDanio) {
                recibirDanio(20);
                yaRecibioDanio = true;
            }
        } else{
            yaRecibioDanio = false;
        }

        //mantener el frame 0 mientras dure la colision
        mientrasTocaObstaculo();
    } else if (nivel == 2) {
        // TODO: implementar lógica para nivel 2
    }
}


// Evento al presionar teclas
void Goku::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_W:
        mvtoArriba = true;
        actualizarFrame(1); // Regresar al sprite principal
        break;
    case Qt::Key_S:
        mvtoAbajo = true;
        actualizarFrame(2); // Cambiar sprite a posición descendente
        break;
    }
}

// Evento al soltar teclas
void Goku::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_W:
        mvtoArriba = false;
        break;
    case Qt::Key_S:
        mvtoAbajo = false;
        actualizarFrame(1); // Regresar al sprite principal
        break;
    }
}

// Cambiar el frame actual (imagen) de Goku
void Goku::actualizarFrame(int indice) {
    if (indice >= 0 && indice < frames.size())
        setPixmap(frames[indice]);
}

// Detectar colisiones con "carro" u "obstaculo"
QString Goku::detectarColision() const {
    const QList<QGraphicsItem*> &items = collidingItems();
    for (QGraphicsItem *item : items) {
        const QString etiqueta = item->data(0).toString();
        if (etiqueta == "carro" || etiqueta == "obstaculo")
            return etiqueta;
    }
    return "";
}

// Saber si tocó un carro
bool Goku::haTocadoCarro() const {
    return tocoCarro;
}

// Saber si tocó un obstáculo
bool Goku::haTocadoObstaculo() const {
    return tocoObstaculo;
}

// Asignar barra de vida externa (HUD)
void Goku::setBarraVida(Vida* barra) {
    vidaHUD = barra;
}

// Quitarle vida a Goku al recibir daño
void Goku::recibirDanio(int cantidad) {
    if (vidaHUD) {
        vidaHUD->restar(cantidad);

        if (vidaHUD->obtenerVida() <= 0) {
            qDebug() << "Goku ha perdido toda la vida";
            // Aquí podrías detener el juego o mostrar un mensaje
        }
    }
}

// Destructor: no elimina la barra de vida, porque no la creó
Goku::~Goku() {
    qDebug() << "Destructor de goku llamado";
    delete timerMovimiento; // Eliminar el temporizador creado con new
    vidaHUD = nullptr;
}

void Goku::patadaGokuNivel1()
{
    // Mostrar frame de agacharse
    actualizarFrame(3);  //frame 3 es agachado
    QCoreApplication::processEvents();
    QThread::msleep(200);  // Esperar para que se vea el frame

    // Mostrar frame de patada
    actualizarFrame(4);  // frame 4 es patada
    QCoreApplication::processEvents();
    QThread::msleep(200);  // Esperar para que se vea la patada

    //detener a goku cuando ya pateo
    detener();
}

void Goku::detener()
{
    // Apagar el timer de movimiento
    if (timerMovimiento && timerMovimiento->isActive())
        timerMovimiento->stop();

    // Reiniciar banderas de desplazamiento vertical
    mvtoArriba = mvtoAbajo = false;

    // Dejarlo en su frame 1
    actualizarFrame(1);
}

void Goku::mientrasTocaObstaculo()
{
    //esta tocando?
    if (tocoObstaculo) {

        // Si no esta ya en rojo se cambia
        if (pixmap().cacheKey() != frames[0].cacheKey())
            actualizarFrame(0);      // frame 0 = modo daño
        return;
    }

    // dejo de tocar, volver al frame apropiado segun su estado vertical
    if (mvtoAbajo)
        actualizarFrame(2);          // agachado
    else
        actualizarFrame(1);          // idle
}
