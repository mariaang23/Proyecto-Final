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

void Nivel::generarNubes()
{
    nube = QPixmap(":/images/nube.png");

    for (int i = 0; i < 35; i++) {   /// Crear las nubes y generarles el movimiento hasta que el nivel termine (Falta)
        for (int j = 3; j > 0; --j) {
            float escala = j * 0.05;
            int ancho = nube.width() * escala;
            int alto = nube.height() * escala;

            QPixmap nubeEscalada = nube.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            int x = i * 250 + QRandomGenerator::global()->bounded(-60, 100);
            int y = QRandomGenerator::global()->bounded(0, 80);

            if (x + nubeEscalada.width() <= escena->width()) {
                QGraphicsPixmapItem *_nube = new QGraphicsPixmapItem(nubeEscalada);
                _nube->setPos(x, y);
                escena->addItem(_nube);
                listaNubes.push_back(_nube);
            }
        }
    }

    // Conectar el timer si no está conectado
    timerNubes = new QTimer(this);
    connect(timerNubes, &QTimer::timeout, this, &Nivel::moverNubes);
    timerNubes->start(45);
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
