#include "robot.h"
#include "explosion.h"
#include "nivel2.h"
#include <QMessageBox>
#include <QDebug>
#include <stdexcept> // Excepciones estándar

// Inicialización del contador
int Robot::contador = 0;

// Constructor principal: crea un robot con animación base y movimiento horizontal
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

// Constructor alternativo: versión para Nivel2 con sprites distintos
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

// Destructor: detiene y elimina todos los timers y libera recursos gráficos
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
}
// Carga animaciones desde spriteSheet según el número del robot
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

// Inicia posición y activa animación y movimiento
void Robot::iniciar(int x, int y, int xDestino)
{
    destinoX = xDestino;
    sprite->setPos(x, y);
    timerMovimiento->start(60);
    timerAnimacion->start(120);
}

// Lógica de movimiento hacia la izquierda hasta alcanzar destino
void Robot::mover()
{
    //qDebug() << "timer mover en robot llamado  "<<contador++;
    sprite->moveBy(-velocidad, 0);

    if (destinoX >= 0 && sprite->x() <= destinoX) {
        timerMovimiento->stop();
        sprite->setPixmap(frames[0]);
    }
}

// Cambia de frame según modo normal o marcha
void Robot::animar()
{

    //qDebug() << "timer animar en robot  llamado  "<<contador++;
    if (frames.isEmpty()) return;

    frameActual = (modoMarcha) ? ((frameActual == 4) ? 5 : 4)
                               : (frameActual + 1) % frames.size();
    sprite->setPixmap(frames[frameActual]);
}

// Despliegue visual usando una secuencia específica de frames
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

// Detiene movimiento y activa ciclo de marcha visual
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

// 🤖 Carga sprites del robot para Nivel2
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

// Inicia disparos automáticos con alternancia de tipo de trayectoria
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
            explosion->setTipoMovimiento(usarParabolico ? Explosion::Parabolico : Explosion::MRU);
            usarParabolico = !usarParabolico;

            QPointF posArma = sprite->scenePos()
                              + QPointF(sprite->pixmap().width() * -0.35,
                                        sprite->pixmap().height() * 0.9);
            explosion->setPosicionInicial(posArma);
            explosion->lanzar();

            Nivel2* nivel2 = dynamic_cast<Nivel2*>(this->parent());
            if (nivel2)
                nivel2->agregarExplosion(explosion);
        }
    });

    timerAtaque->start(1000);
}

// Secuencia de animación rápida y disparo único
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

// Crea una explosión y la lanza desde el robot
void Robot::dispararExplosion(bool parabolica)
{
    Explosion* ex = new Explosion(scene, this);
    ex->setTipoMovimiento(parabolica ? Explosion::Parabolico : Explosion::MRU);

    QPointF pos = sprite->pos() + QPointF(sprite->boundingRect().width() / 2, 60);
    ex->setPosicionInicial(pos);
    ex->lanzar();
}

// Carga los 7 frames de la animación de muerte del robot
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

// Método auxiliar para detener ataques programados y liberar el temporizador
void Robot::detenerAtaques()
{
    if (timerAtaque) {
        timerAtaque->stop();           // Detener ciclo
        timerAtaque->deleteLater();    // Liberación segura en el ciclo de eventos
        timerAtaque = nullptr;
    }
}

// Ejecuta la secuencia visual de muerte del robot
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

