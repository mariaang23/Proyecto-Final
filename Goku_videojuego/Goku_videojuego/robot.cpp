#include "robot.h"
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

    cargarImagen(numeroRobot);               // ← asigna frames y primer pixmap
    sprite->setScale(5.0);                   // tamaño final en pantalla
    sprite->setData(0, "robot");             // etiqueta para colisiones

    /* timers */
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout,
            this,             &Robot::mover);

    timerAnimacion = new QTimer(this);
    connect(timerAnimacion,  &QTimer::timeout,
            this,             &Robot::animar);
}

Robot::~Robot()
{
    qDebug() << "Destructor de robot llamado";
    delete sprite; // Eliminar el sprite creado con new

    // Detener y eliminar los temporizadores
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
    const int ancho           = 50;   // tamaño de cada cuadro
    const int alto            = 56;
    const int framesPorRobot  = 6;    // 6 imágenes por robot

    //cargar sprite
    QPixmap spriteSheet(":/images/robots1.png");
    if (spriteSheet.isNull())
        spriteSheet.load("imagenes/robots1.png");     // ruta alternativa

    if (spriteSheet.isNull()) {
        QMessageBox::critical(nullptr, "Error",
                              "robots1.png no encontrado. Revisa tu .qrc.");
        return;
    }

    //determinar los bloques
    int bloque = 0;      // robot 1 → bloque 0 (frames 0-5)
    switch (numeroRobot) {
    case 1: bloque = 0; break;      // 0-5
    case 2: bloque = 1; break;      // 6-11
    case 3: bloque = 2; break;      // 12-17
    case 4: bloque = 3; break;      // reserva para nivel 2 (18-23)
    default: bloque = 0; break;
    }

    const int frameInicial = bloque * framesPorRobot;

    //recortar frames
    frames.clear();
    for (int i = 0; i < framesPorRobot; ++i) {
        int x = (frameInicial + i) * ancho;
        frames.append(spriteSheet.copy(x, 0, ancho, alto));
    }

    //mostrar
    if (!frames.isEmpty())
        sprite->setPixmap(frames[0]);
}

void Robot::iniciar(int x, int y, int xDestino)
{
    destinoX = xDestino;                 // guarda el destino
    sprite->setPos(x, y);
    timerMovimiento->start(60);
    timerAnimacion->start(120);
}


void Robot::mover()
{
    sprite->moveBy(-velocidad, 0);

    //detener cuando alcance la x destino
    if (destinoX >= 0 && sprite->x() <= destinoX) {
        timerMovimiento->stop();         // deja de desplazarse
        // Opcional: cambiar a frame “quieto”
        sprite->setPixmap(frames[0]);
    }
}

void Robot::animar()
{
    if (frames.isEmpty()) return;

    if (modoMarcha) {
        //simular modo marcha
        frameActual = (frameActual == 4) ? 5 : 4;
    } else {
        //animacion normal
        frameActual = (frameActual + 1) % frames.size();
    }
    sprite->setPixmap(frames[frameActual]);
}


void Robot::desplegarRobot()
{
    if (desplegado) return; // evita la repeticion
    desplegado = true;

    //vector para los frames secuencia
    static const QVector<int> orden = {0,1,3,2,5,4};
    const int delay = 1200; // ms entre cuadros

    for (int i = 0; i < orden.size(); ++i) {
        QTimer::singleShot(i * delay, this, [this, i]() {
            sprite->setPixmap(frames[ orden[i] ]);
        });
    }
}

void Robot::detenerMvtoRobot()
{
    //parar el desplazamiento
    if (timerMovimiento->isActive())
        timerMovimiento->stop();

    //cambiar a los frames modo marcha
    modoMarcha = true;
    frameActual = 4; // frame 4 "parado"
    sprite->setPixmap(frames[frameActual]);

    //siga activa la animacion
    if (!timerAnimacion->isActive())
        timerAnimacion->start(120);   // la misma cadencia que usabas
}
