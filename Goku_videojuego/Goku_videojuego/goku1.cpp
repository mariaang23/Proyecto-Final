#include "goku1.h"
#include <QKeyEvent>
#include <QCoreApplication>
#include <QThread>
#include <QDebug>

// Inicialización del contador
int Goku1::contador = 0;

/**
@brief Constructor de la clase `Goku1`, versión del personaje para el primer nivel del juego.

Inicializa las variables de estado y configura los temporizadores necesarios para el movimiento y la gestión del daño.

- Crea el `timerMovimiento` que llama periódicamente al método `mover()` para actualizar la posición del personaje.
- Crea el `timerDanio` con modo de disparo único, que permite controlar la inmunidad temporal tras recibir daño.
- Inicializa flags de movimiento vertical (`mvtoArriba`, `mvtoAbajo`) y colisiones (`tocoCarro`, `tocoObstaculo`).

Este constructor debe usarse en el contexto del primer nivel, donde `Goku1` se mueve automáticamente y reacciona a obstáculos.

@param scene Puntero a la escena donde se inserta el personaje.
@param velocidad Velocidad de desplazamiento horizontal.
@param fotogWidth Ancho de cada frame del sprite.
@param fotogHeight Alto de cada frame del sprite.
@param parent Puntero al objeto padre en la jerarquía de Qt (opcional).
*/
Goku1::Goku1(QGraphicsScene* scene, int velocidad, int fotogWidth, int fotogHeight, QObject* parent)
    : Goku(scene, velocidad, fotogWidth, fotogHeight, parent),
    frameActual(1),
    contadorCaminata(0),
    mvtoArriba(false),
    mvtoAbajo(false),
    yaRecibioDanio(false),
    puedeRecibirDanio(true),
    tocoCarro(false),
    tocoObstaculo(false)
{
    // Timer para mover a Goku
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &Goku1::mover);

    // Timer para controlar el tiempo entre daños
    timerDanio = new QTimer(this);
    timerDanio->setSingleShot(true);
    connect(timerDanio, &QTimer::timeout, this, [=]() {

        //qDebug() << "timer danio goku1 llamado  "<<contador++;
        puedeRecibirDanio = true;
    });
}

/**
@brief Carga y extrae los sprites de animación de Goku1 desde una hoja de sprites.

Este método intenta cargar la imagen `GokuSpriter.png` desde los recursos del sistema.
Si falla, intenta cargarla desde la ruta local `imagenes/GokuSpriter.png`.

- Divide la hoja de sprites en 5 frames horizontales, cada uno con el ancho y alto especificados por `fotogWidth` y `fotogHeight`.
- Almacena los frames en el vector `frames`.
- Asigna el segundo frame (`frames[1]`) como imagen inicial del personaje.

Este método debe ser invocado antes de mostrar o animar a Goku1.

@note Si la imagen no se encuentra, se imprime un mensaje de error en consola y no se cargan frames.
*/
void Goku1::cargarImagen() {
    frames.clear();
    QPixmap spriteSheet(":/images/GokuSpriter.png");
    if (spriteSheet.isNull()) spriteSheet.load("imagenes/GokuSpriter.png");

    if (spriteSheet.isNull()) {
        qDebug() << "Error: No se encontró GokuSpriter.png";
        return;
    }

    for (int i = 0; i < 5; ++i) {
        frames.append(spriteSheet.copy(i * fotogWidth, 0, fotogWidth, fotogHeight));
    }

    setPixmap(frames[1]); // Frame inicial
}

/**
@brief Posiciona a Goku1 en la escena e inicia su movimiento automático.

Este método establece la posición inicial del personaje en las coordenadas `(x, y)`
y activa el temporizador de movimiento `timerMovimiento`, que ejecuta el método `mover()` cada 60 milisegundos.

Debe llamarse después de crear la instancia de `Goku1` y de haber cargado sus sprites con `cargarImagen()`.

@param x Coordenada horizontal inicial del personaje.
@param y Coordenada vertical inicial del personaje.
*/
void Goku1::iniciar(int x, int y) {
    setPos(x, y);
    timerMovimiento->start(60); // Intervalo de actualización
}

