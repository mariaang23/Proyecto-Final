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
    static int contador;

    explicit Robot(QGraphicsScene *scene, int velocidad, int numeroRobot, QObject *parent = nullptr);
    Robot(QGraphicsScene *scene, QObject *parent);
    ~Robot();

    void iniciar(int x, int y, int xDestino = -1);
    void desplegarRobot();
    void detenerMvtoRobot();
    void iniciarAtaques();
    void murioRobot();

    QGraphicsPixmapItem *getSprite() const { return sprite; }
    const QVector<QPixmap>& getFramesNivel2() const { return framesRobot2; }

    void detenerAtaques();

signals:
    void robotMurio();

private slots:
    void mover();
    void animar();

private:
    void cargarImagen(int numeroRobot);
    void cargarRobot2();
    void animarYDisparar();
    void dispararExplosion(bool parabolica);
    void cargarFramesMuerte();
    void eliminarExplosiones();


    QGraphicsScene *scene = nullptr;
    QGraphicsPixmapItem *sprite = nullptr;
    QVector<QPixmap> frames;
    QVector<QPixmap> framesRobot2;
    QVector<QPixmap> framesMuerte;
    int frameActual = 0;
    int velocidad = 0;
    int destinoX = -1;
    bool desplegado = false;
    bool modoMarcha = false;
    bool usarParabolico = true;
    bool estaMuerto = false;

    QTimer *timerMovimiento = nullptr;
    QTimer *timerAnimacion = nullptr;
    QTimer *timerAtaque = nullptr;
    QTimer *timerMuerte = nullptr;
    int frameMuerte = 0;

    //explosiones
    QList<Explosion*> ListaExplosiones;

};

#endif // ROBOT_H
