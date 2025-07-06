#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "obstaculo.h"
#include <QTimer>
#include <QPointF>

class Explosion : public obstaculo
{
    Q_OBJECT
public:
    enum TipoMovimiento {
        Parabolico,
        MRU
    };

    static const QPointF posicionDisparo;  // por defecto

    Explosion(QGraphicsScene* scene, QObject* parent = nullptr);

    void setTipoMovimiento(TipoMovimiento tipo);
    void setPosicionInicial(QPointF pos);
    void lanzar();  // inicia la animación y movimiento

private:
    QTimer* timerMovimiento;
    float velocidadX;
    float velocidadY;
    float gravedad;
    float tiempo;
    int frameActual;

    TipoMovimiento tipoMovimiento;
    QPointF posicionInicial;
};

#endif // EXPLOSION_H