/**
@brief Actualiza la posición y el estado de Goku1 en la escena, incluyendo movimiento, colisiones y animación.

Este método es ejecutado periódicamente por el temporizador `timerMovimiento` y realiza las siguientes acciones:

1. Calcula el desplazamiento horizontal y vertical según las teclas presionadas (`W`/`S`).
2. Aplica el movimiento si no se ha alcanzado el límite derecho de la escena.
3. Restringe la posición vertical dentro de los márgenes válidos de la escena.
4. Detecta colisiones con elementos etiquetados como `"carro"` o `"obstaculo"`, y actualiza los flags `tocoCarro` y `tocoObstaculo`.
5. Aplica daño si colisiona con un obstáculo, respetando un período de inmunidad de 1 segundo.
6. Actualiza el sprite en función del estado del personaje (`mientrasTocaObstaculo()`).

Este método controla el desplazamiento automático del personaje y su interacción con el entorno del nivel 1.
*/
void Goku1::mover()
{
    //qDebug() << "timer mvto goku1 llamado  "<<contador++;
    // 1. Movimiento horizontal y vertical
    int movimientoVertical = 0;
    if (mvtoArriba) movimientoVertical = -velocidad;
    else if (mvtoAbajo) movimientoVertical = velocidad;

    qreal nuevaX = x() + velocidad;
    qreal limiteX = scene->width() - pixmap().width();

    // 2. Aplicar movimiento
    if (nuevaX <= limiteX) {
        moveBy(velocidad, movimientoVertical);
    } else {
        setX(limiteX);
    }

    // 3. Limitar posición vertical
    QPointF posicionActual = this->pos();
    qreal nuevaY = qBound<qreal>(70.0, posicionActual.y(), scene->height() - pixmap().height() + 80);
    posicionActual.setY(nuevaY);
    setPos(posicionActual);

    // 5. Detección de colisiones
    tocoCarro = false;
    tocoObstaculo = false;

    const QList<QGraphicsItem*> items = collidingItems();
    for (QGraphicsItem* const& item : items) {
        QString etiqueta = item->data(0).toString();
        if (etiqueta == "carro") {
            tocoCarro = true;
            break;
        } else if (etiqueta == "obstaculo") {
            tocoObstaculo = true;
        }
    }

    // 6. Manejo de daños
    if (tocoCarro) {
        yaRecibioDanio = false;
    } else if (tocoObstaculo && puedeRecibirDanio) {
        recibirDanio(20);
        puedeRecibirDanio = false;
        timerDanio->start(1000); // espera 1s antes de volver a recibir daño
    }

    // 7. Actualizar sprite según estado
    mientrasTocaObstaculo();
}

/**
@brief Maneja los eventos de teclado cuando se presionan las teclas de movimiento vertical.

Este método detecta la pulsación de teclas específicas y actualiza las banderas de movimiento correspondientes:

- Si se presiona `W` (`Qt::Key_W`), se activa el movimiento hacia arriba (`mvtoArriba`) y se cambia el sprite a `frame[1]`.
- Si se presiona `S` (`Qt::Key_S`), se activa el movimiento hacia abajo (`mvtoAbajo`) y se cambia el sprite a `frame[2]`.

Este comportamiento permite a Goku1 moverse verticalmente en la escena del juego.

@param event Evento de teclado capturado por Qt.
*/
void Goku1::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_W) {
        mvtoArriba = true;
        actualizarFrame(1);
    } else if (event->key() == Qt::Key_S) {
        mvtoAbajo = true;
        actualizarFrame(2);
    }
}

/**
@brief Maneja los eventos de teclado cuando se sueltan las teclas de movimiento vertical.

Este método desactiva las banderas de movimiento vertical al liberar las teclas `W` o `S`:

- Si se libera `W` (`Qt::Key_W`), se detiene el movimiento hacia arriba (`mvtoArriba`).
- Si se libera `S` (`Qt::Key_S`), se detiene el movimiento hacia abajo (`mvtoAbajo`) y se actualiza el sprite a `frame[1]`, que representa el estado neutro o de caminata.

Este método asegura que Goku1 deje de moverse verticalmente cuando el jugador suelta las teclas.

@param event Evento de teclado que representa la tecla liberada.
*/
void Goku1::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_W) mvtoArriba = false;
    else if (event->key() == Qt::Key_S) {
        mvtoAbajo = false;
        actualizarFrame(1);
    }
}

/**
@brief Cambia el sprite actual de Goku1 al frame indicado.

Este método actualiza la imagen (`pixmap`) del personaje según el índice especificado, siempre que esté dentro del rango válido del vector `frames`.

Se utiliza para reflejar visualmente el estado del personaje, como caminar, recibir daño o realizar una acción.

@param i Índice del frame a mostrar. Debe estar en el rango [0, frames.size()).
*/
void Goku1::actualizarFrame(int i) {
    if (i >= 0 && i < frames.size())
        setPixmap(frames[i]);
}

