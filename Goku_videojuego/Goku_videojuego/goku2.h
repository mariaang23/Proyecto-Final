#ifndef GOKU2_H
#define GOKU2_H

#include "goku.h"
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

class Nivel2;
class Robot;

class Goku2 : public Goku {
    Q_OBJECT

public:
    static int contador;

    Goku2(QGraphicsScene* scene, int velocidad, int fotogWidth, int fotogHeight, Nivel2* nivel, QObject* parent = nullptr);
    ~Goku2();

    void cargarImagen() override;
    void iniciar(int x, int y) override;
    void detener() override;

    void setSueloY(float y);
    void animarMuerte();
    void iniciarKamehameha(float xObjetivo, Robot* robotObjetivo);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void mover() override;

private:
    void actualizarSalto();
    void detectarPocion();
    void actualizarSpriteCaminar(bool derecha);
    void actualizarSpriteSalto();
    void caminarHaciaRobot(float xObjetivo, Robot* robotObjetivo);
    void atacarRobot(Robot* robotObjetivo);
    void caminarHaciaIzquierda(float xDestino);

    QTimer* timerMovimiento;
    QTimer* timerSalto;
    QTimer* timerDanio;
    QTimer* timerMuerte;
    QTimer* animSalto;
    QTimer* avance;
    QTimer* animAtaque;
    QTimer* regreso;

    bool mvtoIzquierda;
    bool mvtoDerecha;
    bool enSalto;
    bool mirandoDerecha = true;
    bool puedeRecibirDanio = true;

    int tiempoSalto;
    float gravedad;
    float sueloY;
    float velocidadVertical;

    Nivel2* nivel2;

    //Efectos de sonido
    QMediaPlayer *salto;
    QAudioOutput *salidaSalto;
};

#endif // GOKU2_H
