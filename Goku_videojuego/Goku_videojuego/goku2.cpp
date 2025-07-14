#include "goku2.h"
#include "nivel2.h"
#include "pocion.h"
#include <QKeyEvent>
#include <QTimer>
#include <QTransform>
#include <QPixmap>
#include <QDebug>
#include <stdexcept>  // Excepciones estándar

// Inicialización del contador
int Goku2::contador = 0;

/**
@brief Constructor de la clase `Goku2`, versión del personaje para el segundo nivel del juego.

Inicializa los parámetros específicos de movimiento lateral y salto, además de asociar el personaje al nivel correspondiente (`Nivel2`).

- Crea y conecta tres temporizadores:
  - `timerMovimiento` para movimiento horizontal.
  - `timerSalto` para simular la física del salto.
  - `timerDanio` para controlar la inmunidad temporal tras recibir daño.
- Establece parámetros iniciales como la gravedad, posición del suelo (`sueloY`) y estado de salto.
- Configura un efecto de sonido (`salto`) que se reproduce al saltar.

@param scene Puntero a la escena donde se mostrará el personaje.
@param velocidad Velocidad de desplazamiento horizontal.
@param fotogWidth Ancho de cada frame del sprite.
@param fotogHeight Alto de cada frame del sprite.
@param nivel Puntero al nivel 2 del juego (no debe ser nulo).
@param parent Puntero al objeto padre en la jerarquía de Qt.

@throw std::invalid_argument Si el puntero a `nivel2` es nulo.
*/
Goku2::Goku2(QGraphicsScene* scene, int velocidad, int fotogWidth, int fotogHeight, Nivel2* nivel, QObject* parent)
    : Goku(scene, velocidad, fotogWidth, fotogHeight, parent),
    mvtoIzquierda(false),
    mvtoDerecha(false),
    enSalto(false),
    tiempoSalto(0),
    gravedad(0.8f),
    sueloY(300),
    velocidadVertical(0),
    nivel2(nivel)
{
    if (!nivel2) {
        throw std::invalid_argument("Goku2: el puntero a Nivel2 no puede ser nulo.");
    }

    // Timer para movimiento lateral continuo
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &Goku2::mover);

    // Timer para física del salto
    timerSalto = new QTimer(this);
    connect(timerSalto, &QTimer::timeout, this, &Goku2::actualizarSalto);

    // Timer que controla inmunidad temporal tras recibir daño
    timerDanio = new QTimer(this);
    connect(timerDanio, &QTimer::timeout, this, [=]() {

        //qDebug() << "timer danio goku2 llamado  "<<contador++;
        puedeRecibirDanio = true;
    });

    //Para sonido
    salto = new QMediaPlayer;
    salidaSalto = new QAudioOutput;

    salto->setAudioOutput(salidaSalto);
    salto->setSource(QUrl("qrc:/images/salto2.mp3"));
}

/**
@brief Destructor de la clase `Goku2`.

Libera todos los recursos asociados al personaje en el segundo nivel del juego, incluyendo temporizadores y componentes de audio.

- Detiene y desconecta los temporizadores de movimiento, salto e inmunidad (`timerMovimiento`, `timerSalto`, `timerDanio`).
- Elimina los objetos de audio utilizados para reproducir el sonido de salto (`QMediaPlayer` y `QAudioOutput`).
- Establece los punteros a `nullptr` para evitar accesos colgantes.

Este destructor garantiza una liberación segura de recursos al salir del nivel o destruir al personaje.
*/
Goku2::~Goku2()
{
    qDebug() << "Destructor de Goku2 llamado";

    if (timerMovimiento) {
        timerMovimiento->stop();
        disconnect(timerMovimiento, nullptr, this, nullptr);
        delete timerMovimiento;
        timerMovimiento = nullptr;
    }

    if (timerSalto) {
        timerSalto->stop();
        disconnect(timerSalto, nullptr, this, nullptr);
        delete timerSalto;
        timerSalto = nullptr;
    }

    if (timerDanio) {
        timerDanio->stop();
        disconnect(timerDanio, nullptr, this, nullptr);
        delete timerDanio;
        timerDanio = nullptr;
    }

    //liberar sonido
    salto->stop();
    delete salto;
    delete salidaSalto;
}

