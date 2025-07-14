#include "nivel2.h"
#include "pocion.h"
#include "goku2.h"
#include "robot.h"
#include <QMessageBox>
#include <QRandomGenerator>
#include <QTimer>
#include <QDebug>
#include <stdexcept>
#include <QPointer>

// Inicialización del contador
int Nivel2::contador = 0;

/**
@brief Constructor del Nivel 2 del juego.

Inicializa los componentes y banderas internas específicas del segundo nivel, utilizando el constructor de la clase base `Nivel`.

- Inicializa punteros clave como `robot`, `barraProgreso` y `temporizadorPociones` en `nullptr`.
- Establece las banderas de control en `false` (`robotInicialCreado`, `pocionesAgregadas`, `perdioGoku`).
- No realiza acciones adicionales dentro del cuerpo del constructor para evitar llamadas inseguras a métodos virtuales.

@param escena Puntero a la escena del juego.
@param vista Puntero a la vista asociada.
@param parent Widget padre opcional.
*/
Nivel2::Nivel2(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent)
    : Nivel(escena, vista, parent, 2),
    robot(nullptr),
    barraProgreso(nullptr),
    temporizadorPociones(nullptr),
    robotInicialCreado(false),
    pocionesAgregadas(false),
    perdioGoku(false)
{
    // Constructor vacío (inicialización en lista de inicialización)
}

/**
@brief Destructor de la clase `Nivel2`, responsable de liberar los recursos específicos del segundo nivel.

Este método complementa al destructor de la clase base `Nivel` realizando la limpieza de los siguientes elementos:

1. Detiene, desconecta y elimina el `temporizadorPociones` que controla la aparición periódica de pociones.
2. Elimina todas las instancias de `Pocion` agregadas a la escena y limpia la lista `listaPociones`.
3. Elimina el objeto `robot`, desconectando previamente todas sus señales y removiendo su sprite de la escena.

@note Los elementos comunes como `goku`, `barraVida` y `barraProgreso` se eliminan en el destructor de `Nivel`.

@see Nivel::~Nivel
*/
Nivel2::~Nivel2()
{
    qDebug() << "Destructor de Nivel2 llamado";

    // 1. Detener y desconectar timers primero
    if (temporizadorPociones) {
        temporizadorPociones->stop();
        disconnect(temporizadorPociones, nullptr, this, nullptr);
        delete temporizadorPociones;
        temporizadorPociones = nullptr;
    }
    //qDebug() << "Destructor nivel 2 destruyo el temporizador pociones correctamente";

    // 2. Limpieza de pociones
    for (auto* pocion : listaPociones) {
        if (pocion && escena) {
            //qDebug() << "entra en pocion y escena ";
            escena->removeItem(pocion);
            delete pocion;
        }
    }
    listaPociones.clear();

    //qDebug() << "Destructor nivel 2 destruyo pociones correctamente";

    // 3. Limpieza del robot (con desconexión de señales)
    if (robot) {
        disconnect(robot, nullptr, this, nullptr); // Desconecta todas sus señales
        if (escena && robot->getSprite()) {
            escena->removeItem(robot->getSprite());
        }
        delete robot;
        robot = nullptr;
    }

    // NOTA: goku, barraVida y barraProgreso son liberados por la clase base Nivel
}

/**
@brief Inicia el Nivel 2, configurando el fondo, HUD, personajes y elementos interactivos.

Este método se encarga de preparar el entorno gráfico y lógico del segundo nivel, realizando las siguientes acciones:

- Carga el fondo específico del nivel (`background2.png`) y genera nubes decorativas.
- Crea e inserta la barra de vida y una barra de progreso basada en la recolección de pociones.
- Inicializa un temporizador (`temporizadorPociones`) que genera nuevas pociones cada 2.5 segundos.
- Agrega a Goku, el robot enemigo y un conjunto inicial de pociones interactivas.

@see Nivel2::agregarPocionAleatoria
@see Nivel2::agregarGoku
@see Nivel2::agregarRobot
*/
void Nivel2::iniciarNivel()
{
    cargarFondoNivel(":/images/background2.png");
    generarNubes();

    // Configuración del HUD (la liberación lo hace la clase base)
    barraVida = new Vida(vista);
    barraVida->move(20, 20);
    barraVida->show();

    barraProgreso = new Progreso(Pociones, ":/images/icono_pocion.png", vista);
    barraProgreso->move(20, 60);
    barraProgreso->setTotalPociones(totalPociones);
    barraProgreso->show();

    // Temporizador de pociones (con parent QObject para auto-liberación)
    temporizadorPociones = new QTimer(this);
    connect(temporizadorPociones, &QTimer::timeout, this, &Nivel2::agregarPocionAleatoria);
    temporizadorPociones->start(2500);

    // Elementos del juego
    agregarPociones();
    agregarGoku();
    agregarRobot();
}

