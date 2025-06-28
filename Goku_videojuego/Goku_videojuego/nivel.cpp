#include "nivel.h"
#include <QRandomGenerator>
#include <QGraphicsPixmapItem>

// Constructor base de Nivel
Nivel::Nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent, int numero)
    : QWidget(parent), numeroNivel(numero), vista(view), escena(escena)
{
    // Timer para actualizar el nivel (las subclases implementan actualizarNivel())
    timerNivel = new QTimer(this);
    connect(timerNivel, &QTimer::timeout, this, [=]() {
        this->actualizarNivel();
    });
    timerNivel->start(20); // Llama a actualizarNivel() cada 20 ms
}

// Destructor base
Nivel::~Nivel()
{
    delete camara;
    delete goku;
    delete barraVida;
    delete barraProgreso;
    delete timerNivel;
    delete timerNubes;

    // Liberar pixmaps de nubes
    for (auto *nube : listaNubes)
        delete nube;

    // Liberar pixmaps del fondo
    for (auto *fondo : listaFondos)
        delete fondo;
}

// Devuelve el margen reservado en pantalla para elementos HUD (como vida)
int Nivel::getMargenHUD() const {
    return margenHUD;
}

// Animación básica de las nubes (común a todos los niveles)
void Nivel::moverNubes()
{
    int velocidadNube = 2;

    for (auto *nubeItem : listaNubes) {
        if (!nubeItem) continue;

        nubeItem->setPos(nubeItem->x() - velocidadNube, nubeItem->y());

        if (nubeItem->x() + nubeItem->pixmap().width() < 0) {
            int nuevaX = escena->width();
            int nuevaY = QRandomGenerator::global()->bounded(0, 100);
            nubeItem->setPos(nuevaX, nuevaY);
        }
    }
}