/**
@brief Carga el sprite inicial del personaje Goku2.

Este método intenta cargar la imagen `Goku_caminando.png` desde los recursos del sistema.
Si falla, realiza un intento alternativo desde la ruta local `imagenes/Goku_caminando.png`.

- Si la imagen se carga correctamente, se asigna el primer frame (posición 0,0) al `pixmap` del personaje con las dimensiones `fotogWidth` y `fotogHeight`.

@throw std::runtime_error Si no se encuentra la imagen requerida.
*/
void Goku2::cargarImagen() {
    QPixmap sprite(":/images/Goku_caminando.png");
    if (sprite.isNull()) sprite.load("imagenes/Goku_caminando.png");

    if (sprite.isNull()) {
        throw std::runtime_error("Goku2::cargarImagen - No se encontró Goku_caminando.png.");
    }

    setPixmap(sprite.copy(0, 0, fotogWidth, fotogHeight));
}

/**
@brief Posiciona a Goku2 en la escena e inicia su movimiento lateral automático.

Este método establece la posición inicial del personaje en `(x, y)` y actualiza la altura del suelo (`sueloY`) para las físicas de salto.

- Verifica que el temporizador de movimiento (`timerMovimiento`) esté correctamente inicializado.
- Inicia el temporizador para que el personaje comience a moverse de forma continua.

@param x Coordenada horizontal inicial.
@param y Coordenada vertical inicial (también define la posición del suelo).

@throw std::runtime_error Si el temporizador de movimiento no ha sido inicializado.
*/
void Goku2::iniciar(int x, int y) {
    setPos(x, y);
    sueloY = y;

    if (!timerMovimiento) {
        throw std::runtime_error("Goku2::iniciar - Timer de movimiento no inicializado.");
    }

    timerMovimiento->start(60);
}

/**
@brief Actualiza la posición y el estado visual de Goku2 durante el movimiento lateral.

Este método es llamado periódicamente por el `timerMovimiento` y realiza lo siguiente:

- Calcula la nueva posición horizontal según las banderas `mvtoIzquierda` y `mvtoDerecha`.
- Restringe el movimiento dentro de los límites de la escena.
- Si Goku2 no está en salto, actualiza su sprite de caminata según la dirección.
- Corrige la posición vertical si excede los límites de la escena.
- Detecta colisiones con objetos etiquetados como `"explosion"`, aplica daño y activa un período de inmunidad de 1 segundo.

Este método es clave para el control de movimiento lateral y la detección de daño en el Nivel 2.

@see Goku2::actualizarSpriteCaminar
@see Goku2::recibirDanio
*/
void Goku2::mover() {

    //qDebug() << "timer mvto goku2 llamado  "<<contador++;
    qreal nuevaX = x();

    if (mvtoDerecha) nuevaX += velocidad;
    if (mvtoIzquierda) nuevaX -= velocidad;

    nuevaX = qBound<qreal>(0, nuevaX, scene->width() - pixmap().width());
    setX(nuevaX);

    if (!enSalto) {
        if (mvtoDerecha) actualizarSpriteCaminar(true);
        else if (mvtoIzquierda) actualizarSpriteCaminar(false);
    }

    // Corrección de límites verticales
    if (y() < 0) setY(0);
    if (y() + pixmap().height() > scene->height())
        setY(scene->height() - pixmap().height());

    // Colisión con explosión: daño temporal
    const QList<QGraphicsItem*> colisiones = collidingItems();
    for (int i = 0; i < colisiones.size(); ++i) {
        QGraphicsItem* item = colisiones[i];
        QString etiqueta = item->data(0).toString();
        if (etiqueta == "explosion" && puedeRecibirDanio) {
            recibirDanio(20);
            puedeRecibirDanio = false;
            timerDanio->start(1000);  // 1 segundo de inmunidad
        }
    }
}