/**
@brief Carga y establece el fondo gráfico del Nivel 2, reemplazando cualquier fondo anterior.

Este método realiza las siguientes acciones:

- Intenta cargar la imagen especificada por `ruta`.
- Si la imagen no se puede cargar, lanza una excepción.
- Elimina los fondos anteriores contenidos en `listaFondos`.
- Crea un nuevo `QGraphicsPixmapItem` con la imagen y lo posiciona en `(0, 0)` dentro de la escena.

@param ruta Ruta al archivo de imagen del fondo (por ejemplo, `:/images/background2.png`).

@throw std::runtime_error Si no se puede cargar el fondo especificado.
*/
void Nivel2::cargarFondoNivel(const QString &ruta)
{
    QPixmap fondo(ruta);
    if (fondo.isNull()) {
        throw std::runtime_error("Nivel2: no se pudo cargar el fondo del nivel.");
    }

    // Limpiar fondos existentes primero
    for (auto* item : listaFondos) {
        escena->removeItem(item);
        delete item;
    }
    listaFondos.clear();

    // Crear nuevo fondo
    QGraphicsPixmapItem* fondoItem = new QGraphicsPixmapItem(fondo);
    fondoItem->setPos(0, 0);
    escena->addItem(fondoItem);
    listaFondos.push_back(fondoItem);
}

/**
@brief Agrega a Goku2 al Nivel 2, configurando su imagen, posición inicial y parámetros físicos.

Este método crea una instancia del personaje `Goku2`, la asocia con la barra de vida y la configura para que interactúe
correctamente con la física del nivel:

- Define las dimensiones del sprite (218x298 píxeles) y la velocidad de movimiento.
- Carga su imagen inicial con `cargarImagen()`.
- Establece el nivel del suelo (`setSueloY`) basado en la altura de la vista.
- Posiciona a Goku2 en la escena en la coordenada `(100, ySuelo)`.

@see Goku2::setSueloY
@see Goku2::iniciar
*/
void Nivel2::agregarGoku()
{
    int anchoFrame = 218, altoFrame = 298, velocidad = 12;

    goku = new Goku2(escena, velocidad, anchoFrame, altoFrame, this);
    Goku2* goku2 = static_cast<Goku2*>(goku); // Conversión segura
    goku2->cargarImagen();
    goku2->setBarraVida(barraVida);

    int ySuelo = vista->height() - altoFrame - 30;
    goku2->setSueloY(ySuelo);
    goku2->iniciar(100, ySuelo); // Posición inicial
}

/**
@brief Carga los frames de animación de las pociones y agrega una poción inicial a la escena.

Este método realiza lo siguiente:

- Carga la hoja de sprites `:/images/pocion.png`.
- Extrae 6 frames individuales de tamaño 65x64 píxeles y los almacena en `framesPocion`.
- Crea una instancia de `Pocion` utilizando los frames y la posiciona en la escena.
- La poción se agrega también a la lista `listaPociones` para su posterior gestión.

@throw std::runtime_error Si no se puede cargar la imagen de la poción.

@see Pocion
*/
void Nivel2::agregarPociones()
{
    QPixmap hojaSprites(":/images/pocion.png");
    if (hojaSprites.isNull()) {
        throw std::runtime_error("Nivel2: imagen de poción no encontrada.");
    }

    framesPocion.clear(); // Limpiar frames existentes

    int anchoSprite = 65, altoSprite = 64;
    for (int i = 0; i < 6; ++i) {
        framesPocion.push_back(hojaSprites.copy(i * anchoSprite, 0, anchoSprite, altoSprite));
    }

    // Primera poción
    Pocion* pocion = new Pocion(framesPocion, 0, 0, 1);
    escena->addItem(pocion);
    listaPociones.push_back(pocion);
}

/**
@brief Agrega una nueva poción en una posición aleatoria dentro de una grilla lógica en la escena del Nivel 2.

Este método es llamado periódicamente por un temporizador (`temporizadorPociones`) para generar nuevas pociones durante el nivel.

- Si la barra de progreso indica que se han recolectado todas las pociones (`getPorcentaje() >= 1.0f`), se detiene el temporizador.
- Selecciona aleatoriamente una fila (0 a 1) y una columna (0 a 6) para colocar la poción.
- Crea una nueva instancia de `Pocion` con los frames previamente cargados y la inserta en la escena y en `listaPociones`.

@see Nivel2::agregarPociones
@see Progreso::getPorcentaje
*/
void Nivel2::agregarPocionAleatoria()
{

    //qDebug() << "timer pociones nivel2 llamado  "<<contador++;
    if (!barraProgreso || barraProgreso->getPorcentaje() >= 1.0f) {
        temporizadorPociones->stop();
        return;
    }

    int fila = QRandomGenerator::global()->bounded(0, 2);
    int columna = QRandomGenerator::global()->bounded(0, 7);
    Pocion* nuevaPocion = new Pocion(framesPocion, fila, columna, 7);
    escena->addItem(nuevaPocion);
    listaPociones.push_back(nuevaPocion);
}

