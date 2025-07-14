#include "robot.h"
#include "explosion.h"
#include <QMessageBox>
#include <QDebug>
#include <stdexcept> // Excepciones estándar

// Inicialización del contador
int Robot::contador = 0;

/**
@brief Constructor principal de la clase Robot.

Crea un robot con animación y movimiento horizontal en una escena gráfica.

@param scene Puntero a la escena gráfica donde se agregará el robot.
@param velocidad Velocidad de desplazamiento horizontal del robot.
@param numeroRobot Número identificador que selecciona el conjunto de sprites a cargar para el robot.
@param parent Objeto padre para la gestión de memoria (generalmente la escena o ventana principal).

@details

Valida que la escena no sea nula y lanza una excepción si lo es.

Crea el sprite gráfico y lo añade a la escena.

Carga las imágenes correspondientes al robot según su número.

Ajusta la escala visual del sprite y le asigna una etiqueta para colisiones.

Configura temporizadores para controlar el movimiento horizontal y la animación del robot.
*/
Robot::Robot(QGraphicsScene *scene, int velocidad, int numeroRobot, QObject *parent)
    : QObject(parent), scene(scene), velocidad(velocidad)
{
    if (!scene)
        throw std::invalid_argument("Robot: la escena no puede ser nula.");

    sprite = new QGraphicsPixmapItem;
    scene->addItem(sprite);

    cargarImagen(numeroRobot); // Carga sprites del robot desde una hoja

    sprite->setScale(5.0);     // Ajuste visual para este tipo
    sprite->setData(0, "robot"); // Etiqueta de colisión

    // Timer que controla el movimiento horizontal
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &Robot::mover);

    // Timer para animar el ciclo de sprites
    timerAnimacion = new QTimer(this);
    connect(timerAnimacion, &QTimer::timeout, this, &Robot::animar);
}

/**
@brief Constructor alternativo de la clase Robot para Nivel 2.

Inicializa un robot con sprites específicos y escala adaptada para el segundo nivel del juego.

@param scene Puntero a la escena gráfica donde se mostrará el robot.
@param parent Objeto padre para gestión de memoria.

@details

Verifica que la escena no sea nula, lanzando excepción si lo es.

Crea el sprite gráfico y lo añade a la escena.

Carga los frames específicos para el robot del Nivel 2.

Ajusta la escala del sprite para adecuarla al nivel.

Asigna una etiqueta diferenciadora para colisiones y lógica de Nivel 2.
*/
Robot::Robot(QGraphicsScene *scene, QObject *parent)
    : QObject(parent), scene(scene)
{
    if (!scene)
        throw std::invalid_argument("Robot (Nivel2): la escena no puede ser nula.");

    sprite = new QGraphicsPixmapItem;
    scene->addItem(sprite);

    cargarRobot2();           // Carga frames para el robot de Nivel2
    sprite->setScale(0.5);    // Escalado diferente
    sprite->setData(0, "robot_nivel2");
}

/**
@brief Destructor de la clase Robot.

Libera todos los recursos asociados al robot, incluyendo temporizadores y objetos gráficos.

@details

Detiene y desconecta los temporizadores de movimiento, animación, ataque y muerte.

Elimina y libera memoria de los temporizadores.

Remueve el sprite gráfico de la escena y lo elimina.

Libera todas las explosiones asociadas al robot, desconectando señales para evitar referencias colgantes.

Limpia la lista de explosiones para evitar fugas de memoria.
*/
Robot::~Robot()
{
    qDebug() << "Destructor de Robot llamado";

    if (timerMovimiento) {
        timerMovimiento->stop();
        disconnect(timerMovimiento, nullptr, this, nullptr);
        delete timerMovimiento;
        timerMovimiento = nullptr;
    }

    if (timerAnimacion) {
        timerAnimacion->stop();
        disconnect(timerAnimacion, nullptr, this, nullptr);
        delete timerAnimacion;
        timerAnimacion = nullptr;
    }

    if (timerAtaque) {
        timerAtaque->stop();
        disconnect(timerAtaque, nullptr, this, nullptr);
        delete timerAtaque;
        timerAtaque = nullptr;
    }

    if (timerMuerte) {
        timerMuerte->stop();
        disconnect(timerMuerte, nullptr, this, nullptr);
        delete timerMuerte;
        timerMuerte = nullptr;
    }

    if (sprite) {
        scene->removeItem(sprite);
        delete sprite;
        sprite = nullptr;
    }

    //liberar explosiones en robot
    for (Explosion* e : ListaExplosiones) {
        if (e) {
            e->disconnect();  // Previene que lambdas pendientes intenten usar el objeto destruido
            //qDebug() << "Entro a desconectar explosion robot y lo logro";
            delete e;
            //qDebug() << "Entro a liberar explosion desde robot y lo logro";
        }
    }
    ListaExplosiones.clear();

}

