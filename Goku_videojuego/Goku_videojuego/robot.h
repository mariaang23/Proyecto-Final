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
    ~Robot();

    void iniciar(int x, int y, int xDestino = -1);
    void desplegarRobot();
    void detenerMvtoRobot();

private slots:
    void mover();  // avanza hacia la izquierda
    void animar();   // cambia de frame

private:
    void cargarImagen(int numeroRobot);   // recorta los 6 frames correctos

    QGraphicsScene      *scene   = nullptr;
    QGraphicsPixmapItem *sprite  = nullptr;
    QVector<QPixmap>     frames;
    int                  frameActual = 0;
    int                  velocidad   = 0;

    QTimer *timerMovimiento = nullptr;
    QTimer *timerAnimacion  = nullptr;
    int  destinoX   = -1;   // -1 no se detiene
    bool desplegado = false;  // para no repetir secuencia
    bool modoMarcha = false; //para el ultimo momomento del nivel 1
};

#endif // ROBOT_H


