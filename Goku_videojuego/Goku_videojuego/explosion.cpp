#include "explosion.h"
#include "goku2.h"
#include <QMessageBox>
#include <QTimer>
#include <QGraphicsItem>
#include <QPixmap>
#include <QDebug>
#include <QtMath>
#include <QPointer>
#include <stdexcept>  // Excepciones estándar

// Posición predeterminada desde donde se lanza la explosión
const QPointF Explosion::posicionDisparo = QPointF(1000, 330);

//contador
int Explosion::contador=0;

/**
@brief Constructor de la clase Explosion.
Crea un proyectil animado que representa una explosión en movimiento. Extrae seis
frames desde la hoja de sprites “:/images/explosion.png”, configura el primero como
imagen inicial y escala el sprite para un tamaño reducido. Establece el tipo de
obstáculo como Explosion, registra la etiqueta “explosion” para detección de colisión
y prepara los parámetros físicos iniciales (trayectoria parabólica por defecto).
Si la imagen no se puede cargar o no contiene frames válidos, lanza una excepción
para evitar un estado inconsistente.
@param scene Escena gráfica donde se insertará la explosión. No puede ser nula.
@param parent Objeto padre en la jerarquía de Qt. Si no se especifica, se asume nullptr.
@throw std::runtime_error Si no se logra cargar la imagen o extraer los frames.
*/
Explosion::Explosion(QGraphicsScene* scene, QObject* parent)
    : obstaculo(scene, obstaculo::Explosion, 6, parent),  // Tipo Explosion con 6 frames
    velocidadX(-10),
    velocidadY(-15),
    gravedad(1.2),
    tiempo(0),
    frameActual(0),
    tipoMovimiento(Parabolico),
    posicionInicial(posicionDisparo)
{
    const int anchoFrame = 100;
    const int altoFrame  = 72;

    // Carga hoja de sprites desde recursos
    QPixmap sheet(":/images/explosion.png");
    if (sheet.isNull()) {
        throw std::runtime_error("Explosion: No se pudo cargar explosion.png.");
    }

    const int numFrames = 6;
    frames.clear();
    for (int i = 0; i < numFrames; ++i)
        frames.append(sheet.copy(i * anchoFrame, 0, anchoFrame, altoFrame));  // Extrae cada frame

    if (frames.isEmpty()) {
        throw std::runtime_error("Explosion: No se pudieron extraer los frames desde la hoja de sprites.");
    }

    // Frame inicial de la animación
    sprite->setPixmap(frames[0]);
    sprite->setScale(1.8);                     // Escala pequeña
    sprite->setData(0, "explosion");           // Identificador del objeto

    //contador+=1;
    //qDebug() << "Explosiones creadas "<<contador;
}

/**
@brief Destructor de la clase Explosion.
Finaliza y libera todos los recursos asociados a la explosión: detiene y desconecta
los temporizadores de movimiento y animación, destruye sus instancias y pone a
nullptr los punteros correspondientes para evitar accesos posteriores. Este proceso
garantiza una limpieza segura y completa cuando el objeto se elimina o se reinicia
el nivel. El sprite es gestionado por la clase base obstáculo.
*/
Explosion::~Explosion() {
    //qDebug() << "Destructor de Explosion llamado";

    // 1. Detener y liberar temporizadores
    if (timerMovimiento) {
        disconnect(timerMovimiento, nullptr, this, nullptr);
        timerMovimiento->stop();
        delete timerMovimiento;
        timerMovimiento = nullptr;
    }

    if (timerAnimacion) {
        disconnect(timerAnimacion, nullptr, this, nullptr);
        timerAnimacion->stop();
        delete timerAnimacion;
        timerAnimacion = nullptr;
    }

    //contador-=1;
    //qDebug() << "Explosiones eliminadas  "<<contador;
}

/**
@brief Establece el tipo de trayectoria que seguirá la explosión.
Permite cambiar dinámicamente entre desplazamiento parabólico (con gravedad) o
movimiento rectilíneo uniforme (MRU) antes de lanzar la explosión. Este ajuste
modifica el comportamiento interno sin necesidad de crear una nueva instancia,
facilitando que los enemigos (por ejemplo, el robot) alternen tipos de disparo.
@param tipo Enumerador que indica si la trayectoria será Parabolico o MRU.
*/
void Explosion::setTipoMovimiento(TipoMovimiento tipo) {
    tipoMovimiento = tipo;
}