/**
@brief Aplica la física del salto y actualiza la posición vertical de Goku2.

Este método es llamado periódicamente por el `timerSalto` durante el salto del personaje. Simula una trayectoria ascendente y descendente
mediante una velocidad vertical afectada por la gravedad.

- Incrementa `velocidadVertical` para simular la aceleración de caída.
- Actualiza la posición vertical del personaje.
- Si alcanza o supera el suelo (`sueloY`), detiene el salto, resetea las variables y actualiza el sprite de caminata.
- También invoca `detectarPocion()` para revisar si se recolectó alguna poción durante el salto.

@see Goku2::detectarPocion
@see Goku2::setSueloY
*/
void Goku2::actualizarSalto() {
    //qDebug() << "timersalto goku2 llamado  "<<contador++;
    velocidadVertical += gravedad;
    qreal nuevaY = y() + velocidadVertical;

    if (nuevaY >= sueloY) {
        nuevaY = sueloY;
        enSalto = false;
        velocidadVertical = 0;

        if (timerSalto) timerSalto->stop();

        actualizarSpriteCaminar(mirandoDerecha);
    }

    setY(nuevaY);
    detectarPocion();
}

/**
@brief Detecta y gestiona la recolección de pociones por parte de Goku2.

Este método verifica si el personaje colisiona con algún objeto del tipo `Pocion`. Si encuentra una colisión válida:

- Detiene su animación (`detener()`).
- Oculta la poción visualmente (`setVisible(false)`), la desactiva (`setEnabled(false)`) y la relega al fondo (`setZValue(-100)`).
- Notifica al nivel (`nivel2`) que una poción ha sido recolectada.

Este método se invoca automáticamente en eventos como el salto (`actualizarSalto()`).

@throw std::runtime_error Si `nivel2` no está inicializado.

@see Nivel2::pocionRecolectada
*/
void Goku2::detectarPocion() {
    QList<QGraphicsItem*> colisiones = collidingItems();
    for (int i = 0; i < colisiones.size(); ++i) {
        Pocion* pocion = dynamic_cast<Pocion*>(colisiones[i]);
        if (pocion) {
            // Solo ocultarla de la escena
            pocion->detener();// Detiene su animacion
            pocion->setVisible(false);// No se ve
            pocion->setEnabled(false);// No colisiona
            pocion->setZValue(-100);// ocultado de todo

            if (!nivel2) {
                throw std::runtime_error("Goku2::detectarPocion - nivel2 no está inicializado.");
            }
            nivel2->pocionRecolectada();
        }
    }
}


/**
@brief Maneja las pulsaciones de teclas para controlar el movimiento de Goku2.

Este método responde a tres teclas:

- `W`: inicia un salto si Goku2 no está ya en el aire. Configura la velocidad vertical, activa el `timerSalto` y reproduce el sonido correspondiente.
- `D`: activa el movimiento hacia la derecha (`mvtoDerecha = true`).
- `A`: activa el movimiento hacia la izquierda (`mvtoIzquierda = true`).

También actualiza el sprite del salto al despegar del suelo.

@param event Evento de teclado generado por Qt.

@throw std::runtime_error Si el temporizador de salto (`timerSalto`) no ha sido inicializado.
*/
void Goku2::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_W && !enSalto) {
        enSalto = true;
        velocidadVertical = -15.0f;

        if (!timerSalto) {
            throw std::runtime_error("Goku2::keyPressEvent - timerSalto no está inicializado.");
        }

        timerSalto->start(16);
        actualizarSpriteSalto();

        salto->stop();
        salto->play();
    } else if (event->key() == Qt::Key_D) {
        mvtoDerecha = true;
    } else if (event->key() == Qt::Key_A) {
        mvtoIzquierda = true;
    }
}

/**
@brief Maneja la liberación de teclas para detener el movimiento lateral de Goku2.

Este método se activa cuando el jugador suelta una de las teclas de dirección:

- Si se suelta `D`, se desactiva el movimiento hacia la derecha (`mvtoDerecha = false`).
- Si se suelta `A`, se desactiva el movimiento hacia la izquierda (`mvtoIzquierda = false`).

Permite que Goku2 deje de desplazarse cuando el usuario deja de presionar una tecla de dirección.

@param event Evento de teclado que representa la tecla liberada.
*/

