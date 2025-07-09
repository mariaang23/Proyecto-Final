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

    void iniciarAtaques(); // método para iniciar los disparos periódicos

    QGraphicsPixmapItem *getSprite() const   { return sprite; }
    const QVector<QPixmap>& getFramesNivel2() const { return framesRobot2; }

    void detenerAtaques();
    void murioRobot();//sprites de robot muerto

private slots:
    void mover();
    void animar();

signals:
    void robotMurio();  // Para avisar a Nivel2 cuando termine la animación


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

    //para la muerte del robot cuando goku le lanza el kamehameha
    void cargarFramesMuerte();// carga la imagen de los 7 frames
    QVector<QPixmap> framesMuerte;// frames de la muerte
    QTimer *timerMuerte = nullptr;// temporizador para avanzar frames
    int frameMuerte; // indice del frame actual
    bool estaMuerto = false;// evita reiniciar la muerte
};

#endif // ROBOT_H
