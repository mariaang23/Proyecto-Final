#include "camaralogica.h"

// Inicialización del contador
int camaraLogica::contador = 0;

/**
@brief Constructor de la clase camaraLogica.
Inicializa el sistema de cámara que sigue al personaje principal (Goku) durante el juego.
Configura un temporizador interno para actualizar la posición de la vista de forma fluida,
permitiendo que la cámara se desplace automáticamente y mantenga a Goku en un punto específico
de la pantalla mientras se mueve por el nivel.
@param vista Puntero a la QGraphicsView donde se renderiza el juego. No puede ser nulo.
@param parent Objeto padre en la jerarquía de Qt. Si no se especifica, se asume nullptr.
*/
camaraLogica::camaraLogica(QGraphicsView *vista, QObject *parent)
    : QObject(parent), view(vista)
{
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &camaraLogica::moverVista);
}

/**
@brief Destructor de la clase camaraLogica.
Detiene el temporizador interno que controla el seguimiento de la cámara para evitar que
la vista continúe actualizándose después de que el objeto haya sido destruido.
Este método asegura una liberación segura de recursos y previene accesos no válidos
a la vista del juego una vez que el nivel ha terminado o se está cerrando.
*/
camaraLogica::~camaraLogica()
{
    detenerMovimiento();
}

/**
@brief Activa el seguimiento automático de la cámara hacia el personaje objetivo.
Inicia el temporizador interno que actualiza la posición de la vista aproximadamente
60 veces por segundo (cada 16 ms), creando un movimiento suave y continuo de la cámara.
Si el temporizador ya está activo, no realiza ninguna acción para evitar duplicaciones.
Este método debe ser llamado después de asociar un personaje mediante seguirAGoku()
para que el efecto de cámara lateral comience a funcionar en el nivel.
*/
void camaraLogica::iniciarMovimiento()
{
    if (!timer->isActive())
        timer->start(16);   // ~60 FPS
}

/**
@brief Detiene el seguimiento automático de la cámara.
Interrumpe el temporizador interno que actualiza la posición de la vista, deteniendo
así cualquier movimiento adicional de la cámara. Si el temporizador ya está inactivo,
no realiza ninguna acción. Este método se utiliza para pausar o finalizar el efecto
de seguimiento al cambiar de nivel o al cerrar el juego.
*/
void camaraLogica::detenerMovimiento()
{
    if (timer->isActive())
        timer->stop();
}

/**
@brief Asocia al personaje Goku como objetivo de seguimiento de la cámara.
Establece el puntero al objeto Goku que la cámara debe seguir durante el desplazamiento
del nivel. Una vez asignado, cualquier llamada a iniciarMovimiento() hará que la vista
se centre automáticamente en la posición actual de Goku, manteniéndolo siempre visible
en pantalla mientras avanza por el escenario.
@param goku Puntero al objeto Goku que se desea seguir. Debe ser válido y no nulo.
*/
void camaraLogica::seguirAGoku(Goku *goku)
{
    objetivo = goku;
}

/**
@brief Actualiza la posición de la vista para seguir al personaje Goku.
Este método es invocado automáticamente por el temporizador cada ~16 ms mientras el
seguimiento está activo. Calcula la nueva posición centrada en Goku y aplica un
desplazamiento lateral equivalente a la mitad del ancho visible, de modo que Goku
aparezca alineado al borde izquierdo de la pantalla. Si Goku o la vista no están
disponibles, la función se retorna sin efecto, garantizando estabilidad.
@note El desplazamiento lateral crea la sensación de desplazamiento lateral continuo
  del escenario, típico de los “side-scrollers”.
*/
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


