#include "nivel1.h"
#include "goku1.h"
#include "robot.h"
#include "obstaculo.h"
#include <QRandomGenerator>
#include <QMessageBox>

// Inicialización del contador
int Nivel1::contador = 0;

/**
@brief Constructor del nivel 1 del juego.

Inicializa el estado lógico del primer nivel (`Nivel1`), heredando configuración común desde la clase base `Nivel`.

- Establece los flags `nivelTerminado` y `perdioGoku` en `false`.
- No realiza más acciones aquí para evitar invocar métodos virtuales durante la construcción, lo cual podría ser inseguro.

@param escena Puntero a la escena donde se desarrollará el nivel.
@param vista Puntero a la vista gráfica (`QGraphicsView`) que contiene la escena.
@param parent Widget padre opcional.
*/
Nivel1::Nivel1(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent)
    : Nivel(escena, vista, parent, 1)
{
    nivelTerminado = false;
    perdioGoku = false;
    // No se hace nada más aquí para evitar errores con funciones virtuales
}

/**
@brief Destructor de la clase `Nivel1`, responsable de liberar recursos específicos del primer nivel.

Este método complementa al destructor de la clase base `Nivel` y realiza las siguientes tareas adicionales:

- Detiene y elimina la cámara (`camara`) si fue creada.
- Detiene y desconecta el temporizador de nivel (`timerNivel`) si sigue activo.
- Elimina el carro final, los tres robots (`r1`, `r2`, `r3`) y todos los obstáculos agregados a la escena mediante `getSprite()`.

@note Este destructor garantiza que todos los elementos visuales y lógicos específicos del Nivel 1 se liberen correctamente.

@see Nivel::~Nivel
*/
Nivel1::~Nivel1()
{
    //qDebug() << "Destructor de Nivel1 llamado";

    // Limpiar la cámara
    if (camara) {
        camara->detenerMovimiento();
        delete camara;
    }

    if (timerNivel) {
        timerNivel->stop();
        disconnect(timerNivel, nullptr, this, nullptr);
    }

    // Eliminar carro (usando getSprite())
    if (carroFinal) {
        escena->removeItem(carroFinal->getSprite());
        delete carroFinal;
    }

    // Eliminar robots (usando getSprite())
    if (r1) { escena->removeItem(r1->getSprite()); delete r1; }
    if (r2) { escena->removeItem(r2->getSprite()); delete r2; }
    if (r3) { escena->removeItem(r3->getSprite()); delete r3; }

    // Eliminar obstáculos (usando getSprite())
    for (auto* obs : listaObstaculos) {
        if (obs) {
            escena->removeItem(obs->getSprite());
            delete obs;
        }
    }
    listaObstaculos.clear();
}

/**
@brief Inicia y configura todos los elementos visuales y lógicos del Nivel 1.

Este método prepara el entorno de juego para el primer nivel realizando los siguientes pasos:

- Carga el fondo del nivel (`cargarFondoNivel()`).
- Genera nubes en movimiento y agrega al personaje principal (`Goku`), el carro final y los obstáculos.
- Crea una cámara lógica (`camaraLogica`) que sigue al personaje mientras avanza.
- Inicializa y activa un temporizador (`timerNivel`) que ejecuta periódicamente `actualizarNivel()` para controlar la lógica del nivel.

@see Nivel1::actualizarNivel
@see Nivel1::agregarGoku
@see Nivel1::agregarObstaculos
@see camaraLogica
*/
void Nivel1::iniciarNivel()
{
    cargarFondoNivel(":/images/background1.png");

    generarNubes();
    agregarGoku();
    agregarCarroFinal();
    agregarObstaculos();

    camara = new camaraLogica(vista, this);
    camara->seguirAGoku(goku);
    camara->iniciarMovimiento();

    // 4) Arrancamos el timer del nivel como antes
    timerNivel = new QTimer(this);
    connect(timerNivel, &QTimer::timeout, this, &Nivel1::actualizarNivel);
    timerNivel->start(20);
}

