#include "goku.h"
#include "qgraphicsitem.h"
#include "qgraphicsscene.h"
#include "qtimer.h"
#include <QKeyEvent>
#include <QMessageBox>

Goku::Goku(QGraphicsScene *scene, int _velocidad, int _fotogWidth, int _fotogHeight, int _nivel, QObject *parent)
    : QObject(parent), QGraphicsPixmapItem(), scene(scene), frameActual(0), velocidad(_velocidad), fotogWidth(_fotogWidth), fotogHeight(_fotogHeight), nivel(_nivel), mvtoArriba(false), mvtoAbajo(false)
{
    scene->addItem(this);
    cargarImagen();

    // Asegurar que el sprite pueda recibir eventos de teclado
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    timerMovimiento = new QTimer(this); // Timer para Goku
    connect(timerMovimiento, &QTimer::timeout, this, &Goku::mover);

    //inicializamos colisiones false
    tocoCarro      = false;   // para colision
    tocoObstaculo  = false;   //para colision

    //vidas para ambos niveles
    vidas=3;
}

void Goku::cargarImagen() {
    QPixmap spriteSheet(":/images/GokuSpriter.png");

    if (spriteSheet.isNull()) {
        // FALLBACK: intenta desde disco para depurar
        spriteSheet.load("imagenes/GokuSpriter.png");
    }
    if (spriteSheet.isNull()) {
        QMessageBox::critical(nullptr, "Error",
                              "No se encontró GokuSpriter.png; verifica el .qrc o la ruta.");
        return;
    }

    frames.clear();
    for (int i = 0; i < 4; ++i) {
        frames.append(spriteSheet.copy(i * fotogWidth, 0, fotogWidth, fotogHeight));
    }

    if (!frames.isEmpty()) {
        setPixmap(frames[0]);
        setScale(0.7);          // 70 % del tamaño original
    }
}

void Goku::iniciar(int x, int y) {
    setPos(x, y);
    timerMovimiento->start(60);
}

void Goku::mover() {
    if (nivel == 1) {
        int movimientoVertical = 0;
        if (mvtoArriba) {
            movimientoVertical = -velocidad;
        } else if (mvtoAbajo) {
            movimientoVertical = velocidad;
        }

        // --- desplazamiento automatico (x) + controlado (y)
        moveBy(velocidad, movimientoVertical);

        // --- limitar la Y dentro de la escena -----------------
        QRectF sceneRect = scene->sceneRect();
        QPointF pos = this->pos();

        const float margen   = 5;                         // espacio libre arriba y abajo
        const float altoSpr  = boundingRect().height();   // ≈ 308 px

        pos.setY(qBound(sceneRect.top()  + margen,        // minimo permitido
                        pos.y(),                          //valor a usar
                        sceneRect.bottom() - altoSpr - margen)); // maximo permitido

        setPos(pos);   // aplica X e Y corregidas


        if (x() + pixmap().width() < 0) {
            timerMovimiento->stop();
        }

        //datos para detectar colisiones con goku
        tocoCarro     = false;
        tocoObstaculo = false;

        //detectar colisiones de este frame
        for (QGraphicsItem *item : collidingItems()) {

            QString etiqueta = item->data(0).toString();

            if (etiqueta == "carro") {
                tocoCarro = true;            // mar­camos
                break;                       // priorizamos carro
            }
            else if (etiqueta == "obstaculo") {
                tocoObstaculo = true;
            }
        }

        // actuar segun con que colisiono
        if (tocoCarro) {
            //implementar finalizacion de nivel
        }
        else if (tocoObstaculo) {
            restarVida();                    // vidas--
        }

    } else if (nivel == 2) {
        // implementar
    }
}

void Goku::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_W:
        mvtoArriba = true;
        break;
    case Qt::Key_S:
        mvtoAbajo = true;
        actualizarFrame(1);             // ← frame de “descenso”
        break;
    }
}

void Goku::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_W:
        mvtoArriba = false;
        break;
    case Qt::Key_S:
        mvtoAbajo = false;
        actualizarFrame(0);             // ← frame neutro
        break;
    }
}

void Goku::actualizarFrame(int indice)
{
    if (indice >= 0 && indice < frames.size())
        setPixmap(frames[indice]);
}

QString Goku::detectarColision() const
{
    for (QGraphicsItem *item : collidingItems())
    {
        // data(0) ya trae "carro" o "obstaculo"
        const QString etiqueta = item->data(0).toString();
        if (etiqueta == "carro" || etiqueta == "obstaculo")
            return etiqueta;          // devolvemos la primera que encontremos
    }
    return "";                        // no toco nada interesante
}

bool Goku::haTocadoCarro()     const {
    return tocoCarro;
}

bool Goku::haTocadoObstaculo() const {
    return tocoObstaculo;
}

void Goku::restarVida(){
    vidas--;
    //implementar los frames de las vidas (barrita)
}
