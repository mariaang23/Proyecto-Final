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
    delete nivel1; // Elimina nivel 1 si existe
    delete nivel2; // Elimina nivel 2 si existe
    delete view;   // Esto también destruye scene
    delete scene;
    delete ui;
}

// Inicia el juego en el nivel 1
void juego::iniciarJuego()
{
    cambiarNivel(2); // Se cambió a nivel 2 para probar fondo y nubes
}

// Cambia entre niveles, manejando la memoria adecuadamente
void juego::cambiarNivel(int numero)
{
    if (nivelActual != nullptr) {
        delete nivelActual; // Elimina nivel actual
        nivelActual = nullptr;
    }

    if (view != nullptr) {
        delete view; // Esto destruye view y su scene
        view = nullptr;
    }

    scene = nullptr; // ya no hay scene activa

    // Crear nuevo nivel
    if (numero == 1) {
        int sceneWidth = 1536 * 3;
        int sceneHeight = 784;

        scene = new QGraphicsScene();
        scene->setSceneRect(0, 0, sceneWidth, sceneHeight);

        view = new QGraphicsView(scene);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        int widthView = sceneWidth / 3;
        int heightView = sceneHeight;

        view->setSceneRect(0, 0, widthView, heightView);
        view->setFixedSize(widthView, heightView);
        view->activateWindow();

        // Crear nivel1 y configurarlo
        nivel1 = new nivel(scene, view, this, 1);
        nivelActual = nivel1;

        nivel1->cargarFondoNivel(":/images/background1.png");
        nivel1->agregarObstaculos();
        nivel1->agregarGokuNivel1();
        nivel1->agregarCarroFinal();

        view->show();

        // Logica para cambiar nivel 2
        //cambiarNivel(2);

    } else {
        int sceneWidth = 1536 * 3;
        int sceneHeight = 784;

        scene = new QGraphicsScene();
        scene->setSceneRect(0, 0, sceneWidth, sceneHeight);

        view = new QGraphicsView(scene);
        view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        int widthView = sceneWidth / 3;
        int heightView = sceneHeight;

        view->setSceneRect(0, 0, widthView, heightView);
        view->setFixedSize(widthView, heightView);
        view->activateWindow();

        // Crear nivel1 y configurarlo
        nivel2 = new nivel(scene, view, this, 2);
        nivelActual = nivel2;
        nivel2->cargarFondoNivel(":/images/background2.png");
        view->show();

    }
}