void Goku2::keyReleaseEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_D) mvtoDerecha = false;
    else if (event->key() == Qt::Key_A) mvtoIzquierda = false;
}

/**
@brief Actualiza el sprite de Goku2 durante la caminata y ajusta su orientación.

Este método cambia el frame del sprite para animar la caminata de Goku2, avanzando de forma cíclica
entre los distintos recortes de la hoja de imagen `Goku_caminando.png`.

- Si el personaje cambia de dirección, se invierte horizontalmente el sprite usando una transformación (`QTransform`).
- La dirección del personaje se actualiza mediante la variable `mirandoDerecha`.

@param derecha Indica si Goku2 se está moviendo hacia la derecha (`true`) o hacia la izquierda (`false`).

@throw std::runtime_error Si no se encuentra la imagen del sprite.
*/

void Goku2::actualizarSpriteCaminar(bool derecha) {
    QPixmap sprite(":/images/Goku_caminando.png");
    if (sprite.isNull()) sprite.load("imagenes/Goku_caminando.png");
    if (sprite.isNull()) {
        throw std::runtime_error("Goku2::actualizarSpriteCaminar - No se encontró el sprite.");
    }

    static int frameIndex = 0;
    int totalFrames = sprite.width() / fotogWidth;
    frameIndex = (frameIndex + 1) % totalFrames;

    setPixmap(sprite.copy(frameIndex * fotogWidth, 0, fotogWidth, fotogHeight));

    if (mirandoDerecha != derecha) {
        mirandoDerecha = derecha;
        QTransform transform;
        if (!mirandoDerecha) {
            transform.scale(-1, 1);
            transform.translate(-pixmap().width(), 0);
        }
        setTransform(transform);
    }
}

/**
@brief Cambia el sprite de Goku2 para representar visualmente que está en el aire durante un salto.

Este método carga el sprite `Goku_saltando.png` y asigna el primer frame al personaje.
Además, ajusta su orientación horizontal dependiendo de la dirección en la que está mirando (`mirandoDerecha`).

- Si `mirandoDerecha` es `false`, se aplica una transformación para voltear el sprite horizontalmente.

@throw std::runtime_error Si no se encuentra la imagen del sprite de salto.
*/

void Goku2::actualizarSpriteSalto() {
    QPixmap sprite(":/images/Goku_saltando.png");
    if (sprite.isNull()) sprite.load("imagenes/Goku_saltando.png");
    if (sprite.isNull()) {
        throw std::runtime_error("Goku2::actualizarSpriteSalto - No se encontró el sprite.");
    }

    setPixmap(sprite.copy(0, 0, 200, 256));

    QTransform transform;
    if (!mirandoDerecha) {
        transform.scale(-1, 1);
        transform.translate(-pixmap().width(), 0);
    }
    setTransform(transform);
}

/**
@brief Define la posición vertical del suelo para Goku2.

Este valor se utiliza como referencia para determinar cuándo el personaje ha aterrizado durante un salto.

@param y Coordenada vertical que representa el nivel del suelo en la escena.
*/

void Goku2::setSueloY(float y) {
    sueloY = y;
}

/**
@brief Detiene todos los temporizadores activos de movimiento y salto de Goku2.

Este método se utiliza para pausar completamente al personaje, por ejemplo, durante animaciones especiales
o al finalizar una acción. No modifica la posición ni el estado visual.

@see Goku2::iniciarKamehameha
@see Goku2::animarMuerte
*/
void Goku2::detener() {
    if (timerMovimiento) timerMovimiento->stop();
    if (timerSalto) timerSalto->stop();
}

