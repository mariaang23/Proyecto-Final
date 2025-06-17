#include "nivel.h"

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

void nivel::cargarFondo(const QString &ruta)
{
    background = QPixmap(ruta);
    // Agregar la misma imagen 3 veces, colocandola una al lado de la otra
    for (int i = 0; i <3; i++){
        QGraphicsPixmapItem *_background = new QGraphicsPixmapItem(background);
        _background -> setPos(i * background.width(), 0);   // Lo posiciona a la derecha del anterior
        escena->addItem(_background); // Se agrega a la escena, para que sea visible
    }

}