/**
@brief Carga la hoja de sprites y extrae los frames correspondientes para un robot específico.

@param numeroRobot Número identificador del robot que determina qué conjunto de frames cargar.

@details

Carga la imagen "robots1.png" desde los recursos o desde un archivo alternativo.

Valida que la imagen se haya cargado correctamente, lanzando excepción en caso contrario.

Selecciona un bloque de frames correspondiente al número del robot, asegurando que el índice esté dentro de un rango válido.

Extrae y almacena en el vector frames los 6 frames correspondientes al robot.

Asigna el primer frame cargado al sprite para que se muestre inicialmente.
*/
void Robot::cargarImagen(int numeroRobot)
{
    const int ancho = 50, alto = 56;
    const int framesPorRobot = 6;

    QPixmap spriteSheet(":/images/robots1.png");
    if (spriteSheet.isNull())
        spriteSheet.load("imagenes/robots1.png");

    if (spriteSheet.isNull())
        throw std::runtime_error("Robot::cargarImagen - No se encontró robots1.png.");

    int bloque = qBound(0, numeroRobot - 1, 3); // Asegura que el índice esté entre 0-3
    const int frameInicial = bloque * framesPorRobot;

    frames.clear();
    for (int i = 0; i < framesPorRobot; ++i) {
        int x = (frameInicial + i) * ancho;
        frames.append(spriteSheet.copy(x, 0, ancho, alto));
    }

    if (!frames.isEmpty())
        sprite->setPixmap(frames[0]); // Primer frame por defecto
}

/**
@brief Inicializa la posición del robot y activa los temporizadores de movimiento y animación.

@param x Coordenada horizontal inicial donde se posicionará el robot.
@param y Coordenada vertical inicial donde se posicionará el robot.
@param xDestino Posición horizontal destino que el robot debe alcanzar antes de detenerse.

@details

Establece la posición inicial del sprite en la escena.

Asigna el destino horizontal para controlar el movimiento.

Inicia el temporizador que mueve el robot periódicamente cada 60 ms.

Inicia el temporizador que actualiza la animación del robot cada 120 ms.
*/
void Robot::iniciar(int x, int y, int xDestino)
{
    destinoX = xDestino;
    sprite->setPos(x, y);
    timerMovimiento->start(60);
    timerAnimacion->start(120);
}

/**
@brief Desplaza el robot horizontalmente hacia la izquierda hasta alcanzar su destino.

@details

Mueve el sprite del robot una cantidad fija en dirección negativa (izquierda) según la velocidad establecida.

Cuando la posición horizontal actual alcanza o pasa el destino definido, detiene el movimiento y resetea el sprite al frame inicial.

@note El movimiento se realiza periódicamente mediante un temporizador.
*/
void Robot::mover()
{
    //qDebug() << "timer mover en robot llamado  "<<contador++;
    sprite->moveBy(-velocidad, 0);

    if (destinoX >= 0 && sprite->x() <= destinoX) {
        timerMovimiento->stop();
        sprite->setPixmap(frames[0]);
    }
}

/**
@brief Actualiza el frame del sprite para animar el robot.

@details

Si el vector de frames está vacío, no realiza ninguna acción.

Cuando modoMarcha está activado, alterna entre los frames 4 y 5 para simular la marcha.

Cuando modoMarcha está desactivado, recorre cíclicamente todos los frames disponibles.

Actualiza el sprite con el frame correspondiente al índice actual.
*/
void Robot::animar()
{

    //qDebug() << "timer animar en robot  llamado  "<<contador++;
    if (frames.isEmpty()) return;

    frameActual = (modoMarcha) ? ((frameActual == 4) ? 5 : 4)
                               : (frameActual + 1) % frames.size();
    sprite->setPixmap(frames[frameActual]);
}

/**
@brief Ejecuta una secuencia visual de despliegue del robot mediante cambios de frames con retardo.

@details

Si el robot ya fue desplegado, no realiza ninguna acción.

Marca el robot como desplegado para evitar repeticiones.

Utiliza una secuencia predefinida de índices de frames para mostrar diferentes poses o estados.

Emplea temporizadores QTimer::singleShot para espaciar los cambios de frame con un retardo fijo (1500 ms) entre cada uno.

Actualiza el sprite con el frame correspondiente en cada llamada retardada, creando una animación escalonada.
*/
void Robot::desplegarRobot()
{
    if (desplegado) return;
    desplegado = true;

    static const QVector<int> orden = {0, 1, 3, 2, 5, 4};
    const int delay = 1500;

    for (int i = 0; i < orden.size(); ++i) {
        QTimer::singleShot(i * delay, this, [this, i]() {

            //qDebug() << "timer single shot de mostrar robot en robot llamado  "<<contador++;
            sprite->setPixmap(frames[orden[i]]);
        });
    }
}

