#ifndef GOKU2_H
#define GOKU2_H

#include "goku.h"

class Nivel2; // Declaración adelantada

class Goku2 : public Goku
{
    Q_OBJECT
public:
    Goku2(QGraphicsScene *scene, int velocidad, int fotogWidth, int fotogHeight, Nivel2* nivel, QObject *parent = nullptr);
    ~Goku2();

    void detener() override;
    void iniciar(int x, int y) override;
    void cargarImagen() override;
    void setSueloY(float y);
    void animarMuerte();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mover() override;

private:
    void actualizarSpriteSalto();
    void actualizarSpriteCaminar(bool dir);
    void actualizarSalto();
    void detectarPocion();

    QTimer *timerSalto;

    int contadorCaminata;
    bool mvtoIzquierda;
    bool mvtoDerecha;
    bool mirandoDerecha;

    // Física del salto
    qreal yInicial;
    bool enSalto;
    float tiempoSalto;
    float gravedad;
    float sueloY;
    float velocidadVertical;

    Nivel2* nivel2; // Referencia al nivel actual
};

#endif // GOKU2_H
