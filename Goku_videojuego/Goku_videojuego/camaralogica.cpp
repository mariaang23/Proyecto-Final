#include "camaralogica.h"

// Inicialización del contador
int camaraLogica::contador = 0;

camaraLogica::camaraLogica(QGraphicsView *vista, QObject *parent)
    : QObject(parent), view(vista)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &camaraLogica::moverVista);
}

camaraLogica::~camaraLogica()
{
    detenerMovimiento();
}

void camaraLogica::iniciarMovimiento()
{
    if (!timer->isActive())
        timer->start(16);   // ~60 FPS
}

void camaraLogica::detenerMovimiento()
{
    if (timer->isActive())
        timer->stop();
}

void camaraLogica::seguirAGoku(Goku *goku)
{
    objetivo = goku;
}

void camaraLogica::moverVista()
{
    //qDebug() << "timer camaralogica llamado  "<<contador++;
    if (!objetivo || !view) return;

    // Obtener la posición de Goku
    qreal xGoku = objetivo->x();
    qreal yCentro = objetivo->y();

    // Queremos que Goku esté al borde izquierdo, entonces
    // centramos la vista en un punto desplazado a la derecha
    qreal desplazamiento = view->viewport()->width() / 2.0;

    view->centerOn(xGoku + desplazamiento, yCentro);


}