/**
@brief Carga y posiciona múltiples copias del fondo para crear un escenario horizontal extendido.

Este método utiliza la imagen especificada en `ruta` para crear un fondo repetido que cubre un área amplia del nivel.

- Inserta 5 instancias del fondo (`cantidad = 5`) una al lado de la otra.
- Cada fondo se coloca con `setZValue(-10)` para asegurar que permanezca detrás de los elementos del juego.
- Los objetos creados se almacenan en `listaFondos` para su posterior gestión y limpieza.

@param ruta Ruta al recurso gráfico del fondo (por ejemplo, `:/images/background1.png`).
*/
void Nivel1::cargarFondoNivel(const QString &ruta)
{
    QPixmap fondo(ruta);
    int ancho = fondo.width();
    int cantidad = 5;

    for (int i = 0; i < cantidad; ++i) {
        QGraphicsPixmapItem* fondoItem = escena->addPixmap(fondo);
        fondoItem->setZValue(-10);
        fondoItem->setPos(i * ancho, 0);
        listaFondos.push_back(fondoItem);
    }
}

/**
@brief Agrega al personaje Goku y las barras de vida y progreso al Nivel 1.

Este método crea e inicializa los componentes principales del jugador para el Nivel 1:

- Coloca la barra de vida en la parte superior izquierda de la vista.
- Agrega una barra de progreso que representa el avance hacia el carro final.
- Crea una instancia de `Goku1`, carga su imagen, asocia la barra de vida y lo posiciona en la escena.
- Inicia un temporizador que actualiza visualmente la barra de progreso a medida que Goku avanza.

@note La barra de progreso usa el ícono del carro (`:/images/icono_carro.png`) y se actualiza cada 50 ms.

@see Goku1::iniciar
@see Progreso::actualizarProgreso
*/
void Nivel1::agregarGoku()
{
    int posX = 100;
    int posY = 400;
    int velocidad = 6;

    // Barra de vida en la esquina superior izquierda
    barraVida = new Vida(vista);
    barraVida->move(20, 20);
    barraVida->show();

    // Nueva barra de progreso usando el ícono del carro y tipo Horizontal
    barraProgreso = new Progreso(Horizontal, ":/images/icono_carro.png", vista);
    barraProgreso->move(20, 60);
    barraProgreso->show();

    // Goku nivel 1
    goku = new Goku1(escena, velocidad, 200, 249, this);
    static_cast<Goku1*>(goku)->cargarImagen();
    goku->setBarraVida(barraVida);
    goku->iniciar(posX, posY);

    // Timer para actualizar el progreso de avance
    QTimer* timerProgreso = new QTimer(this);
    connect(timerProgreso, &QTimer::timeout, this, [=]() {
        //qDebug() << "timer progreso en nivel1 llamado  "<<contador++;
        if (goku && carroFinal && barraProgreso) {
            float inicio = 0;
            float fin = carroFinal->getSprite()->x();
            barraProgreso->actualizarProgreso(goku->x(), inicio, fin);
        }
    });
    timerProgreso->start(50);
}

/**
@brief Agrega el carro final al Nivel 1 en una posición fija de la escena.

Este método crea una instancia del objeto `Carro`, lo posiciona en las coordenadas `(4900, 550)` dentro de la escena
y lo inicializa llamando a su método `iniciar()`.

- El carro es el objetivo final del personaje en este nivel.
- La posición fija corresponde al final del recorrido de Goku.

@see Carro::iniciar
*/
void Nivel1::agregarCarroFinal()
{
    carroFinal = new Carro(escena, 0, this);
    carroFinal->iniciar(4900, 550);
}

