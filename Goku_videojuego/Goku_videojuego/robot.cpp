#include "robot.h"
#include "explosion.h"
#include "nivel2.h"
#include <QMessageBox>

Robot::Robot(QGraphicsScene *scene,
             int velocidad,
             int numeroRobot,
             QObject *parent)
    : QObject(parent),
    scene(scene),
    velocidad(velocidad)
{
    sprite = new QGraphicsPixmapItem;
    scene->addItem(sprite);

    cargarImagen(numeroRobot);
    sprite->setScale(5.0);
    sprite->setData(0, "robot");

    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout,
            this,             &Robot::mover);

    timerAnimacion = new QTimer(this);
    connect(timerAnimacion,  &QTimer::timeout,
            this,             &Robot::animar);
}

Robot::Robot(QGraphicsScene *scene, QObject *parent)
    : QObject(parent),
    scene(scene)
{
    sprite = new QGraphicsPixmapItem;
    scene->addItem(sprite);

    cargarRobot2();
    sprite->setScale(0.5);
    sprite->setData(0, "robot_nivel2");
}

Robot::~Robot()
{
    qDebug() << "Destructor de robot llamado";
    delete sprite;

    if (timerMovimiento) {
        timerMovimiento->stop();
        delete timerMovimiento;
    }
    if (timerAnimacion) {
        timerAnimacion->stop();
        delete timerAnimacion;
    }
}

void Robot::cargarImagen(int numeroRobot)
{
    const int ancho = 50;
    const int alto = 56;
    const int framesPorRobot = 6;

    QPixmap spriteSheet(":/images/robots1.png");
    if (spriteSheet.isNull())
        spriteSheet.load("imagenes/robots1.png");

    if (spriteSheet.isNull()) {
        QMessageBox::critical(nullptr, "Error",
                              "robots1.png no encontrado. Revisa tu .qrc.");
        return;
    }

    int bloque = 0;
    switch (numeroRobot) {
    case 1: bloque = 0; break;
    case 2: bloque = 1; break;
    case 3: bloque = 2; break;
    case 4: bloque = 3; break;
    default: bloque = 0; break;
    }

    const int frameInicial = bloque * framesPorRobot;

    frames.clear();
    for (int i = 0; i < framesPorRobot; ++i) {
        int x = (frameInicial + i) * ancho;
        frames.append(spriteSheet.copy(x, 0, ancho, alto));
    }

    if (!frames.isEmpty())
        sprite->setPixmap(frames[0]);
}

void Robot::iniciar(int x, int y, int xDestino)
{
    destinoX = xDestino;
    sprite->setPos(x, y);
    timerMovimiento->start(60);
    timerAnimacion->start(120);
}

void Robot::mover()
{
    sprite->moveBy(-velocidad, 0);

    if (destinoX >= 0 && sprite->x() <= destinoX) {
        timerMovimiento->stop();
        sprite->setPixmap(frames[0]);
    }
}

void Robot::animar()
{
    if (frames.isEmpty()) return;

    if (modoMarcha) {
        frameActual = (frameActual == 4) ? 5 : 4;
    } else {
        frameActual = (frameActual + 1) % frames.size();
    }
    sprite->setPixmap(frames[frameActual]);
}

void Robot::desplegarRobot()
{
    if (desplegado) return;
    desplegado = true;

    static const QVector<int> orden = {0,1,3,2,5,4};
    const int delay = 1500;

    for (int i = 0; i < orden.size(); ++i) {
        QTimer::singleShot(i * delay, this, [this, i]() {
            sprite->setPixmap(frames[ orden[i] ]);
        });
    }
}

void Robot::detenerMvtoRobot()
{
    if (timerMovimiento->isActive())
        timerMovimiento->stop();

    modoMarcha = true;
    frameActual = 4;
    sprite->setPixmap(frames[frameActual]);

    if (!timerAnimacion->isActive())
        timerAnimacion->start(120);
}

void Robot::cargarRobot2()
{
    const int anchoFrame = 100;
    const int altoFrame  = 150;

    QPixmap sheet(":/images/robot.png");

    if (sheet.isNull()) {
        QMessageBox::critical(nullptr, "Error",
                              "No se encontró robot.png en el .qrc ni en /imagenes");
        return;
    }

    const int numFrames = sheet.width() / anchoFrame;
    framesRobot2.clear();
    for (int i = 0; i < numFrames; ++i)
        framesRobot2.append(sheet.copy(i * anchoFrame, 0, anchoFrame, altoFrame));

    if (!framesRobot2.isEmpty()) {
        sprite->setPixmap(framesRobot2[4]);
        frames = framesRobot2;
    }
}