/**
@brief Actualiza el sprite de Goku1 según si está colisionando con un obstáculo o moviéndose hacia abajo.

- Si `tocoObstaculo` es verdadero, se muestra el sprite correspondiente al daño (`frames[0]`) y se detiene cualquier otro cambio de imagen.
- En caso contrario, si se está moviendo hacia abajo (`mvtoAbajo`), se actualiza al frame de descenso (`frames[2]`), de lo contrario se mantiene el frame de caminata (`frames[1]`).

Este método debe llamarse en cada ciclo de movimiento para reflejar visualmente el estado del personaje.

@see Goku1::mover()
*/
void Goku1::mientrasTocaObstaculo() {
    if (tocoObstaculo) {
        if (pixmap().cacheKey() != frames[0].cacheKey())
            actualizarFrame(0); // sprite de daño
        return;
    }
    if (mvtoAbajo) actualizarFrame(2);
    else actualizarFrame(1);
}

/**
@brief Ejecuta la animación de patada de Goku1, utilizada específicamente en el Nivel 1.

Este método reproduce dos frames de ataque (`frames[3]` y `frames[4]`) con una pausa de 200 milisegundos entre ellos para simular el movimiento de una patada.

- Se llama a `QCoreApplication::processEvents()` para forzar la actualización de la interfaz gráfica entre los cambios de frame.
- Al finalizar la animación, se llama a `detener()` para detener el movimiento de Goku1 y restaurar su estado neutral.

@note Este método está diseñado para usarse en el evento especial de enfrentamiento con el carro en el Nivel 1.
*/
void Goku1::patadaGokuNivel1() {
    actualizarFrame(3); // patada frame 1
    QCoreApplication::processEvents();
    QThread::msleep(200);
    actualizarFrame(4); // patada frame 2
    QCoreApplication::processEvents();
    QThread::msleep(200);
    detener();
}

/**
@brief Detiene el movimiento de Goku1 y restablece su sprite a la posición neutra.

Este método se encarga de detener el temporizador de movimiento si está activo, desactivar las banderas de movimiento vertical
(`mvtoArriba` y `mvtoAbajo`) y actualizar el sprite al frame neutral (`frames[1]`).

Se utiliza, por ejemplo, después de realizar una acción como una patada o al finalizar una secuencia especial.

@see Goku1::patadaGokuNivel1()
*/
void Goku1::detener() {
    if (timerMovimiento && timerMovimiento->isActive())
        timerMovimiento->stop();

    mvtoArriba = mvtoAbajo = false;
    actualizarFrame(1);
}

/**
@brief Detecta si Goku1 está colisionando con un carro u obstáculo.

Este método recorre los elementos con los que Goku1 está colisionando y retorna una cadena identificadora
si alguno de ellos tiene la etiqueta `"carro"` o `"obstaculo"` en su `data(0)`.

@return `"carro"` si colisiona con un carro, `"obstaculo"` si colisiona con un obstáculo, o cadena vacía (`""`) si no hay colisión relevante.
*/
QString Goku1::detectarColision() const {
    const QList<QGraphicsItem*> items = collidingItems();
    for (QGraphicsItem* const& item : items) {
        QString etiqueta = item->data(0).toString();
        if (etiqueta == "carro" || etiqueta == "obstaculo")
            return etiqueta;
    }
    return "";
}

/**
@brief Indica si Goku1 ha colisionado con un carro en el último ciclo de movimiento.

@return `true` si se detectó una colisión con un objeto etiquetado como "carro", `false` en caso contrario.
*/
bool Goku1::haTocadoCarro() const { return tocoCarro; }

/**
@brief Indica si Goku1 ha colisionado con un obstáculo en el último ciclo de movimiento.

@return `true` si se detectó una colisión con un objeto etiquetado como "obstaculo", `false` en caso contrario.
*/
bool Goku1::haTocadoObstaculo() const { return tocoObstaculo; }

/**
@brief Destructor de la clase `Goku1`.

Libera los recursos asociados a los temporizadores de movimiento (`timerMovimiento`) y de daño (`timerDanio`).

- Detiene y desconecta los temporizadores si están activos.
- Libera la memoria asociada a ellos y deja los punteros en `nullptr`.

Este destructor asegura una limpieza adecuada de recursos específicos de `Goku1`, evitando fugas de memoria y conflictos con el sistema de señales de Qt.
*/
Goku1::~Goku1()
{
    qDebug() << "Destructor de Goku1 llamado";

    if (timerMovimiento) {
        timerMovimiento->stop();
        disconnect(timerMovimiento, nullptr, this, nullptr);
        delete timerMovimiento;
        timerMovimiento = nullptr;
    }

    if (timerDanio) {
        timerDanio->stop();
        disconnect(timerDanio, nullptr, this, nullptr);
        delete timerDanio;
        timerDanio = nullptr;
    }
}
