#include "nivel2.h"
#include "pocion.h"
#include "goku2.h"
#include "robot.h"
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <QRandomGenerator>
#include <QTimer>
#include <QDebug>
#include <stdexcept>

// Constructor del nivel 2
Nivel2::Nivel2(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent)
    : Nivel(escena, vista, parent, 2),
    robotInicialCreado(false),
    pocionesAgregadas(false),
    barraProgreso(nullptr),
    temporizadorPociones(nullptr) {}

// Destructor: libera memoria de pociones, explosiones y temporizador
Nivel2::~Nivel2()
{
    qDebug() << "Destructor de Nivel2 llamado";

    for (auto* pocion : listaPociones)
        delete pocion; // Libera cada poción creada con new
    listaPociones.clear();

    if (temporizadorPociones) {
        temporizadorPociones->stop();
        delete temporizadorPociones;
        temporizadorPociones = nullptr;
    }

    for (auto* exp : listaExplosiones)
        delete exp; // Libera cada explosión
    listaExplosiones.clear();
}

// Inicializa el nivel: carga fondo, nubes, HUD (vida y progreso) y personajes
void Nivel2::iniciarNivel() {
    cargarFondoNivel(":/images/background2.png");
    generarNubes(); // Método heredado

    // Crea barra de vida
    barraVida = new Vida(vista);
    barraVida->move(20, 20);
    barraVida->show();

    // Crea barra de progreso para pociones
    barraProgreso = new Progreso(Pociones, ":/images/icono_pocion.png", vista);
    barraProgreso->move(20, 60);
    barraProgreso->setTotalPociones(totalPociones);
    barraProgreso->show();

    agregarPociones(); // Añade la primera poción

    // Configura temporizador para generar más pociones aleatorias
    temporizadorPociones = new QTimer(this);
    connect(temporizadorPociones, &QTimer::timeout, this, &Nivel2::agregarPocionAleatoria);
    temporizadorPociones->start(2500); // cada 2.5 segundos

    agregarGoku();
    agregarRobot();
}

// Carga el fondo del nivel y lo agrega a la escena
void Nivel2::cargarFondoNivel(const QString &ruta) {
    background = QPixmap(ruta);
    if (background.isNull()) {
        throw std::runtime_error("Nivel2: no se pudo cargar el fondo del nivel.");
    }
    QGraphicsPixmapItem* _background = new QGraphicsPixmapItem(background);
    _background->setPos(0, 0);
    escena->addItem(_background);
    listaFondos.push_back(_background); // Se almacena para futura eliminación
}

// Crea a Goku con sus parámetros específicos
void Nivel2::agregarGoku() {
    int anchoFrame = 218, altoFrame = 298, velocidad = 12;

    goku = new Goku2(escena, velocidad, anchoFrame, altoFrame, this);
    static_cast<Goku2*>(goku)->cargarImagen(); // Carga sprite
    goku->setBarraVida(barraVida); // Conecta barra de vida

    int xInicial = 100;
    int yInicial = vista->height() - altoFrame - 30;

    Goku2* goku2 = dynamic_cast<Goku2*>(goku);
    if (!goku2) throw std::runtime_error("Nivel2: Goku no es instancia válida de Goku2.");

    goku2->setSueloY(yInicial);
    goku->iniciar(xInicial, yInicial);
}

// Carga la hoja de sprites y crea una poción inicial
void Nivel2::agregarPociones() {
    QPixmap hojaSprites(":/images/pocion.png");
    if (hojaSprites.isNull()) {
        throw std::runtime_error("Nivel2: imagen de poción no encontrada.");
    }

    int anchoSprite = 65, altoSprite = 64;

    for (int i = 0; i < 6; ++i) {
        QPixmap frame = hojaSprites.copy(i * anchoSprite, 0, anchoSprite, altoSprite); // Extrae cada frame
        framesPocion.push_back(frame);
    }

    Pocion* pocion = new Pocion(framesPocion, 0, 0, 1);
    escena->addItem(pocion);
    listaPociones.push_back(pocion);
}

// Agrega una poción en una posición aleatoria si aún no se ha completado el nivel
void Nivel2::agregarPocionAleatoria() {
    if (!barraProgreso || barraProgreso->getPorcentaje() >= 1.0f) {
        if (temporizadorPociones) {
            temporizadorPociones->stop();
            temporizadorPociones->deleteLater();
            temporizadorPociones = nullptr;
        }
        return;
    }

    int fila = QRandomGenerator::global()->bounded(0, 2); // Fila aleatoria
    int columna = QRandomGenerator::global()->bounded(0, 7); // Columna aleatoria
    Pocion* nuevaPocion = new Pocion(framesPocion, fila, columna, 7);
    escena->addItem(nuevaPocion);
    listaPociones.push_back(nuevaPocion);
}

// Suma una poción a la barra de progreso
void Nivel2::pocionRecolectada() {
    if (barraProgreso)
        barraProgreso->sumarPocion();
}

// Lógica principal que se ejecuta periódicamente durante el nivel
// Evalúa si Goku ha perdido o si el jugador ha ganado recolectando todas las pociones
void Nivel2::actualizarNivel() {
    if (!goku) return;

    // Si Goku se queda sin vida
    if (barraVida && barraVida->obtenerVida() <= 0) {
        perdioGoku = true;
        gameOver();
        return;
    }

    // Si se recolectaron todas las pociones
    if (barraProgreso && barraProgreso->getPorcentaje() >= 1.0f) {
        // Detener generación de pociones
        if (temporizadorPociones) {
            temporizadorPociones->stop();
            temporizadorPociones->deleteLater();
            temporizadorPociones = nullptr;
        }

        // Detener el temporizador del nivel y ataques enemigos
        if (timerNivel) timerNivel->stop();
        if (robot) robot->detenerAtaques();

        // Esperamos 1 segundo antes de iniciar la secuencia final con el Kamehameha
        QTimer::singleShot(1000, this, [this]() {
            Goku2* goku2 = dynamic_cast<Goku2*>(goku);
            if (goku2 && robot) {
                // Cuando el robot muere, se emite la señal de nivel completado
                connect(robot, &Robot::robotMurio, this, [this]() {
                    emit nivelCompletado();
                });
                // Goku lanza el Kamehameha hacia el robot
                goku2->iniciarKamehameha(robot->getSprite()->x(), robot);
            }
        });
        return;
    }
}

// Crea al enemigo robot, lo escala y lo posiciona al final del escenario
// El posicionamiento se hace usando márgenes fijos respecto al tamaño de la escena
void Nivel2::agregarRobot() {
    const float escala = 3.0;
    const int margenSuelo = 10, margenDerecha = 30;

    robot = new Robot(escena, this);
    robot->getSprite()->setScale(escala);

    const QPixmap& frame0 = robot->getSprite()->pixmap();
    const int anchoRob = frame0.width() * escala;
    const int altoRob  = frame0.height() * escala;

    const int x = 1530 - anchoRob - margenDerecha;
    const int y = 784  - altoRob  - margenSuelo;
    robot->getSprite()->setPos(x, y);
    robot->iniciarAtaques();
}

// Agrega una explosión a la lista (para control de memoria)
void Nivel2::agregarExplosion(Explosion* e) {
    listaExplosiones.push_back(e);
}

// Finaliza el nivel por derrota
void Nivel2::gameOver() {
    if (timerNivel) timerNivel->stop();
    mostrarGameOver();
    emit gokuMurio();
}

// Devuelve el puntero a Goku
Goku* Nivel2::getGoku() const {
    return goku;
}
