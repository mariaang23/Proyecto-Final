#include "goku.h"
#include <QDebug>
#include <stdexcept>  // Para lanzar excepciones estándar

// Inicialización del contador
int Goku::contador = 0;

// Constructor base de Goku (clase abstracta)
Goku::Goku(QGraphicsScene* scene, int velocidad, int fotogWidth, int fotogHeight, QObject* parent)
    : QObject(parent),
    QGraphicsPixmapItem(),
    scene(scene),
    timerMovimiento(nullptr),
    timerDanio(nullptr),
    frameActual(0),
    velocidad(velocidad),
    fotogWidth(fotogWidth),
    fotogHeight(fotogHeight),
    mirandoDerecha(true),
    vidaHUD(nullptr),
    puedeRecibirDanio(true)
{
    // Validación defensiva: escena no puede ser nula
    if (!scene) {
        throw std::invalid_argument("Goku: la escena no puede ser nula.");
    }

    // Validación de parámetros visuales
    if (fotogWidth <= 0 || fotogHeight <= 0) {
        throw std::invalid_argument("Goku: las dimensiones de los sprites deben ser positivas.");
    }

    if (velocidad <= 0) {
        throw std::invalid_argument("Goku: la velocidad debe ser mayor que cero.");
    }

    // Inserta Goku en la escena y lo hace enfocable
    scene->addItem(this);
    setFlag(QGraphicsItem::ItemIsFocusable);  // Permite recibir eventos de teclado
    setFocus();                                // Goku recibe foco automáticamente
}

// Permite asociar la barra de vida externa al personaje
void Goku::setBarraVida(Vida* barra) {
    if (!barra) {
        throw std::invalid_argument("Goku::setBarraVida - el puntero a la barra de vida no puede ser nulo.");
    }
    vidaHUD = barra;
}

// Aplica daño al personaje, actualiza el HUD visual
void Goku::recibirDanio(int cantidad) {
    if (cantidad < 0) {
        throw std::invalid_argument("Goku::recibirDanio - cantidad de daño negativa no permitida.");
    }

    if (!vidaHUD) {
        throw std::runtime_error("Goku::recibirDanio - no se ha asociado una barra de vida.");
    }

    vidaHUD->restar(cantidad);  // Resta la vida en el HUD

    if (vidaHUD->obtenerVida() <= 0) {
        //qDebug() << "Goku ha perdido toda la vida";  // Punto crítico: posible game over
        // Se podría emitir una señal aquí si se desea
    }
}

// Destructor base: deja punteros en nullptr y detiene posibles timers
Goku::~Goku() {
    qDebug() << "Destructor de Goku base llamado";

    if (timerMovimiento && timerMovimiento->isActive()) {
        timerMovimiento->stop();
    }

    if (timerDanio && timerDanio->isActive()) {
        timerDanio->stop();
    }

    // Qt los eliminará automáticamente si tienen parent
    timerMovimiento = nullptr;
    timerDanio = nullptr;
    vidaHUD = nullptr;
    scene = nullptr;
}

// Devuelve la vida actual del personaje
int Goku::obtenerVida() const {
    if (!vidaHUD) {
        throw std::runtime_error("Goku::obtenerVida - la barra de vida no está definida.");
    }

    return vidaHUD->obtenerVida();
}
