#ifndef CARRO_H
#define CARRO_H

#include "obstaculo.h"
#include <QTimer>

class Carro : public obstaculo
{
    Q_OBJECT

public:
    Carro(QGraphicsScene *scene, int velocidad, QObject *parent = nullptr);
    void iniciar(int x, int y) override;   // Posicionamiento
    void rotar();                          // Rotacion
    bool estaGirando() const;             //Esta girando?
    void empezarEspiral();     //iniciar espiral(mvto circular)

private slots:
    void animarRotacion();                //implementar

private:
    QPixmap imagenCarro;           // Imagen completa que contiene las vistas del carro
    int cuadroActual;              // indice del cuadro actual
    int anchoCuadro;               // Ancho de cada cuadro
    int altoCuadro;                // Alto de cada cuadro
    QTimer *timerRotacion;         // Temporizador para animacion
    int anguloActual;              // angulo
    bool girando;
};

#endif // CARRO_H