/**
@brief Ejecuta la animación de muerte de Goku2 utilizando una secuencia de frames.

Este método carga la hoja de sprites `Goku_muere.png`, extrae 6 frames y los reproduce en secuencia para mostrar
una animación fluida de muerte. Durante la animación:

- Se detiene el movimiento del personaje (`detener()`).
- Se actualiza el `pixmap` cada 50 ms mediante un `QTimer`.
- Una vez completada la animación, el movimiento puede reanudarse (opcionalmente).

@note La animación de muerte es visual únicamente; el control de game over debe manejarse externamente.

@throw std::runtime_error Si no se encuentra la imagen de la animación de muerte.
*/
void Goku2::animarMuerte() {
    QPixmap spriteSheet(":/images/Goku_muere.png");
    if (spriteSheet.isNull()) {
        throw std::runtime_error("Goku2::animarMuerte - No se encontró Goku_muere.png.");
    }

    QVector<QPixmap> framesMuerte;
    const int anchoFrame = 280;
    const int altoFrame = 298;

    for (int i = 0; i < 6; ++i) {
        framesMuerte.append(spriteSheet.copy(i * anchoFrame, 0, anchoFrame, altoFrame));
    }

    detener();  // Detener movimiento mientras muere

    int* index = new int(0);  // Índice dinámico para animación
    timerMuerte = new QTimer(this);  // Se destruye automáticamente

    connect(timerMuerte, &QTimer::timeout, this, [=]() mutable {

        //qDebug() << "timer muerte goku2 llamado  " <<contador++;
        if (*index < framesMuerte.size()) {
            this->setPixmap(framesMuerte[*index]);
            (*index)++;
        } else {
            timerMuerte->stop();
            timerMuerte->deleteLater();
            delete index;

            // Reanudar movimiento después de morir (opcional)
            timerMovimiento->start(60);
        }
    });

    timerMuerte->start(50);  // 50 ms entre frames para una animación fluida
}

/**
@brief Inicia la animación del salto previo al ataque Kamehameha de Goku2.

Este método detiene cualquier movimiento actual y reproduce una animación de salto compuesta por 6 frames
extraídos de `Goku_kam1.png`. Cada frame se muestra con un intervalo de 100 ms utilizando un temporizador (`animSalto`).

Una vez completada la animación de salto, se invoca `caminarHaciaRobot()` para que Goku2 se acerque al enemigo antes de lanzar el ataque.

@param xObjetivo Coordenada horizontal del robot objetivo.
@param robotObjetivo Puntero al objeto `Robot` que será atacado.

@throw std::runtime_error Si no se puede cargar el sprite de salto.
@see Goku2::caminarHaciaRobot
*/
void Goku2::iniciarKamehameha(float xObjetivo, Robot* robotObjetivo) {
    detener();  // Detener cualquier movimiento previo

    QVector<QPixmap> framesSalto;
    QPixmap spriteSalto(":/images/Goku_kam1.png");
    const int w1 = 200, h1 = 262;

    for (int i = 0; i < 6; ++i)
        framesSalto.append(spriteSalto.copy(i * w1, 0, w1, h1));

    int* index = new int(0);
    animSalto = new QTimer(this);

    connect(animSalto, &QTimer::timeout, this, [=]() mutable {

        //qDebug() << "timer animsalto goku2 llamado  "<<contador++;
        if (*index < framesSalto.size()) {
            setPixmap(framesSalto[*index]);
            setTransform(QTransform());
            setScale(1.0);
            (*index)++;
        } else {
            animSalto->stop();
            animSalto->deleteLater();
            delete index;

            // Camina hacia el robot después del salto
            caminarHaciaRobot(xObjetivo, robotObjetivo);
        }
    });

    animSalto->start(100);  // 100 ms por frame
}

