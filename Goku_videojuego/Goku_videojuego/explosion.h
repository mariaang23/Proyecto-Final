#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "obstaculo.h"
#include <QTimer>
#include <QPointF>

class Explosion : public obstaculo
{
    Q_OBJECT
public:
    static int contador;

    enum TipoMovimiento {
        Parabolico,
        MRU
    };

    static const QPointF posicionDisparo;  // por defecto

    Explosion(QGraphicsScene* scene, QObject* parent = nullptr);
    ~Explosion();

    void setTipoMovimiento(TipoMovimiento tipo);
    void setPosicionInicial(QPointF pos);
    void lanzar();

private:
    QTimer* timerMovimiento;
    QTimer* timerFrames;
    float velocidadX;
    float velocidadY;
    float gravedad;
    float tiempo;
    int frameActual;

    TipoMovimiento tipoMovimiento;
    QPointF posicionInicial;
};

#endif // EXPLOSION_H
