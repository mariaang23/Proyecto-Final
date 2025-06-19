#include "juego.h"
#include "ui_juego.h"
#include "obstaculo.h"

juego::juego(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::juego)
    , nivel1(nullptr)
    , nivel2(nullptr)
    , nivelActual(nullptr)
{
    ui->setupUi(this);
    iniciarJuego();
}

juego::~juego()
{
    //finalizarJuego();
    delete ui;
}

void juego::iniciarJuego()
{
    int sceneWidth =  1536 * 3; // Ancho escena
    int sceneHeight = 784;      // Alto escena

    scene = new QGraphicsScene();
    scene -> setSceneRect(0,0,sceneWidth, sceneHeight);

    // Crear la vista que mostrará la escena
    view = new QGraphicsView(scene);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Ocultar barra de navegación vertical
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Ocultar barras de navegación horizontal

    int widthView = sceneWidth / 3; // Ancho vista
    int heightView = sceneHeight;   // Alto vista
    view -> setSceneRect(0, 0, widthView, heightView);
    view -> setFixedSize(widthView, heightView);

    nivel1 = new nivel(scene, view, this);
    nivel1 -> cargarFondo(":/images/background1.png");

    obstaculo *ave = new obstaculo(scene, obstaculo::Ave, 5, this);
    ave -> iniciar(2000, 150);

    obstaculo *montania = new obstaculo(scene, obstaculo::Montania, 5, this);
    montania -> iniciar(3500, 784/2);

    obstaculo *roca = new obstaculo(scene, obstaculo::Roca, 5, this);
    roca -> iniciar(1500, 784/2);

    view -> show();
}

void juego::cambiarNivel(int numero)
{
    /*if (nivelActual)
        nivelActual->ocultar();   // Ocultar nivel actual

    if (numero == 1)
        nivelActual = nivel1;
    else if (numero == 2)
        nivelActual = nivel2;

    if (nivelActual)
        nivelActual->mostrar();   // Mostrar nuevo nivel
    */
}

void juego::finalizarJuego()
{
    delete nivel1;
    delete nivel2;
    nivel1 = nullptr;
    nivel2 = nullptr;
    nivelActual = nullptr;
}
