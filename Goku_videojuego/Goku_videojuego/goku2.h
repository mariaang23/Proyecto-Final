#ifndef GOKU2_H
#define GOKU2_H

#include "goku.h"

class Goku2 : public Goku
{
    Q_OBJECT
public:
    Goku2(QGraphicsScene *scene, int velocidad, int fotogWidth, int fotogHeight, QObject *parent = nullptr);
    ~Goku2();

    void detener() override;
    void iniciar(int x, int y) override;
    void setSueloY(float y);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void cargarImagen() override;
    void mover() override;

private:
    void actualizarSpriteSalto();
    void actualizarSpriteCaminar(bool derecha);
    void actualizarSalto();

    QTimer *timerSalto;
    int contadorCaminata;

    bool mvtoIzquierda;
    bool mvtoDerecha;
    bool mirandoDerecha;

    // Físicas del salto
    qreal yInicial;
    bool enSalto;
    float tiempoSalto;
    float gravedad;
    float sueloY;
    float velocidadVertical;
};

#endif // GOKU2_H
