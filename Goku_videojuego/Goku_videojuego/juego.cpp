#include "juego.h"
#include "ui_juego.h"
#include <QDebug>
#include <QScreen>
#include <QGuiApplication>
#include <QCloseEvent>
#include <QPointer>
#include <QMediaPlayer>
#include <QAudioOutput>

// Inicialización del contador
int juego::contador = 0;

/**
@brief Constructor de la clase principal `juego`, encargado de inicializar la interfaz gráfica y configurar el entorno inicial.

Este constructor crea la ventana principal del juego, configura la interfaz de usuario y prepara los elementos necesarios para iniciar los niveles:

- Inicializa punteros a los niveles y escena (`nivel1`, `nivel2`, `scene`, `view`) como `nullptr`.
- Centra la ventana principal en la pantalla.
- Conecta el botón de inicio a la función `iniciarJuego()`.
- Muestra la pantalla de bienvenida (`mostrarPantallaInicio()`).
- Crea un temporizador (`timerFoco`) para asegurar que Goku reciba el foco correctamente al iniciar el nivel.

@param parent Puntero al widget padre (opcional).
*/
juego::juego(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::juego)
    , scene(nullptr)
    , view(nullptr)
    , nivel1(nullptr)
    , nivel2(nullptr)
    , nivelActual(nullptr)
    , exito(nullptr)
    , timerFoco(new QTimer(this))
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

    //una sola vez
    timerFoco->setSingleShot(true);
    connect(timerFoco, &QTimer::timeout, this, [this]() {
        //qDebug()<<"timer foco en juego llamado  "<<contador++;
        if (nivelActual && nivelActual->getGoku())
            nivelActual->getGoku()->setFocus();
    });

    qDebug()<<"Creando juego ";
}

/**
@brief Destructor de la clase `juego`, responsable de liberar recursos y cerrar correctamente la aplicación.

Este destructor garantiza una limpieza segura y ordenada de todos los elementos creados durante la ejecución del juego:

- Llama a `cerrarNivel(false)` para detener y destruir cualquier nivel activo.
- Elimina manualmente la escena (`scene`) y la vista (`view`) si existen.
- Libera la interfaz gráfica (`ui`) de Qt.
- Establece los punteros liberados a `nullptr` para evitar referencias colgantes.

Este método se ejecuta automáticamente al cerrar la ventana principal del juego.
*/
juego::~juego()
{
    //qDebug() << "Destructor de juego llamado";

    // Limpiar en orden seguro
    cerrarNivel(false);  // Cierra el nivel sin mostrar menú

    // Eliminar vista y escena si existen
    if (view) {
        view->setScene(nullptr);  // Desvincula la escena de la vista.
        delete scene;             // Libera la escena manualmente.
        qDebug() << "Destruyendo view...";
        delete view;              // Libera la vista.
        qDebug() << "View destruido. Scene aún existe?" << (scene != nullptr);
        scene = nullptr;
        view = nullptr;
    }

    // Eliminar interfaz
    delete ui;

    qDebug() << "Cerrando juego exitoso!!";
}

/**
@brief Inicia una nueva sesión de juego, configurando la vista, escena y cargando el primer nivel.

Este método se activa al presionar el botón de inicio en la interfaz principal. Realiza los siguientes pasos:

- Oculta y desactiva la pantalla de bienvenida.
- Crea una nueva ventana `QGraphicsView` y una escena `QGraphicsScene` donde se desarrollará el juego.
- Configura la vista con tamaño fijo, sin barras de desplazamiento y con suavizado de bordes.
- Conecta la destrucción de la vista a la limpieza del nivel actual mediante `cerrarNivel(true)`.
- Llama a `cambiarNivel(2)` para cargar el segundo nivel.
- Muestra la ventana del juego centrada en la pantalla.

@see juego::cambiarNivel
@see juego::cerrarNivel
*/
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

/**
@brief Cambia el nivel actual del juego, destruyendo el anterior y cargando uno nuevo.

Este método permite alternar entre el Nivel 1 y el Nivel 2. Realiza las siguientes acciones:

- Llama a `cerrarNivel(false)` para liberar el nivel anterior.
- Ajusta el tamaño de la escena según el nivel seleccionado.
- Crea una nueva instancia de `Nivel1` o `Nivel2` y lo asigna como `nivelActual`.
- Conecta señales del nivel para manejar eventos como la muerte de Goku o la finalización del nivel.
- Inicia el nuevo nivel mediante `iniciarNivel()`.
- Activa el temporizador `timerFoco` para garantizar que Goku reciba el foco tras cargar el nivel.

@param numero Número del nivel a cargar (1 o 2).

@see juego::cerrarNivel
@see Nivel::iniciarNivel
@see juego::mostrarPantallaInicio
*/
void juego::cambiarNivel(int numero)
{
    qDebug() << "Cambiando a nivel" << numero;

    // antes de crear un nuevo nivel, cerramos correctamente el actual
    cerrarNivel(false);   // esto hara delete nivel1/nivel2 si estaban vivos

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

                    //qDebug() << "timer singleshot nivel1 en juego llamado"<<contador++;
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

                    //qDebug() << "timer single nivel2 en juego llamado  "<<contador++;
                    if (view) view->close();
                    mostrarPantallaInicio();
                });
            });

            connect(nivel2, &Nivel2::nivelCompletado, this, &juego::mostrarExito);
        }

        nivelActual->iniciarNivel();

        timerFoco->start(100); //actualiza el foco cuando se cambia nivel, no crea uno nuevo
        /*
        // Asegurar foco en el personaje principal
        QTimer::singleShot(100, this, [this]() {
            if (nivelActual && nivelActual->getGoku()) {
                qDebug() << "timer nivelactual en juego llamado";
                nivelActual->getGoku()->setFocus();
            }
        });*/

    } catch (const std::exception& e) {
        qCritical() << "Error al crear nivel:" << e.what();
        if (view) view->close();
        mostrarPantallaInicio();
    }
}

