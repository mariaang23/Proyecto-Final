#include "camaralogica.h"
#include "qgraphicsitem.h"
int camaraLogica::contCamara=0;
// Recibe un puntero a la vista (QGraphicsView), la velocidad del movimiento y el objeto padre
camaraLogica::camaraLogica(QGraphicsView *vista, int velocidad, QObject *parent)
    : QObject(parent), view(vista), velocidad(velocidad)
{
    contCamara+=1;
    qDebug()<<"creo camara constructor "<<contCamara;
    // Crear temporizador para mover camara cada 60 ms
    timer = new QTimer(this);
    // Conectar temporizador con el slot
    connect(timer, &QTimer::timeout, this, &camaraLogica::moverVista);
}
//aunque el padre QTimer realiza la liberacion, por buena practica se realiza en el destructor
camaraLogica::~camaraLogica() {
    contCamara-=1;
    qDebug()<<"camara destructor "<<contCamara;
    qDebug() << "Destructor de camara llamado";
    delete timer;
}

void  camaraLogica::iniciarMovimiento()
{
    timer->start(60);
}

void camaraLogica::detenerMovimiento()
{
    timer->stop();
}

void camaraLogica::moverVista()
{
    // Obtener rectangulo que representa vista actual
    QRectF rect = view->sceneRect();

    // Calcular nueva posicion en x hacia la derecha (x1 = x0 + v)
    qreal nuevaX = rect.x() + velocidad;

    // Verificar si al mover la camara se pasa el final de la escena (x1 + anchoVistaActual >= anchoEscena)
    if (nuevaX + rect.width() >= view->scene()->sceneRect().width()) {
        // Si se llega al final del ancho, se pone la camara en el borde (anchoEscena - anchoVistaActual, altoVistaActual,anchoVistaActual, altoVistaActual )
        view->setSceneRect(view->scene()->sceneRect().width() - rect.width(), rect.y(), rect.width(), rect.height());
        detenerMovimiento();
    } else {
        // Mover camara hacia la derecha
        view->setSceneRect(nuevaX, rect.y(), rect.width(), rect.height());
    }
}

void camaraLogica::seguirAGoku(Goku* goku)
{
    disconnect(timer, nullptr, nullptr, nullptr); // desconecta cualquier conexión previa

    connect(timer, &QTimer::timeout, this, [=]() {
        int xCentro = goku->x() - view->width() / 3;
        view->centerOn(xCentro, view->scene()->height() / 2);
    });
}

