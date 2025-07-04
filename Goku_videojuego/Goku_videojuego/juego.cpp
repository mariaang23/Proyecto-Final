#include "juego.h"
#include "ui_juego.h"
#include <QDebug>
#include <QScreen>
#include <QGuiApplication>

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

    // Centrar ventana en la pantalla
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);

    // Conectar botón de iniciar con la función que arranca el juego
    connect(ui->botonIniciar, &QPushButton::clicked, this, &juego::iniciarJuego);

}

// Destructor: libera todos los recursos
juego::~juego()
{
    qDebug() << "Destructor de juego llamado";
    delete nivel1;
    delete nivel2;
    if(view != nullptr){
        delete view;
    }
    delete scene;
    delete ui;
    delete timerEstado;
}

// Inicia el juego en el nivel 1
void juego::iniciarJuego()
{
    // Ocultar los elementos del menú
    if (nivelActual || view || scene){
        cerrarNivel(false);          // cierra recursos sin mostrar de nuevo el menu
    }

    ui->widget->hide();              // imagen / panel de bienvenida
    ui->widget->setEnabled(false);
    ui->botonIniciar->hide();
    ui->botonIniciar->clearFocus();  // libera el foco

    cambiarNivel(1);                 // se encarga de crear scene, view y Nivel1

}

// Cambia entre niveles, manejando la memoria adecuadamente
void juego::cambiarNivel(int numero)
{
    // Elimina el nivel actual si existe
    if (nivelActual != nullptr) {
        delete nivelActual;
        nivelActual = nullptr;
    }

    // Elimina la vista si existe
    if (view != nullptr) {
        delete view;
        view = nullptr;
    }

    scene = nullptr;

    // Configuración del tamaño de la escena
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
    view->setFocusPolicy(Qt::NoFocus); // para que no robe el foco de Goku

    // Crear el nivel correspondiente
    if (numero == 1) {
        nivel1 = new Nivel1(scene, view, this);
        //cinectar nivel con señal de gokuMurio
        connect(nivel1, &Nivel1::gokuMurio,this, [this]{QTimer::singleShot(
                        10000, this, &juego::regresarAlMenuTrasDerrota); // slot que cierra y muestra menu y 2500mls
                });
        nivel1->iniciarNivel(); // aquí se le da foco a Goku
        QTimer::singleShot(100, this, [=]() {
            if (nivel1 && nivel1->getGoku())
                nivel1->getGoku()->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
        });
        nivelActual = nivel1;

    } else {
        nivel2 = new Nivel2(scene, view, this);
        nivel2->iniciarNivel(); // aquí también debe darse foco a Goku
        QTimer::singleShot(100, this, [=]() {
            if (nivel2 && nivel2->getGoku())
                nivel2->getGoku()->setFocus(Qt::FocusReason::ActiveWindowFocusReason);
        });
        nivelActual = nivel2;
    }

    view->show();
}

void juego::mostrarPantallaInicio()
{
    ui->widget->show();
    ui->widget->setEnabled(true);
    ui->botonIniciar->show();
    ui->botonIniciar->setFocus();
}

void juego::cerrarNivel(bool mostrarMenu)
{
    delete nivelActual;
    nivelActual = nullptr;
    if (view) {
        view->deleteLater();
        view = nullptr;
    }
    delete scene;
    scene = nullptr;

    if (mostrarMenu) mostrarPantallaInicio();
}

void juego::regresarAlMenuTrasDerrota()
{
    cerrarNivel(true);          // elimina todo y muestra el menú
}


