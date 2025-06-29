#include "juego.h"
#include "ui_juego.h"
#include <QDebug>

// Constructor de la ventana principal del juego
juego::juego(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::juego)
    , view(nullptr)
    , nivel1(nullptr)
    , nivel2(nullptr)
    , nivelActual(nullptr)
{
    ui->setupUi(this); // Inicializa interfaz gráfica
    iniciarJuego();    // Inicia juego automáticamente
}

// Destructor: libera todos los recursos
juego::~juego()
{
    delete nivel1;
    delete nivel2;
    delete view;
    view = nullptr; // Establecer en nullptr despes de eliminar
    delete scene;
    delete ui;
}

// Inicia el juego en el nivel 1
void juego::iniciarJuego()
{
    cambiarNivel(1); //Para probar ambos niveles
}

// Cambia entre niveles, manejando la memoria adecuadamente
void juego::cambiarNivel(int numero)
{
    if (nivelActual != nullptr) {
        delete nivelActual;
        nivelActual = nullptr;
    }

    if (view != nullptr) {
        delete view;
        view = nullptr;
    }

    scene = nullptr;

    // Configuración común
    int sceneWidth = (numero == 1) ? 1536 * 4 : 1536;
    int sceneHeight = 784;

    scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, sceneWidth, sceneHeight);

    view = new QGraphicsView(scene);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    int widthView = (numero == 1) ? sceneWidth / 4 : sceneWidth;
    int heightView = sceneHeight;

    view->setSceneRect(0, 0, widthView, heightView);
    view->setFixedSize(widthView, heightView);
    view->activateWindow();

    if (numero == 1) {
        nivel1 = new Nivel1(scene, view, this);
        nivel1->iniciarNivel();
        nivelActual = nivel1;
    } else {
        nivel2 = new Nivel2(scene, view, this);
        nivel2->iniciarNivel();
        nivelActual = nivel2;
    }

    view->show();
}
