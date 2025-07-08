#include "juego.h"
#include "ui_juego.h"
#include <QDebug>
#include <QScreen>
#include <QGuiApplication>
#include <QCloseEvent>

juego::juego(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::juego)
    , scene(nullptr)
    , view(nullptr)
    , nivel1(nullptr)
    , nivel2(nullptr)
    , nivelActual(nullptr)
    , exito(nullptr)
{
    ui->setupUi(this);

    // Centrar ventana principal
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - this->width()) / 2;
    int y = (screenGeometry.height() - this->height()) / 2;
    this->move(x, y);

    // Configurar botón de inicio
    connect(ui->botonIniciar, &QPushButton::clicked, this, &juego::iniciarJuego);

    // Mostrar solo la pantalla de bienvenida inicialmente
    mostrarPantallaInicio();
}

juego::~juego()
{
    qDebug() << "Destructor de juego llamado";

    // Limpiar en orden seguro
    cerrarNivel(false);  // Cierra el nivel sin mostrar menú

    // Eliminar vista y escena si existen
    if (view) {
        view->close();
        delete view;
        view = nullptr;
    }

    if (scene) {
        delete scene;
        scene = nullptr;
    }

    // Eliminar interfaz
    delete ui;
}

void juego::iniciarJuego()
{
    // Ocultar completamente la interfaz de bienvenida
    ui->widget->hide();
    ui->widget->setEnabled(false);
    ui->botonIniciar->hide();
    ui->botonIniciar->clearFocus();

    // Crear la vista del juego como ventana independiente
    view = new QGraphicsView();
    view->setWindowTitle("Goku Adventure");
    view->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    // Crear escena para los niveles
    scene = new QGraphicsScene();
    view->setScene(scene);

    // Configurar vista
    view->setFixedSize(1536, 784); // Tamaño fijo para el juego
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setRenderHint(QPainter::Antialiasing);

    // Conectar señal de cierre de la vista
    connect(view, &QGraphicsView::destroyed, this, [this]() {
        if (nivelActual) {
            cerrarNivel(true);
        }
    });

    // Iniciar el primer nivel
    cambiarNivel(1);

    // Mostrar la ventana del juego
    view->show();

    // Forzar actualización de eventos para obtener geometría real
    QApplication::processEvents();

    // Centrar la vista en pantalla
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    QRect frame = view->frameGeometry(); // incluye bordes y barra de título

    int vx = (screenGeometry.width() - frame.width()) / 2;
    int vy = (screenGeometry.height() - frame.height()) / 2;
    view->move(vx, vy);
}

void juego::cambiarNivel(int numero)
{
    qDebug() << "Cambiando a nivel" << numero;

    // Limpiar nivel actual si existe
    if (nivelActual) {
        delete nivelActual;
        nivelActual = nullptr;
    }

    // Configurar tamaño de escena según el nivel
    int sceneWidth = (numero == 1) ? 1536 * 4 : 1536; // Nivel 1 es más ancho
    int sceneHeight = 784;

    scene->clear();
    scene->setSceneRect(0, 0, sceneWidth, sceneHeight);

    // Crear nivel específico
    try {
        if (numero == 1) {
            nivel1 = new Nivel1(scene, view, this);
            nivelActual = nivel1;

            connect(nivel1, &Nivel1::gokuMurio, this, [this]() {
                QTimer::singleShot(3000, this, [this]() {
                    if (view) view->close();
                    mostrarPantallaInicio();
                });
            });

            connect(nivel1, &Nivel1::nivelCompletado, this, &juego::mostrarTransicion);

        } else {
            nivel2 = new Nivel2(scene, view, this);
            nivelActual = nivel2;

            connect(nivel2, &Nivel2::gokuMurio, this, [this]() {
                QTimer::singleShot(3000, this, [this]() {
                    if (view) view->close();
                    mostrarPantallaInicio();
                });
            });

            connect(nivel2, &Nivel2::nivelCompletado, this, &juego::mostrarExito);
        }

        nivelActual->iniciarNivel();

        // Asegurar foco en el personaje principal
        QTimer::singleShot(100, this, [this]() {
            if (nivelActual && nivelActual->getGoku()) {
                nivelActual->getGoku()->setFocus();
            }
        });

    } catch (const std::exception& e) {
        qCritical() << "Error al crear nivel:" << e.what();
        if (view) view->close();
        mostrarPantallaInicio();
    }
}

void juego::cerrarNivel(bool mostrarMenu)
{
    qDebug() << "Cerrando nivel actual";

    // Desconectar señales primero
    if (nivelActual) {
        disconnect(nivelActual, nullptr, this, nullptr);
    }

    // Limpiar niveles específicos
    if (nivel1) {
        delete nivel1;
        nivel1 = nullptr;
    }

    if (nivel2) {
        delete nivel2;
        nivel2 = nullptr;
    }

    nivelActual = nullptr;

    // Limpiar escena
    if (scene) {
        scene->clear();
    }

    if (mostrarMenu) {
        mostrarPantallaInicio();
    }
}

void juego::mostrarPantallaInicio()
{
    // Mostrar elementos de bienvenida
    ui->widget->show();
    ui->widget->setEnabled(true);
    ui->botonIniciar->show();
    ui->botonIniciar->setFocus();

    // Asegurar que la ventana principal esté visible
    this->show();
}

void juego::mostrarTransicion()
{
    if (!view || !nivelActual) return;

    nivelActual->setEnabled(false); // Pausar el nivel

    QLabel *transicion = new QLabel(view);
    transicion->setPixmap(QPixmap(":/images/transicion.png")
                              .scaled(view->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    transicion->setAlignment(Qt::AlignCenter);
    transicion->setAttribute(Qt::WA_DeleteOnClose);
    transicion->show();

    QTimer::singleShot(2000, this, [this, transicion]() {
        transicion->close();
        cambiarNivel(2); // Cambiar al siguiente nivel
    });
}

void juego::mostrarExito()
{
    if (!view) return;

    nivelActual->setEnabled(false); // Pausar el nivel

    exito = new QLabel(view);
    exito->setPixmap(QPixmap(":/images/exito.png")
                         .scaled(view->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    exito->setAlignment(Qt::AlignCenter);
    exito->setAttribute(Qt::WA_DeleteOnClose);
    exito->show();

    QTimer::singleShot(4000, this, [this]() {
        if (view) view->close();
        mostrarPantallaInicio();
    });
}


void juego::regresarAlMenuTrasDerrota()
{
    // Cierra la ventana del juego si existe
    if (view) {
        view->close();
        delete view;
        view = nullptr;
    }

    // Limpia el nivel actual
    cerrarNivel(false);

    // Muestra la pantalla de inicio
    mostrarPantallaInicio();
}

void juego::closeEvent(QCloseEvent *event)
{
    // Cerrar la ventana del juego si está abierta
    if (view) {
        view->close();
    }

    // Asegurar limpieza
    cerrarNivel(false);

    QMainWindow::closeEvent(event);
}