/**
@brief Agrega múltiples obstáculos al Nivel 1 de forma aleatoria y escalonada.

Este método genera 18 obstáculos a lo largo del recorrido del personaje, con posiciones iniciales a partir de `x = 1500`.
Cada obstáculo puede ser de uno de los siguientes tipos seleccionados aleatoriamente:

- `Ave`: se posiciona en una altura aleatoria entre 100 y 150.
- `Montania`: se coloca en la base de la escena y se escala en altura.
- `Roca`: se ubica entre 350 y 550 píxeles de altura.

Cada obstáculo es inicializado con una velocidad fija de desplazamiento y espaciado horizontal variable para dar variedad al nivel.

@see obstaculo::iniciar
@see obstaculo::cargarImagenes
*/
void Nivel1::agregarObstaculos()
{
    int x = 1500;
    int velocidad = 10;

    for (int i = 0; i < 18; ++i) {
        int tipo = QRandomGenerator::global()->bounded(0, 3);
        obstaculo* obs = nullptr;

        switch (tipo) {
        case 0:
            obs = new obstaculo(escena, obstaculo::Ave, velocidad, this);
            obs->iniciar(x, QRandomGenerator::global()->bounded(100, 150));
            x += 500;
            break;
        case 1:
            obs = new obstaculo(escena, obstaculo::Montania, velocidad, this);
            obs->cargarImagenes();
            obs->iniciar(x, escena->height() - obs->getAltura());
            x += 600;
            break;
        case 2:
            obs = new obstaculo(escena, obstaculo::Roca, velocidad, this);
            obs->iniciar(x, QRandomGenerator::global()->bounded(350, 550));
            x += 700;
            break;
        }

        listaObstaculos.push_back(obs);
    }
}

/**
@brief Lógica principal de actualización del Nivel 1, ejecutada periódicamente por un temporizador.

Este método gestiona el avance del nivel verificando el estado del personaje, las colisiones y la progresión de los eventos:

- Si Goku pierde toda la vida, se marca el nivel como perdido (`perdioGoku`) y se llama a `gameOver()`.
- Si Goku alcanza el carro y no ha ejecutado aún la patada, se reproduce la animación de ataque y se inicia el movimiento en espiral del carro.
- Una vez que el carro termina su caída (espiral completada y llega al suelo), se agregan los robots enemigos y se elimina el carro.
- Después de 5 segundos, se emite la señal `nivelCompletado()` para continuar el flujo del juego.

@see Goku1::haTocadoCarro
@see Carro::iniciarMovimientoEspiral
@see Nivel1::agregarRobots
@see Nivel1::quitarCarroVista
*/
void Nivel1::actualizarNivel()
{
    //qDebug() << "timeractualizar nivel en nivel1 llamado  "<<contador++;
    if (!goku || !carroFinal) return;

    // Si se acabó la vida de Goku
    if (goku->obtenerVida() <= 0) {
        perdioGoku = true;
        gameOver();
    }

    // Detectar si Goku llegó al carro
    Goku1* goku1 = dynamic_cast<Goku1*>(goku);
    if (goku1 && goku1->haTocadoCarro() && !gokuYaPateo) {
        gokuYaPateo = true;
        goku1->patadaGokuNivel1();

        float posXpatada = goku1->x();
        //qDebug() << "posición de Goku en x = " << posXpatada;
        carroFinal->iniciarMovimientoEspiral(posXpatada);
    }

    if (carroFinal->espiralHecha && carroFinal->haLlegadoAlSuelo()) {
        agregarRobots();
        quitarCarroVista();

        // Espera 5 segundos antes de emitir la señal
        QTimer::singleShot(5000, this, [this]() {

            //qDebug() << "timer single shot mostrar nivelcompletado en nivel 1 llamado  " <<contador++;
            emit nivelCompletado();
        });
    }
}

