#include "nivel2.h"
#include "pocion.h"
#include "goku2.h"
#include "robot.h"
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <QRandomGenerator>
#include <QTimer>
#include <QDebug>

Nivel2::Nivel2(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent)
    : Nivel(escena, vista, parent, 2),
    robotInicialCreado(false),
    pocionesAgregadas(false),
    barraProgreso(nullptr),
    temporizadorPociones(nullptr)
{}

Nivel2::~Nivel2()
{
    qDebug() << "Destructor de Nivel2 llamado";

    // 1. Eliminar cada poción creada dinámicamente
    for (auto* pocion : listaPociones)
        delete pocion;
    listaPociones.clear();

    // 2. Detener y eliminar el temporizador de pociones si existe
    if (temporizadorPociones) {
        temporizadorPociones->stop();
        delete temporizadorPociones;
        temporizadorPociones = nullptr;
    }
    //Liberar explosiones
    for (auto* exp : listaExplosiones){
        delete exp;
    }
    listaExplosiones.clear();
}

void Nivel2::iniciarNivel() {
    cargarFondoNivel(":/images/background2.png");
    generarNubes();

    // Crear barra de vida
    barraVida = new Vida(vista);      // Usa la misma vista que la barra de progreso
    barraVida->move(20, 20);          // Posición superior
    barraVida->show();

    // Crear barra de progreso para las pociones
    barraProgreso = new Progreso(Pociones, ":/images/icono_pocion.png", vista);
    barraProgreso->move(20, 60);
    barraProgreso->setTotalPociones(totalPociones);
    barraProgreso->show();

    agregarPociones(); // Poción inicial

    // Timer para seguir generando más pociones
    temporizadorPociones = new QTimer(this);
    connect(temporizadorPociones, &QTimer::timeout, this, &Nivel2::agregarPocionAleatoria);
    temporizadorPociones->start(2500); // cada 1 segundo

    agregarGoku();
    agregarRobot();
}

void Nivel2::cargarFondoNivel(const QString &ruta) {
    background = QPixmap(ruta);
    QGraphicsPixmapItem* _background = new QGraphicsPixmapItem(background);
    _background->setPos(0, 0);
    escena->addItem(_background);
    listaFondos.push_back(_background);
}

void Nivel2::agregarGoku() {
    int anchoFrame = 218;
    int altoFrame = 298;
    int velocidad = 12;

    goku = new Goku2(escena, velocidad, anchoFrame, altoFrame, this);
    static_cast<Goku2*>(goku)->cargarImagen();

    //vida de goku
    goku->setBarraVida(barraVida);

    int xInicial = 100;
    int yInicial = vista->height() - altoFrame - 30;

    Goku2* goku2 = dynamic_cast<Goku2*>(goku);
    if (goku2) {
        goku2->setSueloY(yInicial);
    }

    goku->iniciar(xInicial, yInicial);
}

void Nivel2::agregarPociones() {
    QPixmap hojaSprites(":/images/pocion.png");

    int anchoSprite = 65;
    int altoSprite = 64;

    for (int i = 0; i < 6; ++i) {
        QPixmap frame = hojaSprites.copy(i * anchoSprite, 0, anchoSprite, altoSprite);
        framesPocion.push_back(frame);
    }

    // Crea una sola poción inicial para que empiece el nivel
    int anchoVista = vista->width();
    Pocion* pocion = new Pocion(framesPocion, anchoVista, 0, 0, 1);
    escena->addItem(pocion);
    listaPociones.push_back(pocion);
}

void Nivel2::agregarPocionAleatoria() {
    if (!barraProgreso || barraProgreso->getPorcentaje() >= 1.0f) {
        if (temporizadorPociones) {
            temporizadorPociones->stop();
            temporizadorPociones->deleteLater();
            temporizadorPociones = nullptr;
        }
        return;
    }

    int fila = QRandomGenerator::global()->bounded(0, 2);
    int columna = QRandomGenerator::global()->bounded(0, 7);
    int anchoVista = vista->width();

    Pocion* nuevaPocion = new Pocion(framesPocion, anchoVista, fila, columna, 7);
    escena->addItem(nuevaPocion);
    listaPociones.push_back(nuevaPocion);
}

void Nivel2::pocionRecolectada() {
    if (barraProgreso)
        barraProgreso->sumarPocion();
}

void Nivel2::actualizarNivel()
{
    if (!goku) return;

    //goku murio?
    if (barraVida && barraVida->obtenerVida() <= 0) {// Vida llega a 0
        perdioGoku = true;
        gameOver();
        return;
    }

    //si completa todas las pociones
    if (barraProgreso && barraProgreso->getPorcentaje() >= 1.0f) {

        // Dejamos de crear pociones nuevas
        if (temporizadorPociones) {
            temporizadorPociones->stop();
            temporizadorPociones->deleteLater();
            temporizadorPociones = nullptr;
        }

        //detener ataques del robot
        if (timerNivel) timerNivel->stop();

        //emite nivel completado si completa las pociones (por ahora)
        QTimer::singleShot(1000, this, [this]() {
            emit nivelCompletado();
        });
        return;
    }
}


void Nivel2::agregarRobot()
{
    const float escala       = 3.0;   // escala
    const int   margenSuelo = 10;
    const int   margenDerecha   = 30;

    robot = new Robot(escena, this);          // crea el sprite
    robot->getSprite()->setScale(escala);     // primero la escala

    //alto y ancho la escala
    const QPixmap& frame0 = robot->getSprite()->pixmap();
    const int anchoRob = frame0.width()  * escala;
    const int altoRob = frame0.height() * escala;

    // coloca abajo derecha en el suelo
    const int x = 1530  - anchoRob - margenDerecha;
    const int y = 784 - altoRob  - margenSuelo;

    robot->getSprite()->setPos(x, y);

    escena->addItem(robot->getSprite());

    robot->iniciarAtaques();
}

void Nivel2::agregarExplosion(Explosion* e) {
    listaExplosiones.push_back(e);
}

void Nivel2::gameOver()
{
    if (timerNivel) timerNivel->stop();

    mostrarGameOver();          //  heredado de Nivel
    emit gokuMurio();           // seguirá avisando al objeto juego
}

Goku* Nivel2::getGoku() const {
    return goku;
}
