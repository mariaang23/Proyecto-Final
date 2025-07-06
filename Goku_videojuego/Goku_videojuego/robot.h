#ifndef ROBOT_H
#define ROBOT_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>

class Explosion;

class Robot : public QObject
{
    Q_OBJECT
public:
    explicit Robot(QGraphicsScene *scene, int velocidad, int numeroRobot, QObject *parent = nullptr);
    Robot(QGraphicsScene *scene, QObject *parent);
    ~Robot();

    void iniciar(int x, int y, int xDestino = -1);
    void desplegarRobot();
    void detenerMvtoRobot();

    void iniciarAtaques(); // método para iniciar los disparos periódicos

    QGraphicsPixmapItem *getSprite() const   { return sprite; }
    const QVector<QPixmap>& getFramesNivel2() const { return framesRobot2; }

private slots:
    void mover();
    void animar();

private:
    void cargarImagen(int numeroRobot);
    void cargarRobot2();

    void animarYDisparar(); // <-- Nuevo método

    void dispararExplosion(bool parabolica); // Llama a Explosion

    QGraphicsScene      *scene   = nullptr;
    QGraphicsPixmapItem *sprite  = nullptr;
    QVector<QPixmap>     frames;
    QVector<QPixmap>     framesRobot2;
    int frameActual = 0;
    int velocidad   = 0;

    QTimer *timerMovimiento = nullptr;
    QTimer *timerAnimacion  = nullptr;
    QTimer *timerAtaque     = nullptr;  //  Nuevo timer para disparo

    int  destinoX   = -1;
    bool desplegado = false;
    bool modoMarcha = false;
    bool usarParabolico = true;
};

#endif // ROBOT_H