/**
@brief Agrega y despliega tres robots enemigos en el Nivel 1 después de que el carro cae al suelo.

Este método crea tres instancias de `Robot` con velocidades y posiciones específicas,
y las introduce de forma escalonada usando temporizadores para crear una secuencia de aparición dinámica.

- Los robots se despliegan con un retardo de 0 ms, 600 ms y 1200 ms respectivamente.
- Después de 1800 ms, se detiene el movimiento de los tres robots con `detenerMvtoRobot()`.
- Solo se ejecuta una vez por nivel gracias a la bandera `robotsCreados`.

@note Este método se llama desde `actualizarNivel()` tras detectar que el carro final ha llegado al suelo.

@see Robot::iniciar
@see Robot::desplegarRobot
@see Robot::detenerMvtoRobot
*/
void Nivel1::agregarRobots()
{
    if (robotsCreados || !carroFinal || !carroFinal->haLlegadoAlSuelo())
        return;

    robotsCreados = true;
    int ySuelo = 500;
    int velocidad = 6;

    r1 = new Robot(escena, velocidad, 1, this);
    r2 = new Robot(escena, velocidad, 2, this);
    r3 = new Robot(escena, velocidad, 3, this);

    r1->iniciar(5000, ySuelo, 5300);
    r1->desplegarRobot();

    QTimer::singleShot(600, this, [=]() {

        //qDebug() << "timer singles  robots 1 nivel1 llamado  "<<contador++;
        r2->iniciar(5300, ySuelo, 5600);
        r2->desplegarRobot();
    });

    QTimer::singleShot(1200, this, [=]() {

        //qDebug() << "timer singles  robots 2 nivel1 llamado  "<<contador++;
        r3->iniciar(5600, ySuelo, 5900);
        r3->desplegarRobot();
    });

    QTimer::singleShot(1800, this, [=]() {

        //qDebug() << "timer singles  robots 3 nivel1 llamado  "<<contador++;
        r1->detenerMvtoRobot();
        r2->detenerMvtoRobot();
        r3->detenerMvtoRobot();
    });
}

/**
@brief Elimina el carro final de la escena una vez que ha cumplido su función.

Este método remueve el sprite del objeto `carroFinal` de la escena y libera su memoria.
Se utiliza después de que el carro ha realizado su movimiento en espiral y ha llegado al suelo.

@note Este método se llama típicamente tras `carroFinal->haLlegadoAlSuelo()` en el flujo de `actualizarNivel()`.

@see Nivel1::actualizarNivel
*/
void Nivel1::quitarCarroVista()
{
    if (carroFinal && escena) {
        escena->removeItem(carroFinal->getSprite());
        delete carroFinal;
        carroFinal = nullptr;
    }
}

/**
@brief Maneja el evento de "Game Over" en el Nivel 1 cuando Goku pierde toda su vida.

Este método asegura que el proceso de derrota solo se ejecute una vez mediante la bandera `gameOverProcesado`.
Realiza las siguientes acciones:

- Detiene y desconecta el temporizador principal del nivel (`timerNivel`).
- Llama a `mostrarGameOver()` para mostrar visualmente la pantalla de derrota.
- Emite la señal `gokuMurio()` para notificar al juego que Goku ha sido derrotado.

@note Esta función es invocada desde `actualizarNivel()` cuando la vida de Goku llega a cero.

@see Nivel1::mostrarGameOver
@see Nivel1::actualizarNivel
*/
void Nivel1::gameOver()
{
    if (gameOverProcesado) return;//si ya se llamo (v), no se llame mas

    //pasamos a estado verdadero si ya se llamo
    gameOverProcesado = true;

    if (timerNivel) {
        timerNivel->stop();
        disconnect(timerNivel, nullptr, this, nullptr);  //desconexión explícita
    }
    mostrarGameOver();
    emit gokuMurio();
}

/**
@brief Indica si Goku fue derrotado en el Nivel 1.

@return `true` si Goku perdió toda su vida y se activó el estado de "Game Over", `false` en caso contrario.
*/
bool Nivel1::getPerdioGoku() const{
    return perdioGoku;
}

/**
@brief Verifica si el Nivel 1 ha finalizado exitosamente.

@return `true` si se completó el nivel y se activó la condición de victoria, `false` en caso contrario.
*/
bool Nivel1::haTerminado() const
{
    return nivelTerminado;
}

/**
@brief Devuelve el puntero al objeto Goku utilizado en el Nivel 1.

Este método permite acceder al personaje principal (`Goku`) desde otras clases, como el sistema de cámara o la lógica del juego.

@return Puntero a `Goku` si ha sido creado, o `nullptr` si aún no ha sido inicializado.
*/
Goku* Nivel1::getGoku() const
{
    return goku;
}