/**
@brief Cierra y limpia el nivel actual, liberando todos sus recursos.

Este método se encarga de destruir correctamente el nivel en curso (`nivel1` o `nivel2`), desconectar sus señales
y limpiar la escena asociada. Además, detiene el temporizador `timerFoco` y cualquier `QTimer` en modo `singleShot`
que esté pendiente (como los usados tras `gokuMurio`).

@param mostrarMenu Si es `true`, muestra nuevamente la pantalla de bienvenida al finalizar la limpieza.

@see juego::cambiarNivel
@see juego::mostrarPantallaInicio
*/
void juego::cerrarNivel(bool mostrarMenu)
{
    qDebug() << "Cerrando nivel actual";

    // Desconectar señales primero
    if (nivelActual) {
        disconnect(nivelActual, nullptr, this, nullptr);
    }

    //Eliminar niveles
    if (nivel1) {
        delete nivel1;
        nivel1 = nullptr;
    }
    if (nivel2) {
        delete nivel2;
        nivel2 = nullptr;
    }

    nivelActual = nullptr;
    if (scene) {
        scene->clear();
    }

    //detenemos el timer foco goku
    if (timerFoco && timerFoco->isActive())
        timerFoco->stop();

    // Cancela cualquier singleShot pendiente generado por gokuMurio
    for (auto *t : findChildren<QTimer*>()){
        if (t->isSingleShot() && t->isActive()){
            t->stop();
        }
    }


    if (mostrarMenu) {
        mostrarPantallaInicio();
    }
}

/**
@brief Muestra la pantalla de inicio del juego y reactiva la interfaz gráfica principal.

Este método se utiliza para regresar al menú inicial después de cerrar un nivel o completar una partida.

- Muestra el `widget` de bienvenida y habilita la interacción del botón de inicio.
- Establece el foco en el botón `ui->botonIniciar`.
- Se asegura de que la ventana principal esté visible.

@see juego::cerrarNivel
@see juego::iniciarJuego
*/
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

/**
@brief Muestra una animación de transición visual entre niveles y luego cambia al Nivel 2.

Este método detiene temporalmente el nivel actual y presenta una imagen de transición centrada sobre la vista del juego.
Después de 4 segundos, se cierra la transición y se invoca `cambiarNivel(2)` para continuar con el juego.

- La imagen utilizada es `:/images/transicion.png`.
- La transición se escala al tamaño de la vista y se destruye automáticamente tras mostrarse.

@see juego::cambiarNivel
*/
void juego::mostrarTransicion()
{
    if (!view || !nivelActual) return;

    nivelActual->setEnabled(false); // Pausar el nivel

    transicion = new QLabel(view);
    transicion->setPixmap(QPixmap(":/images/transicion.png")
                              .scaled(view->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    transicion->setAlignment(Qt::AlignCenter);
    transicion->setAttribute(Qt::WA_DeleteOnClose);
    transicion->show();
  
    QTimer::singleShot(4000, this, [this]() {
        //qDebug() << "timer transicion en juego  llamado  "<<contador++;
        transicion->close();
        delete transicion;
        cambiarNivel(2);
    });
}

/**
@brief Muestra una pantalla de éxito al completar un nivel y luego retorna al menú principal.

Este método pausa el nivel actual y despliega una imagen de felicitación (`:/images/exito.png`) centrada sobre la vista del juego.
Después de 4 segundos, se cierra la vista y se muestra nuevamente la pantalla de inicio.

- La imagen se adapta al tamaño actual de la vista.
- La etiqueta `QLabel` se destruye automáticamente tras cerrarse.

@see juego::mostrarPantallaInicio
*/
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

        //qDebug() << "timer mostrar pantalla de inicio en juego llamado  "<<contador++;
        exito->close();
        delete exito;
        if (view) view->close();
        mostrarPantallaInicio();
    });
}

/**
@brief Retorna al menú principal tras una derrota en el juego.

Este método se utiliza cuando el jugador pierde. Realiza los siguientes pasos:

- Cierra y elimina la vista del juego si está activa.
- Llama a `cerrarNivel(false)` para limpiar el estado del nivel sin mostrar el menú dentro de ese método.
- Llama a `mostrarPantallaInicio()` para regresar a la pantalla de bienvenida.

@see juego::cerrarNivel
@see juego::mostrarPantallaInicio
*/
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

/**
@brief Maneja el evento de cierre de la ventana principal del juego.

Este método se ejecuta automáticamente cuando el usuario cierra la ventana principal (`juego`).

- Cierra y elimina la vista del juego si está activa (`view`).
- Llama a `cerrarNivel(false)` para garantizar la limpieza del nivel actual.
- Propaga el evento de cierre al `QMainWindow` base para completar el proceso.

@param event Puntero al evento de cierre (`QCloseEvent`) proporcionado por Qt.

@see juego::cerrarNivel
*/
void juego::closeEvent(QCloseEvent *event)
{
    // Cerrar la ventana del juego si esta abierta
    if (view) {
        view->close();
        view->deleteLater();
        view = nullptr;
    }

    // Asegurar limpieza
    cerrarNivel(false);

    QMainWindow::closeEvent(event);
}
