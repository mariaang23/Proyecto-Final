#include "nivel.h"
#include "carro.h"
#include "obstaculo.h"
#include "QRandomGenerator"
#include <QDebug>

// Recibe un puntero a la escena, la vista y el objeto padre
nivel::nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent)
    : QWidget(parent), vista(view), escena(escena)
{
    int velocidad = 5;  // velocidad desplazamiento camara
    camara = new camaraLogica(vista, velocidad, this); // this: nivel padre de la cámara
    camara->iniciarMovimiento();
}

nivel::~nivel()
{
    delete escena;
}

void nivel::cargarFondoNivel1(const QString &ruta)
{
    background = QPixmap(ruta);
    // Agregar la misma imagen 3 veces, colocandola una al lado de la otra
    for (int i = 0; i <3; i++){
        QGraphicsPixmapItem *_background = new QGraphicsPixmapItem(background);
        _background -> setPos(i * background.width(), 0);   // Lo posiciona a la derecha del anterior
        escena->addItem(_background); // Se agrega a la escena, para que sea visible
    }

    //QList<QGraphicsPixmapItem*> listaNubes;
    nube = QPixmap(":/images/nube.png");
    QPixmap nubeEscalada = nube.scaled(500, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation); // se escala la nube

    for (int i = 0; i < 14; i++) {
        int x = i * 400 + QRandomGenerator::global()->bounded(-60, 50); // Varía el espacio en x
        int y = QRandomGenerator::global()->bounded(0, 71);             // Varía el espacio en y

        if (x + nubeEscalada.width() <= 1536 * 3){  // Se agrega las nubes escaladas, siempre que no sobrepasen el ancho de la escena
            QGraphicsPixmapItem *_nube = new QGraphicsPixmapItem(nubeEscalada);
            _nube->setPos(x, y);
            escena->addItem(_nube);
            //listaNubes.append(_nube);
        }
    }
}

void nivel::agregarCarroFinal()
{
    int x = 4000;  // justo antes del borde
    int y = 300;   // abajo, pero sin salirse

    carroFinal = new Carro(escena, 0, this);
    carroFinal->iniciar(x, y);
    carroFinal->rotar();
}

