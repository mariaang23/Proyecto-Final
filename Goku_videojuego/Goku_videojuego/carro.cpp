#include "carro.h"

// Inicialización del contador
int Carro::contador = 0;
int Carro::contCarro=0;

Carro::Carro(QGraphicsScene *scene, int velocidad, QObject *parent)
    : obstaculo(scene, obstaculo::Roca, velocidad, parent), anguloActual(0), girando(false)
{
    imagenCarro = QPixmap(":/images/carro_rojo.png"); // sprite sheet del carro desde los recursos

    cuadroActual = 0; // frmae[0]

    anchoCuadro = 400;
    altoCuadro = 251;

    // Extrae el primer cuadro desde la imagen y lo aplica al sprite
    QPixmap cuadro = imagenCarro.copy(cuadroActual * anchoCuadro, 0, anchoCuadro, altoCuadro);
    sprite->setPixmap(cuadro);

    sprite->setData(0, "carro");

    //conexion para el movimiento
    timerEspiral = new QTimer(this);
    connect(timerEspiral, &QTimer::timeout, this, &Carro::actualizarMovimiento);
    contCarro+=1;
    //qDebug()<<"creo carro constructor "<<contCarro;

}

Carro::~Carro() {
    //qDebug() << "Destructor de carro llamado";
    contCarro-=1;
    //qDebug()<<"libero carro desstructor "<<contCarro;
    timerEspiral->stop();
    delete timerEspiral; // Eliminar el temporizador creado con new
}

void Carro::iniciar(int x, int y)
{
    sprite->setPos(x, y);
}

void Carro::rotar()
{
    // Si el carro no está girando, activa el temporizador para comenzar la animación
    if (!girando) {
        girando = true;                // Marca que ya está girando
        timerRotacion->start(150);    // Cambia el cuadro de animación cada 150 milisegundos
    }
}

bool Carro::estaGirando() const {
    return girando;
}

void Carro::iniciarMovimientoEspiral(float _posXpatada)
{
    if (espiralHecha || fase != 3) return;         // si ya estaba girando, no hacer nada

    posXpatada = _posXpatada; //se toma por referencia en nivel 1 la posicion en x que quedo goku cuando pateo

    //cambiamos los valores de inicio para que se realice el movimiento mas lejos de goku y no pase por encima
    inicio.setX(posXpatada + 1000); // Ajusta la distancia
    inicio.setY(sprite->y());      // Mantener la misma altura inicial

    espiralHecha = true;
    fase   = 0;                    // comenzamos con la fase de subida
    tiempo = 0.0f;
    inicio = sprite->pos();               // guardar posicion actual
    timerEspiral->start(20);   // llama actualizarMovimiento() cada 20 ms
}

void Carro::actualizarMovimiento()
{
    //qDebug() << "timer carro actualizar mvto llamado timerEspiral "<<contador++;
    float dt = 0.020f;  // 20 milisegundos
    tiempo += dt;

    if (fase == 0)  // Subida
    {
        // el numero 127 sale de la resta de las posiciones finales para evitar que el carro pase por encima de goku
        float x = 127 + inicio.x() + vx * tiempo;
        float y = inicio.y() - vy * tiempo + 0.5f * g * tiempo * tiempo;
        sprite->setPos(x, y);

        if (tiempo >= vy / g) {
            inicio = sprite->pos();   // guardamos la cima como centro del círculo
            tiempo = 0.0f;
            fase = 1;
        }
    }
    else if (fase == 1)  // Círculo
    {
        this->animarRotacion();
        float w = 2.0f * 3.1416f / tiempoGiro;  // velocidad angular
        float x = inicio.x() + radio * cos(w * tiempo);
        float y = inicio.y() - radio * sin(w * tiempo);
        sprite->setPos(x, y);

        if (tiempo >= tiempoGiro) {
            inicio = sprite->pos();  // guardamos el punto final del circulo
            tiempo = 0.0f;
            fase = 2;
        }
    }
    else if (fase == 2)  // Caída
    {
        float y = inicio.y() + 0.5f * g * tiempo * tiempo;
        sprite->setY(y);

        //float posXcarroFinal=sprite->x();
        //qDebug() << "poscion de carro final en x = "<<posXcarroFinal;

        if (y >= ySuelo) {
            sprite->setY(ySuelo);
            fase = 3;
            timerEspiral->stop();  // se acabó la animación
        }
    }
}


void Carro::animarRotacion()
{
    // frame ya existente:
    cuadroActual = (cuadroActual + 1) % 3;
    QPixmap cuadro = imagenCarro.copy(cuadroActual * anchoCuadro, 0,
                                      anchoCuadro, altoCuadro);
    sprite->setPixmap(cuadro);

}

bool Carro::haLlegadoAlSuelo() const
{
    return fase == 3 && sprite->y() >= 500; //ha llegado al suelo
}
