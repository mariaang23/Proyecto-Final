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
}

void Goku::cargarImagen() {
    QPixmap spriteSheet(":/images/GokuSpriter.png");

    if (spriteSheet.isNull()) {
        qWarning() << "No se pudo cargar GokuSpriter.png";
        return;
    }

    frames.clear();
    for (int i = 0; i < 4; ++i) {
        frames.append(spriteSheet.copy(i * fotogWidth, 0, fotogWidth, fotogHeight));
    }

    if (!frames.isEmpty()) {
        setPixmap(frames[0]);
    }
}

void Goku::iniciar(int x, int y) {
    setPos(x, y);
    timerMovimiento->start(50);
}

void Goku::mover() {
    if (nivel == 1) {
        int movimientoVertical = 0;
        if (mvtoArriba) {
            movimientoVertical = -velocidad;
        } else if (mvtoAbajo) {
            movimientoVertical = velocidad;
        }

        moveBy(velocidad, movimientoVertical);

        QRectF sceneRect = scene->sceneRect();
        QPointF pos = this->pos();
        pos.setY(qBound(sceneRect.top(), pos.y(), sceneRect.bottom() - pixmap().height()));

        setPos(pos);

        if (x() + pixmap().width() < 0) {
            timerMovimiento->stop();
        }
    } else if (nivel == 2) {
        // implementar
    }
}

void Goku::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W)
        mvtoArriba = true;
    else if (event->key() == Qt::Key_S)
        mvtoAbajo = true;
}

void Goku::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_W)
        mvtoArriba = false;
    else if (event->key() == Qt::Key_S)
        mvtoAbajo = false;
}