/**
@brief Detiene el movimiento horizontal del robot y activa la animación de marcha.

@details

Si el temporizador de movimiento está activo, lo detiene.

Cambia el modo a marcha, configurando el frame actual en 4 para comenzar la animación específica de marcha.

Actualiza el sprite con el frame inicial de marcha.

Si el temporizador de animación no está activo, lo inicia con un intervalo de 120 ms para actualizar los frames de marcha periódicamente.
*/
void Robot::detenerMvtoRobot()
{
    if (timerMovimiento && timerMovimiento->isActive())
        timerMovimiento->stop();

    modoMarcha = true;
    frameActual = 4;
    sprite->setPixmap(frames[frameActual]);

    if (timerAnimacion && !timerAnimacion->isActive())
        timerAnimacion->start(120);
}

/**
@brief Carga los frames de animación específicos para el robot del Nivel 2.

@details

Carga la hoja de sprites robot.png desde los recursos.

Valida que la imagen se haya cargado correctamente, lanzando una excepción si falla.

Divide la imagen en frames individuales con tamaño fijo (100x150 px).

Almacena todos los frames en el vector framesRobot2.

Establece el sprite inicial en el frame 4 y asigna framesRobot2 al vector general frames para uso en animaciones.
*/
void Robot::cargarRobot2()
{
    const int anchoFrame = 100, altoFrame = 150;

    QPixmap sheet(":/images/robot.png");
    if (sheet.isNull())
        throw std::runtime_error("Robot::cargarRobot2 - No se encontró robot.png.");

    int numFrames = sheet.width() / anchoFrame;
    framesRobot2.clear();
    for (int i = 0; i < numFrames; ++i)
        framesRobot2.append(sheet.copy(i * anchoFrame, 0, anchoFrame, altoFrame));

    if (!framesRobot2.isEmpty()) {
        sprite->setPixmap(framesRobot2[4]);
        frames = framesRobot2;
    }
}

/**
@brief Inicia el ciclo automático de ataques del robot con animación y lanzamiento de explosiones.

@details

Crea un temporizador que dispara periódicamente (cada 1000 ms) una animación de ataque mediante la secuencia de frames especificada.

Cambia el frame del sprite en cada llamada para simular el ataque.

Cada vez que la animación completa un ciclo, crea una nueva explosión en la posición del arma del robot.

Alterna el tipo de movimiento de la explosión entre parabólico y movimiento rectilíneo uniforme (MRU) en cada ataque.

Añade la explosión a la lista interna para su posterior gestión y liberación.
*/
void Robot::iniciarAtaques()
{
    timerAtaque = new QTimer(this);
    connect(timerAtaque, &QTimer::timeout, this, [=]() {

        //qDebug() << "timer ataque de robot en robot llamado  "<<contador++;
        static QVector<int> framesDisparo = {0, 1, 2, 3, 4};
        static int indexFrame = 0;

        sprite->setPixmap(framesRobot2[framesDisparo[indexFrame]]);
        indexFrame = (indexFrame + 1) % framesDisparo.size();

        if (indexFrame == 0) {
            Explosion* explosion = new Explosion(scene, this);
            ListaExplosiones.append(explosion);
            explosion->setTipoMovimiento(usarParabolico ? Explosion::Parabolico : Explosion::MRU);
            usarParabolico = !usarParabolico;

            QPointF posArma = sprite->scenePos()
                              + QPointF(sprite->pixmap().width() * -0.35,
                                        sprite->pixmap().height() * 0.9);
            explosion->setPosicionInicial(posArma);
            explosion->lanzar();

        }
    });

    timerAtaque->start(1000);
}

/**
@brief Ejecuta una secuencia rápida de animación seguida de un disparo de explosión.

@details

Verifica que haya al menos 5 frames disponibles para la animación.

Cambia el sprite del robot a diferentes frames en intervalos de 200 ms usando temporizadores QTimer::singleShot.

Al tercer temporizador, dispara una explosión con movimiento parabólico y cambia el sprite al frame final de la secuencia.
*/
void Robot::animarYDisparar()
{
    if (framesRobot2.size() < 5) return;

    sprite->setPixmap(framesRobot2[0]);
    QTimer::singleShot(200, this, [this]() {

        //qDebug() << "timer 1 single shot en robot llamado  "<<contador++;
        sprite->setPixmap(framesRobot2[1]); });
    QTimer::singleShot(400, this, [this]() {

        //qDebug() << "timer 2 single shot en robot llamado  "<<contador++;
        sprite->setPixmap(framesRobot2[2]); });
    QTimer::singleShot(600, this, [this]() {

        //qDebug() << "timer 3 single shot en robot llamado  "<<contador++;
        dispararExplosion(true);
        sprite->setPixmap(framesRobot2[4]);
    });
}

