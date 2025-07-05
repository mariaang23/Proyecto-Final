#ifndef EXPLOSION_H
#define EXPLOSION_H

#include "obstaculo.h"

class Explosion : public obstaculo
{
    Q_OBJECT
public:
    // posicion fija de la boca del arma del robor
    // se declara static para que sea la misma para todas las explosiones
    static const QPointF posicionDisparo;

    // Constructor: crea los frames y arranca el timer
    Explosion(QGraphicsScene* scene,QObject* parent = nullptr);

};


#endif // EXPLOSION_H
