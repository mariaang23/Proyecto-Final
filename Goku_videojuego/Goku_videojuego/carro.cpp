#include "carro.h"

// Inicialización del contador
int Carro::contador = 0;
int Carro::contCarro=0;

/**
@brief Constructor de la clase Carro.
Inicializa un objeto Carro como obstáculo especial de tipo Roca en la escena
especificada. Carga su sprite desde un sprite-sheet horizontal ("carro_rojo.png"),
establece el primer frame visible y prepara el temporizador que controlará su
animación de movimiento en espiral (usada cuando Goku lo patea en el Nivel 1).
Además, etiqueta el sprite como "carro" para facilitar las detecciones de colisión
y contabiliza la creación del carro mediante un contador estático.
@param scene Escena gráfica donde se insertará el carro. No puede ser nula.
@param velocidad Velocidad inicial del carro (heredada de obstáculo) – no se usa
             en este constructor, pues el movimiento espiral se maneja internamente.
@param parent Objeto padre en la jerarquía de Qt. Si no se especifica, se asume nullptr.
*/
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

/**
@brief Destructor de la clase Carro.
Libera los recursos asociados al carro, deteniendo el temporizador encargado de su
movimiento en espiral y eliminando el objeto correspondiente. Reduce en uno el contador
estático que lleva la cuenta total de instancias de Carro existentes, lo que facilita
el seguimiento de su ciclo de vida durante la ejecución del juego.
@note El sprite es eliminado por la clase base obstáculo; aquí solo se gestiona el
  temporizador específico del carro.
*/
Carro::~Carro() {
    //qDebug() << "Destructor de carro llamado";
    contCarro-=1;
    //qDebug()<<"libero carro desstructor "<<contCarro;
    timerEspiral->stop();
    delete timerEspiral; // Eliminar el temporizador creado con new
}

/**
@brief Posiciona inicialmente el carro en la escena.
Establece las coordenadas (x, y) absolutas donde aparecerá el sprite del carro.
Este método se invoca justo después de crear el objeto para ubicarlo en el punto
del escenario donde debe estar visible antes de que comience cualquier animación
de movimiento o interacción con el jugador.
@param x Coordenada horizontal deseada dentro de la escena.
@param y Coordenada vertical deseada dentro de la escena.
*/
void Carro::iniciar(int x, int y)
{
    sprite->setPos(x, y);
}

/**
@brief Inicia la animación de rotación del carro.
Activa el temporizador responsable de alternar los cuadros del sprite para simular
que el carro está girando sobre sí mismo. Solo procede si el carro no se encuentra
ya en estado de rotación, evitando así reinicios innecesarios o superposiciones de
temporizadores. Este efecto visual se utiliza cuando el carro es pateado por Goku
o durante secuencias específicas del nivel.
@note La animación se ejecuta cada 150 ms hasta que se detenga externamente.
*/
void Carro::rotar()
{
    // Si el carro no está girando, activa el temporizador para comenzar la animación
    if (!girando) {
        girando = true;                // Marca que ya está girando
        timerRotacion->start(150);    // Cambia el cuadro de animación cada 150 milisegundos
    }
}

/**
@brief Indica si el carro está actualmente en estado de rotación.
Devuelve el valor del flag girando, que se activa al llamar a rotar() y permite
saber si la animación de giro está en curso. Útil para evitar reinicios del efecto
o para sincronizar otras acciones que dependan de que el carro esté o no girando.
@return true si el carro está girando; false en caso contrario.
*/
bool Carro::estaGirando() const {
    return girando;
}

/**
@brief Inicia la trayectoria parabólica-en-espiral del carro tras ser pateado por Goku.
Configura y pone en marcha la animación que simula el vuelo del carro por los aires:
primero una subida parabólica, luego un giro circular y, finalmente, una caída libre.
Solo se ejecuta si el carro no ha realizado ya la espiral y se encuentra en estado
de reposo (fase 3). Se registra la posición horizontal exacta en la que Goku lo pateó
para calcular un punto de inicio desplazado 1000 píxeles hacia la derecha, evitando
que el vehículo pase por encima del personaje. El temporizador timerEspiral
generará actualizaciones cada 20 ms para lograr un movimiento fluido.
@param _posXpatada Posición en X del personaje Goku en el momento de la patada.
*/
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

/**
@brief Actualiza la posición y rotación del carro durante su trayectoria espiral.
Se ejecuta periódicamente (cada 20 ms) mientras el temporizador timerEspiral está activo.
Divide la animación en tres fases consecutivas:
Subida parabólica: el carro asciende describiendo una parábola controlada por gravedad g
y velocidades iniciales vx, vy. La coordenada horizontal se desplaza 127 píxeles para evitar
que el vehículo pase por encima de Goku.
Giro circular: activa la animación de rotación mientras el carro describe un arco circular
de radio radio y duración tiempoGiro.
Caída libre: finalmente el carro cae verticalmente hasta alcanzar la altura ySuelo.
Al finalizar la tercera fase se detiene el temporizador y se marca la animación como concluida
(fase 3), notificando al resto del juego que el carro ha tocado el suelo y que pueden proceder
eventos posteriores (aparición de robots, cambio de estado, etc.).
@note El método se apoya en animarRotacion() durante la fase circular para mostrar los frames
  del sprite correspondientes al giro del vehículo.
*/
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

/**
@brief Actualiza el frame del sprite para simular la rotación del carro.
Avanza cíclicamente entre los tres primeros frames del sprite-sheet horizontal
cada vez que es invocada, generando la ilusión de giro continuo del vehículo
durante la fase circular de su trayectoria. El índice se reinicia automáticamente
al alcanzar el último frame, asegurando una animación fluida y sin interrupciones.
*/
void Carro::animarRotacion()
{
    // frame ya existente:
    cuadroActual = (cuadroActual + 1) % 3;
    QPixmap cuadro = imagenCarro.copy(cuadroActual * anchoCuadro, 0,
                                      anchoCuadro, altoCuadro);
    sprite->setPixmap(cuadro);
}

/**
@brief Indica si el carro ha finalizado su trayectoria y tocado el suelo.
@return true si el carro se encuentra en la fase final (3) y su posición vertical
    es igual o superior a 500 píxeles; `false` en cualquier otro caso.
@note Este método es utilizado por Nivel1 para detectar el momento exacto en que
  el carro aterriza y proceder con la aparición de los robots enemigos.
*/
bool Carro::haLlegadoAlSuelo() const
{
    return fase == 3 && sprite->y() >= 500; //ha llegado al suelo
}
