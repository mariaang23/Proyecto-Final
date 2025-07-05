#ifndef ROBOT_H
#define ROBOT_H

#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QTimer>

class QGraphicsScene;
class Robot : public QObject
{
    Q_OBJECT
public:
    explicit Robot(QGraphicsScene *scene, int velocidad, int numeroRobot, QObject *parent = nullptr);
    //sobre carga nivel 2
    Robot(QGraphicsScene *scene, QObject *parent);
    ~Robot();

    void iniciar(int x, int y, int xDestino = -1);
    void desplegarRobot();
    void detenerMvtoRobot();

    // Accesos que necesita Nivel 2
    QGraphicsPixmapItem *getSprite() const   { return sprite; }
    const QVector<QPixmap>& getFramesNivel2() const { return framesRobot2; }

private slots:
    void mover();  // avanza hacia la izquierda
    void animar();   // cambia de frame

private:
    void cargarImagen(int numeroRobot);   // recorta los 6 frames correctos nivel1
    void cargarRobot2(); //para el nivel 2

    QGraphicsScene      *scene   = nullptr;
    QGraphicsPixmapItem *sprite  = nullptr;
    QVector<QPixmap>     frames; //vector para frames robots nivel 1
    QVector<QPixmap> framesRobot2;  //vector para almacenar frames de robot2
    int frameActual = 0;
    int velocidad   = 0;

    QTimer *timerMovimiento = nullptr;
    QTimer *timerAnimacion  = nullptr;
    int  destinoX   = -1;   // -1 no se detiene
    bool desplegado = false;  // para no repetir secuencia
    bool modoMarcha = false; //para el ultimo momomento del nivel 1
};

#endif // ROBOT_H