/**
@brief Mueve a Goku2 automáticamente hacia el robot enemigo antes de ejecutar el ataque Kamehameha.

Este método crea un temporizador (`avance`) que desplaza a Goku2 hacia la coordenada `xObjetivo`, avanzando con la velocidad definida.
Mientras se desplaza, se actualiza el sprite de caminata mirando hacia la derecha.

Cuando Goku2 está lo suficientemente cerca del objetivo (a menos de 50 unidades), se detiene el avance y se llama a `atacarRobot()`.

@param xObjetivo Coordenada X destino a la que Goku2 debe acercarse.
@param robotObjetivo Puntero al robot que será atacado.

@see Goku2::atacarRobot
*/
void Goku2::caminarHaciaRobot(float xObjetivo, Robot* robotObjetivo) {
    avance = new QTimer(this);

    connect(avance, &QTimer::timeout, this, [=]() mutable {

        //qDebug() << "timer avance goku2 llamado  "<<contador++;
        qreal xActual = this->x();
        if (xActual + velocidad < xObjetivo - 50) {
            setX(xActual + velocidad);
            actualizarSpriteCaminar(true);
        } else {
            avance->stop();
            avance->deleteLater();
            atacarRobot(robotObjetivo);  // Comienza ataque una vez cerca
        }
    });

    avance->start(60);
}

/**
@brief Ejecuta la animación de ataque Kamehameha de Goku2 contra el robot enemigo.

Este método reproduce una secuencia de 8 frames extraídos de `Goku_kam2.png`, mostrando la animación del ataque.
Cada frame se actualiza cada 80 milisegundos mediante un temporizador (`animAtaque`).

Una vez completada la animación:
- Se llama a `murioRobot()` en el `robotObjetivo` después de 1.3 segundos.
- Goku2 retrocede automáticamente llamando a `caminarHaciaIzquierda()`.

@param robotObjetivo Puntero al objeto `Robot` que será atacado. Puede ser `nullptr`.

@see Goku2::caminarHaciaIzquierda
@see Robot::murioRobot
*/
void Goku2::atacarRobot(Robot* robotObjetivo) {
    QVector<QPixmap> framesAtaque;
    QPixmap spriteAtaque(":/images/Goku_kam2.png");
    const int w2 = 325, h2 = 347;

    for (int i = 0; i < 8; ++i)
        framesAtaque.append(spriteAtaque.copy(i * w2, 0, w2, h2));

    int* index = new int(0);
    animAtaque = new QTimer(this);

    connect(animAtaque, &QTimer::timeout, this, [=]() mutable {

        //qDebug() << "timerataque goku2  llamado  "<<contador++;
        if (*index < framesAtaque.size()) {
            setPixmap(framesAtaque[*index]);
            setTransform(QTransform());
            setScale(1.0);
            (*index)++;
        } else {
            animAtaque->stop();
            animAtaque->deleteLater();
            delete index;

            if (robotObjetivo) {
                // Llama método de muerte del robot después del ataque
                QTimer::singleShot(1300, this, [=]() {
                    robotObjetivo->murioRobot();
                });
            }

            // Goku retrocede después del ataque
            float destino = qMax(0.0, this->x() - 300.0);
            caminarHaciaIzquierda(destino);
        }
    });

    animAtaque->start(80);  // 80 ms por frame
}

/**
@brief Hace que Goku2 camine automáticamente hacia la izquierda hasta alcanzar una posición objetivo.

Este método crea un temporizador (`regreso`) que mueve a Goku2 hacia la coordenada `xDestino` disminuyendo su posición `x` con cada ciclo.
Durante el movimiento, el sprite se actualiza con la animación de caminata hacia la izquierda.

Una vez alcanzado o superado el destino, se detiene el movimiento, se ajusta la posición final exacta y se actualiza el sprite final.

@param xDestino Coordenada horizontal a la que Goku2 debe retroceder tras el ataque.
*/
void Goku2::caminarHaciaIzquierda(float xDestino) {
    regreso = new QTimer(this);

    connect(regreso, &QTimer::timeout, this, [=]() mutable {

        //qDebug() << "timer regreso goku2  llamado  "<<contador++;
        qreal xActual = this->x();
        if (xActual - velocidad > xDestino) {
            setX(xActual - velocidad);
            actualizarSpriteCaminar(false);
        } else {
            regreso->stop();
            regreso->deleteLater();
            setX(xDestino);
            actualizarSpriteCaminar(false);
        }
    });

    regreso->start(60);
}
