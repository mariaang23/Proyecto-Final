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

    // Centrar ventana en la pantalla
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);

    // Conectamos el botón de iniciar con la función que arranca el juego
    connect(ui->botonIniciar, &QPushButton::clicked, this, &juego::iniciarJuego);
}

// Destructor: libera todos los recursos
juego::~juego()
{
    qDebug() << "Destructor de juego llamado";
    delete nivel1;
    delete nivel2;
    delete view;
    view = nullptr;
    delete scene;
    delete ui;
}

// Inicia el juego en el nivel 1
void juego::iniciarJuego()
{
    // Ocultar los elementos del menú
    ui->botonIniciar->hide();
    ui->widget->hide();
    ui->widget->setEnabled(false);      // Evita que el widget del menú reciba foco
    ui->widget->clearFocus();           // Libera foco del menú

    cambiarNivel(1);                    // Cambia al nivel 1
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