/**
@brief Define el punto desde el cual se lanzará la explosión.
Actualiza la coordenada inicial (posicionInicial) que el método lanzar() empleará
para situar el sprite justo antes de iniciar su trayectoria. De esta forma, la explosión
puede originarse en cualquier lugar de la escena (boca del robot, cañón, etc.) sin
necesidad de crear un nuevo objeto.
@param pos Coordenadas absolutas dentro de la escena donde comenzará el disparo.
*/
void Explosion::setPosicionInicial(QPointF pos) {
    posicionInicial = pos;
}

/**
@brief Avanza un paso en la secuencia de frames de la explosión.
Incrementa el índice de frame actual y actualiza el sprite con la imagen correspondiente,
generando la animación de estallido. Cuando se alcanza el último frame, detiene y libera
el temporizador de animación para evitar ciclos innecesarios, marcando así el fin
visual de la explosión.
@note El método verifica que el sprite exista antes de actuar, garantizando estabilidad
  si la explosión se destruye durante la animación.
*/
void Explosion::avanzarFrameAnimacion() {
    if (!sprite) return;

    if (frameActual < frames.size() - 1) {
        frameActual++;
        sprite->setPixmap(frames[frameActual]);
    } else {
        if (timerAnimacion) {
            timerAnimacion->stop();
            timerAnimacion->deleteLater();
            timerAnimacion = nullptr;
        }
    }
}

/**
 @brief Inicia el lanzamiento de la explosión, activando su movimiento físico y su animación visual.

 Este método coloca el sprite de la explosión en su posición inicial (`posicionInicial`) y configura su trayectoria
 dependiendo del tipo de movimiento especificado (parabólico o movimiento rectilíneo uniforme). Luego crea dos temporizadores:

1. **`timerMovimiento`**: se encarga de actualizar la posición del sprite cada 30ms, aplicando una física simple.
    - Si el movimiento es parabólico, se aplica una aceleración simulando la gravedad.
    - Si colisiona con un objeto `Goku2`, se le aplica daño y se oculta la explosión.
    - Si la explosión sale de los límites de la pantalla, también se detiene y se oculta.

2. **`timerAnimacion`**: cambia el frame del sprite cada 300ms para mostrar la animación de la explosión.
   - La animación se detiene automáticamente al llegar al último frame.
Este método es llamado una vez por cada instancia de explosión que se desea animar/lanzar en la escena.

@see Explosion::setTipoMovimiento() para definir el tipo de trayectoria antes de lanzar.
@see Explosion::setPosicionInicial() para definir desde dónde inicia el lanzamiento.
 */
void Explosion::lanzar() {
    // Configuración inicial
    sprite->setPos(posicionInicial);
    tiempo = 0;
    frameActual = 0;
    sprite->setPixmap(frames[0]);

    // Parámetros de movimiento
    if (tipoMovimiento == Parabolico) {
        velocidadX = -10;
        velocidadY = -15;
        gravedad = 1.2;
    } else {  // MRU
        velocidadX = -12;
        velocidadY = 6;
        gravedad = 0;
    }

    // Temporizador de MOVIMIENTO FÍSICO
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, [this]() {
        if (!sprite || !scene) return;  // Validación directa

        // Actualizar posición
        float x = sprite->x() + velocidadX;
        float y = sprite->y();

        if (tipoMovimiento == Parabolico) {
            y += velocidadY + gravedad * tiempo;
            tiempo += 0.5;
        } else {
            y += velocidadY;
        }

        sprite->setPos(x, y);

        // Detección de COLISIONES
        QList<QGraphicsItem*> colisiones = sprite->collidingItems();
        for (int i = 0; i < colisiones.size(); ++i) {
            QGraphicsItem* item = colisiones.at(i);
            if (Goku2* goku = dynamic_cast<Goku2*>(item)) {
                goku->recibirDanio(20);
                goku->animarMuerte();
                timerMovimiento->stop();
                sprite->hide();
                return;
            }
        }

        // Límites de la pantalla
        if (sprite->y() >= scene->height() - 50 ||
            sprite->x() < -100 ||
            sprite->x() > scene->width() + 100) {
            timerMovimiento->stop();
            sprite->hide();
        }
    });


    timerMovimiento->start(30);  // Ejecuta cada 30ms

    // Temporizador de ANIMACIÓN VISUAL
    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &Explosion::avanzarFrameAnimacion);

    timerAnimacion->start(300);  // Cambia frame cada 300ms
}
