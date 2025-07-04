#include "nivel.h"
#include <QRandomGenerator>
#include <QGraphicsPixmapItem>

int Nivel::contNubes=0;

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
    // 1. Eliminar elementos principales del nivel (si fueron creados con new)
    delete camara;
    delete goku;
    delete barraVida;
    delete barraProgreso;

    // 2. Eliminar temporizadores
    delete timerNivel;
    delete timerNubes;

    // 3. Eliminar todas las nubes creadas (pixmaps en la escena)
    for (auto *nube : listaNubes) {
        contNubes -= 1;
        delete nube;
    }
    listaNubes.clear();  // Por seguridad

    // 4. Eliminar fondos
    for (auto *fondo : listaFondos)
        delete fondo;
    listaFondos.clear();  // Por seguridad
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
                contNubes += 1;
                //qDebug() << "nubes creadas "<< contNubes;
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

void Nivel::mostrarGameOver()
{
    if (overlayGameOver) return;       // ya esta mostrado

    // Crea un QLabel hijo de la vista para que quede “encima”
    overlayGameOver = new QLabel(vista);
    overlayGameOver->setPixmap(
        QPixmap(":/images/gameOver.png")
            .scaled(vista->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    overlayGameOver->setAlignment(Qt::AlignCenter);
    overlayGameOver->setAttribute(Qt::WA_TransparentForMouseEvents); // opcional
    overlayGameOver->setAttribute(Qt::WA_DeleteOnClose);
    overlayGameOver->show();
}
