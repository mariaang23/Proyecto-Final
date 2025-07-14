#include "goku.h"
#include <QDebug>
#include <stdexcept>  // Para lanzar excepciones estándar

// Inicialización del contador
int Goku::contador = 0;

/**
@brief Constructor de la clase abstracta Goku. Inicializa los parámetros visuales y lógicos del personaje.

Este constructor configura los atributos principales del personaje Goku, que hereda de `QGraphicsPixmapItem`.
Valida los parámetros de entrada para asegurar que la escena y las dimensiones del sprite sean válidas.

- Inserta al personaje en la escena.
- Establece su capacidad de recibir eventos de teclado (`setFocus()`).
- Inicializa punteros como `timerMovimiento`, `timerDanio` y `vidaHUD` en `nullptr`.

@param scene Puntero a la escena de juego donde se insertará el personaje. No debe ser nulo.
@param velocidad Velocidad horizontal del personaje. Debe ser mayor que cero.
@param fotogWidth Ancho de cada frame del sprite.
@param fotogHeight Alto de cada frame del sprite.
@param parent Puntero al objeto padre en el sistema de objetos de Qt.

@throw std::invalid_argument Si la escena es nula o los parámetros son inválidos.
*/
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

/**
@brief Asocia una barra de vida externa al personaje Goku.

Este método permite vincular un objeto `Vida` al personaje, de forma que cualquier daño recibido
pueda reflejarse visualmente en la interfaz mediante dicha barra.

Debe ser llamado después de crear la barra de vida y antes de que Goku reciba daño.

@param barra Puntero al objeto `Vida` que representa el HUD de salud del personaje.

@throw std::invalid_argument Si el puntero proporcionado es nulo.
*/
void Goku::setBarraVida(Vida* barra) {
    if (!barra) {
        throw std::invalid_argument("Goku::setBarraVida - el puntero a la barra de vida no puede ser nulo.");
    }
    vidaHUD = barra;
}

/**
@brief Aplica daño al personaje y actualiza visualmente la barra de vida asociada.

Este método reduce la vida del personaje en la cantidad especificada.
La vida se refleja a través de un HUD representado por un objeto `Vida`, el cual debe estar previamente asociado
 mediante `setBarraVida()`.

- Si la cantidad es negativa, lanza una excepción.
- Si no hay barra de vida asociada, también lanza una excepción.
- Si la vida llega a cero o menos, se considera que el personaje ha sido derrotado, aunque la reacción específica
(ej. animación o final del juego) debe ser manejada externamente.

@param cantidad Cantidad de puntos de vida que se deben restar al personaje.

@throw std::invalid_argument Si la cantidad es negativa.
@throw std::runtime_error Si no se ha definido una barra de vida.
*/
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

/**
@brief Destructor de la clase base `Goku`.

Este destructor detiene y limpia los temporizadores asociados al movimiento y al control de daño,
dejando los punteros en `nullptr` para evitar accesos colgantes.

No elimina directamente los objetos `timerMovimiento`, `timerDanio` ni `vidaHUD` ya que Qt se encarga
de destruirlos automáticamente si tienen un `QObject` padre asignado.

También libera la referencia a la escena para prevenir referencias circulares o fugas de memoria.

Este destructor debe ser invocado automáticamente al destruir una instancia de `Goku` o sus subclases.
*/
Goku::~Goku() {
    //qDebug() << "Destructor de Goku base llamado";

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

/**
@brief Devuelve la cantidad de vida actual del personaje.

Este método consulta el valor actual de vida a través del objeto `vidaHUD` asociado previamente mediante `setBarraVida()`.

@return Entero que representa la cantidad actual de vida.

@throw std::runtime_error Si no se ha definido una barra de vida (`vidaHUD` es nulo).
*/
int Goku::obtenerVida() const {
    if (!vidaHUD) {
        throw std::runtime_error("Goku::obtenerVida - la barra de vida no está definida.");
    }

    return vidaHUD->obtenerVida();
}
