#include "goku.h"
#include <QDebug>

// Constructor de Goku (base abstracta)
Goku::Goku(QGraphicsScene* scene, int velocidad, int fotogWidth, int fotogHeight, QObject* parent)
    : QObject(parent),
    QGraphicsPixmapItem(),
    scene(scene),
    velocidad(velocidad),
    fotogWidth(fotogWidth),
    fotogHeight(fotogHeight),
    mirandoDerecha(true),
    vidaHUD(nullptr)
{
    // Se añade Goku a la escena
    scene->addItem(this);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

// Método para asociar la barra de vida a Goku
void Goku::setBarraVida(Vida* barra) {
    vidaHUD = barra;
}

// Método que permite restar vida a Goku
void Goku::recibirDanio(int cantidad) {
    if (vidaHUD) {
        vidaHUD->restar(cantidad);
        if (vidaHUD->obtenerVida() <= 0)
            qDebug() << "Goku ha perdido toda la vida";
    }
}

// Destructor base
Goku::~Goku() {
    qDebug() << "Destructor de Goku base llamado";
    vidaHUD = nullptr;
}