/**
@brief Informa a la barra de progreso que se ha recolectado una poción.

Este método debe ser llamado cada vez que Goku2 recolecta una poción en el Nivel 2.
Incrementa internamente el contador de pociones recolectadas en la barra de progreso.

@see Progreso::sumarPocion
@see Goku2::detectarPocion
*/
void Nivel2::pocionRecolectada()
{
    if (barraProgreso) {
        barraProgreso->sumarPocion();
    }
}

/**
@brief Lógica principal de actualización del Nivel 2, ejecutada periódicamente por un temporizador.

Este método controla la progresión del nivel verificando condiciones de derrota o victoria:

- **Derrota:** Si la vida de Goku llega a 0, se marca el estado como perdido (`perdioGoku`) y se llama a `gameOver()`.
- **Victoria:** Si la barra de progreso llega al 100%, se detienen los ataques del robot, los temporizadores relevantes,
  y tras 1 segundo se inicia la animación de ataque Kamehameha de Goku2. Si el robot muere, se emite la señal `nivelCompletado`.

@note Usa `QTimer::singleShot` para introducir una pausa antes de lanzar el ataque final.

@see Nivel2::gameOver
@see Goku2::iniciarKamehameha
@see Progreso::getPorcentaje
*/
void Nivel2::actualizarNivel()
{
    if (!goku) return;

    // Verificar derrota
    if (barraVida && barraVida->obtenerVida() <= 0) {
        perdioGoku = true;
        gameOver();
        return;
    }

    // Verificar victoria (con QPointer para seguridad)
    if (barraProgreso && barraProgreso->getPorcentaje() >= 1.0f) {
        temporizadorPociones->stop();
        if (timerNivel) timerNivel->stop();
        if (robot) robot->detenerAtaques();

        QTimer::singleShot(1000, this, [this]() {
            //qDebug() << "timer antes iniciar kameja en nivel2 llamado  "<<contador++;
            Goku2* goku2 = static_cast<Goku2*>(goku);
            if (goku2 && robot) {
                connect(robot, &Robot::robotMurio, this, &Nivel2::nivelCompletado);
                goku2->iniciarKamehameha(robot->getSprite()->x(), robot);
            }
        });
    }
}

/**
@brief Agrega el robot enemigo al Nivel 2 y lo posiciona escalado en la esquina inferior derecha de la escena.

Este método:

- Crea una instancia de `Robot` y la escala 3 veces su tamaño original.
- Calcula su posición para alinearlo en la parte inferior derecha de la escena (`1530x784`).
- Inicia sus ataques automáticamente tras colocarlo en la escena.

@see Robot::iniciarAtaques
*/
void Nivel2::agregarRobot()
{
    const float escala = 3.0;
    robot = new Robot(escena, this);
    robot->getSprite()->setScale(escala);

    QPixmap frame0 = robot->getSprite()->pixmap();
    int x = 1530 - (frame0.width() * escala) - 30;
    int y = 784 - (frame0.height() * escala) - 10;
    robot->getSprite()->setPos(x, y);
    robot->iniciarAtaques();
}

/**
@brief Maneja el evento de "Game Over" en el Nivel 2 cuando Goku pierde toda su vida.

Este método se encarga de:

- Detener el temporizador principal del nivel (`timerNivel`) y el temporizador de aparición de pociones.
- Mostrar visualmente la pantalla de derrota llamando a `mostrarGameOver()`.
- Emitir la señal `gokuMurio()` para notificar al sistema que el jugador ha perdido.

@see Nivel2::actualizarNivel
@see Nivel::mostrarGameOver
*/
void Nivel2::gameOver() {
    if (timerNivel) timerNivel->stop();
    if (temporizadorPociones) temporizadorPociones->stop();
    mostrarGameOver();
    emit gokuMurio();
}

/**
@brief Indica si el Nivel 2 ha sido completado exitosamente.

Este método verifica si la barra de progreso ha alcanzado o superado el 100% de pociones recolectadas.

@return `true` si el jugador completó el nivel, `false` en caso contrario.
*/
bool Nivel2::haTerminado() const
{
    return barraProgreso && barraProgreso->getPorcentaje() >= 1.0f;
}

/**
@brief Devuelve el puntero al personaje Goku utilizado en el Nivel 2.

Permite acceder al objeto principal para control o consulta desde otras partes del programa.

@return Puntero a `Goku` si está inicializado; `nullptr` en caso contrario.
*/
Goku* Nivel2::getGoku() const
{
    return goku;
}