void Robot::iniciarAtaques()
{
    timerAtaque = new QTimer(this);
    connect(timerAtaque, &QTimer::timeout, this, [=]() {
        // 1. Animar el disparo (cambio de frame)
        static QVector<int> framesDisparo = {0, 1, 2, 3, 4};  // simulación
        static int indexFrame = 0;
        sprite->setPixmap(framesRobot2[framesDisparo[indexFrame]]);
        indexFrame = (indexFrame + 1) % framesDisparo.size();

        // 2. Solo disparar cuando finaliza la animación (frame final)
        if (indexFrame == 0) {
            // Crear una nueva explosión
            Explosion* explosion = new Explosion(scene, this);

            // Alternar tipo de movimiento
            if (usarParabolico)
                explosion->setTipoMovimiento(Explosion::Parabolico);
            else
                explosion->setTipoMovimiento(Explosion::MRU);

            usarParabolico = !usarParabolico; // alternar para el próximo

            // Posicionar la explosión justo en el arma del robot
            QPointF posArma = sprite->scenePos() + QPointF(sprite->pixmap().width() * -0.35, // ancho para que salga explosion del arma
                                                           sprite->pixmap().height() * 0.9); // alto para que salga explosion del arma
            explosion->setPosicionInicial(posArma);
            explosion->lanzar();

            // Aquí es donde se registra en Nivel2
            Nivel2* nivel2 = dynamic_cast<Nivel2*>(this->parent());
            if (nivel2)
                nivel2->agregarExplosion(explosion);
        }
    });

    timerAtaque->start(1000);  // cada 1 s
}


void Robot::animarYDisparar()
{
    // Animar frames para levantar el brazo: 0 → 1 → 2
    if (framesRobot2.size() < 5) return;

    sprite->setPixmap(framesRobot2[0]); // inicio
    QTimer::singleShot(200, this, [this]() {
        sprite->setPixmap(framesRobot2[1]); // brazo medio
    });
    QTimer::singleShot(400, this, [this]() {
        sprite->setPixmap(framesRobot2[2]); // brazo arriba
    });
    QTimer::singleShot(600, this, [this]() {
        dispararExplosion(true); // Parabólica. Cambia a false si prefieres MRU.
        sprite->setPixmap(framesRobot2[4]); // regresa al frame de espera
    });
}

void Robot::dispararExplosion(bool parabolica)
{
    Explosion* ex = new Explosion(scene, this);
    ex->setTipoMovimiento(parabolica ? Explosion::Parabolico : Explosion::MRU);

    // Posición frente al robot
    QPointF pos = sprite->pos() + QPointF(sprite->boundingRect().width() / 2, 60);
    ex->setPosicionInicial(pos);
    ex->lanzar();
}

void Robot::cargarFramesMuerte()
{
    if (!framesMuerte.isEmpty()) return;

    QPixmap sheet(":/images/murioRobot.png");
    const int numFrames = 7;
    const int ancho= 200;
    const int alto= 150;

    for (int i = 0; i < numFrames; ++i)
        framesMuerte.append(sheet.copy(i * ancho, 0, ancho, alto));
}

void Robot::murioRobot()
{
    if (estaMuerto) return; //evitar repetir movto si ya murio
    estaMuerto = true;

    //detener el mvto anterior
    if (timerMovimiento) timerMovimiento->stop();
    if (timerAnimacion)  timerAnimacion->stop();
    if (timerAtaque)     timerAtaque->stop();

    //cargar los frames
    cargarFramesMuerte();
    frameMuerte = 0;//frame inicial 0
    sprite->setPixmap(framesMuerte[frameMuerte]);

    // timer de muerte
    if (!timerMuerte) {
        timerMuerte = new QTimer(this);
        connect(timerMuerte, &QTimer::timeout, this, [this]() {
            ++frameMuerte;
            //detener timer cuando llega al ultimo frame
            if (frameMuerte >= 7) {
                timerMuerte->stop();
                return;
            }
            //cambiar el sprite al siguiente
            sprite->setPixmap(framesMuerte[frameMuerte]);
        });
    }
    timerMuerte->start(1000); //tiempo entre cada sprite 1s
}