/**
@brief Crea y lanza una explosión desde la posición actual del robot.

@param parabolica Booleano que determina si el movimiento de la explosión será parabólico (true) o movimiento rectilíneo uniforme (false).

@details

Instancia un objeto Explosion y lo añade a la lista interna para su gestión.

Configura el tipo de movimiento según el parámetro parabolica.

Establece la posición inicial de la explosión en un punto relativo al sprite del robot (centro horizontal y un poco hacia abajo).

Ejecuta el lanzamiento de la explosión.
*/
void Robot::dispararExplosion(bool parabolica)
{
    Explosion* ex = new Explosion(scene, this);
    ListaExplosiones.append(ex);  //agregar a la lista
    ex->setTipoMovimiento(parabolica ? Explosion::Parabolico : Explosion::MRU);

    QPointF pos = sprite->pos() + QPointF(sprite->boundingRect().width() / 2, 60);
    ex->setPosicionInicial(pos);
    ex->lanzar();
}

/**
@brief Carga los frames de animación correspondientes a la muerte del robot.

@details

Verifica si los frames de muerte ya están cargados para evitar cargas duplicadas.

Carga la hoja de sprites murioRobot.png desde los recursos, lanzando excepción si no se encuentra.

Extrae 7 frames individuales de tamaño 120x150 píxeles para la animación de muerte.

Almacena los frames en el vector framesMuerte.
*/
void Robot::cargarFramesMuerte()
{
    // Si ya están cargados, no repetir
    if (!framesMuerte.isEmpty()) return;

    // Cargar hoja de sprites de muerte
    QPixmap sheet(":/images/murioRobot.png");
    if (sheet.isNull())
        throw std::runtime_error("Robot::cargarFramesMuerte - No se encontró murioRobot.png.");

    const int numFrames = 7;
    const int ancho = 120;
    const int alto  = 150;

    // Extraer los frames individuales del sprite sheet
    for (int i = 0; i < numFrames; ++i)
        framesMuerte.append(sheet.copy(i * ancho, 0, ancho, alto));
}

/**
@brief Detiene el temporizador de ataques automáticos del robot y libera su memoria de forma segura.

@details

Detiene el temporizador si está activo para cesar el ciclo de ataques.

Llama a deleteLater() para garantizar la eliminación segura en el ciclo de eventos de Qt.

Asigna el puntero a nullptr para evitar referencias colgantes.
*/
void Robot::detenerAtaques()
{
    if (timerAtaque) {
        timerAtaque->stop();           // Detener ciclo
        timerAtaque->deleteLater();    // Liberación segura en el ciclo de eventos
        timerAtaque = nullptr;
    }
}

/**
@brief Inicia la secuencia de muerte visual y lógica del robot.

@details

Evita ejecutar la secuencia si el robot ya está marcado como muerto.

Detiene todos los temporizadores relacionados con movimiento, animación y ataques.

Carga los frames de animación de muerte si aún no están cargados.

Inicializa la animación de muerte mostrando el primer frame.

Crea un temporizador que avanza los frames de muerte periódicamente (cada 500 ms).

Cuando la animación termina, emite la señal robotMurio() para notificar a otras partes del programa (por ejemplo, Nivel2).
*/
void Robot::murioRobot()
{
    // Evitar reinicio si ya ha muerto
    if (estaMuerto) return;
    estaMuerto = true;

    // Detener cualquier actividad visual o lógica del robot
    if (timerMovimiento) timerMovimiento->stop();
    if (timerAnimacion)  timerAnimacion->stop();
    if (timerAtaque)     timerAtaque->stop();

    // Cargar frames de muerte (si no lo estaban)
    cargarFramesMuerte();
    frameMuerte = 0;  // Reinicio de índice

    // Asignar primer sprite de muerte
    if (framesMuerte.size() > 0)
        sprite->setPixmap(framesMuerte[frameMuerte]);

    // Crear temporizador para avanzar frame por frame de muerte
    if (!timerMuerte) {
        timerMuerte = new QTimer(this);
        connect(timerMuerte, &QTimer::timeout, this, [this]() {

            //qDebug() << "timer muerte en robot llamado  "<<contador++;
            ++frameMuerte;

            // Si se alcanza el último frame, detener animación
            if (frameMuerte >= framesMuerte.size()) {
                timerMuerte->stop();
                emit robotMurio();  // Señal para que Nivel2 actúe
                return;
            }

            // Actualizar el sprite al siguiente frame de muerte
            sprite->setPixmap(framesMuerte[frameMuerte]);
        });
    }

    timerMuerte->start(500); // 1 segundo por frame
}

