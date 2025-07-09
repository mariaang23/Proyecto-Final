#ifndef CARRO_H
#define CARRO_H

#include "obstaculo.h"
#include <QTimer>

class Carro : public obstaculo
{
    Q_OBJECT

public:
    static int contCarro;
    static int contador;

    Carro(QGraphicsScene *scene, int velocidad, QObject *parent = nullptr);
    ~Carro();
    void iniciar(int x, int y) override;   // Posicionamiento
    void rotar();                          // Rotacion
    bool estaGirando() const;             //Esta girando?
    void iniciarMovimientoEspiral(float _posXpatada);  // se llama cuando toca Goku
    void actualizarMovimiento();      // se llama cada 20 ms

    bool espiralHecha = false; //determina que ya se realizo la espiral(evitar que se haga mas de una)
    bool haLlegadoAlSuelo() const; //esta haciendo la espiral?


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

    //movimiento espirar del carro
    int fase = 3;     // empieza en fase 3 = Terminado
    float tiempo = 0.0f;      // tiempo en segundos
    QPointF inicio;   // punto de inicio de la fase

    // valores para controlar la forma del movimiento
    float g      = 500.0f;   // gravedad
    float vx     = 80.0f;// velocidad horizontal
    float vy     = 350.0f; // velocidad hacia arriba
    float radio  = 120.0f;   // radio del giro
    float tiempoGiro = 2.0f;  // cuanto dura el giro
    float ySuelo = 500.0f;   // altura del suelo
    QTimer *timerEspiral;  // timer para mvto
    float posXpatada=0;

};

#endif // CARRO_H

