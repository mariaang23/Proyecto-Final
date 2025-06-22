#include "juego.h"
#include "ui_juego.h"

juego::juego(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::juego)
    , view(nullptr)
    , nivel1(nullptr)
    , nivel2(nullptr)
    , nivelActual(nullptr)
{
    ui->setupUi(this);
    iniciarJuego();
}

juego::~juego()
{
    delete nivel1;
    delete nivel2;
    delete view;
    delete scene;
    delete ui;
}

void juego::iniciarJuego()
{
    cambiarNivel(1);

}

void juego::cambiarNivel(int numero)
{
    if (numero == 1) {

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

        view->activateWindow();

        nivel1 = new nivel(scene, view, this);
        nivel1 -> cargarFondoNivel1(":/images/background1.png");

        nivel1->agregarObstaculos();
        nivel1 -> agregarGokuNivel1();

        nivel1->agregarCarroFinal();

        view -> show();

        nivelActual = nivel1;
    }

    // Nivel 2
}


