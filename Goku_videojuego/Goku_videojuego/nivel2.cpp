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

void Nivel2::pocionRecolectada()
{
    if (barraProgreso) {
        barraProgreso->sumarPocion();
    }
}

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

void Nivel2::gameOver() {
    if (timerNivel) timerNivel->stop();
    if (temporizadorPociones) temporizadorPociones->stop();
    mostrarGameOver();
    emit gokuMurio();
}

bool Nivel2::haTerminado() const
{
    return barraProgreso && barraProgreso->getPorcentaje() >= 1.0f;
}

Goku* Nivel2::getGoku() const
{
    return goku;
}
