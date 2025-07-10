#include "nivel.h"
#include <QRandomGenerator>
#include <QGraphicsPixmapItem>
#include <stdexcept>  // Para lanzar excepciones estándar
#include <QDebug>

// Inicialización del contador de nubes compartido entre niveles
int Nivel::contNubes = 0;
// Inicialización del contador
int Nivel::contador = 0;

// Constructor base del nivel abstracto
// Valida escena y vista antes de usarlas
Nivel::Nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent, int numero)
    : QWidget(parent), vista(view), escena(escena), numeroNivel(numero)
{
    // Validación de parámetros críticos (¡Ahora más robusta!)
    if (!escena || !view) {
        qCritical() << "Nivel: La escena o la vista son nulas. Nivel:" << numero;
        throw std::invalid_argument("Nivel: La escena o la vista no pueden ser nulas.");
    }

    // Inicializa el temporizador principal del nivel (actualiza la lógica cada 20 ms)
    // Timer ahora se desconecta explícitamente en destructor
    timerNivel = new QTimer(this);
    connect(timerNivel, &QTimer::timeout, this, [=]() {

        //qDebug() << "timer nivel en nivel  llamado  "<<contador++;
        this->actualizarNivel();  // Llama al método virtual (definido por subclases)
    });
    timerNivel->start(20);

    qDebug() << "Nivel" << numero << "creado correctamente en nivel Padre";
}

// Destructor del nivel base
// Limpia en orden inverso y es más seguro
Nivel::~Nivel()
{
    qDebug() << "Destructor de Nivel padre llamado para destruir el nivel" << numeroNivel;

    // 1. Detener todos los timers primero (¡CRÍTICO!)
    if (timerNivel) {
        timerNivel->stop();
        disconnect(timerNivel, nullptr, this, nullptr);
    }

    if (timerNubes) {
        timerNubes->stop();
        disconnect(timerNubes, nullptr, this, nullptr);
    }

    // 2. Limpieza directa de items gráficos (sin llamada virtual)
    if (goku && escena) {
        escena->removeItem(goku);
        delete goku;
        goku = nullptr;
    }

    for (auto* nube : listaNubes) {
        if (nube && escena) escena->removeItem(nube);
        delete nube;
    }
    listaNubes.clear();

    for (auto* fondo : listaFondos) {
        if (fondo && escena) escena->removeItem(fondo);
        delete fondo;
    }
    listaFondos.clear();

    // 3. Eliminar otros objetos (orden inverso al de creación)

    if (barraVida && barraVida->parent() == this) {
        delete barraVida;
        barraVida = nullptr;
    }

    if (barraProgreso) {
        barraProgreso->setParent(nullptr);  // Desvincula de la vista
        barraProgreso->hide();              // Evita que quede visible si tarda en destruirse
        delete barraProgreso;
        barraProgreso = nullptr;
    }

    delete timerNivel;
    timerNivel = nullptr;

    delete timerNubes;
    timerNubes = nullptr;

    if (overlayGameOver) {
        overlayGameOver->deleteLater();
        overlayGameOver = nullptr;
    }

    qDebug() << "Nivel" << numeroNivel << "destruido correctamente desde padre nivel";
}

// Mantenemos limpiarEscena() para uso durante el juego (no en destructor)
void Nivel::limpiarEscena()
{
    // Implementación idéntica a lo que estaba en el destructor
    if (goku && escena) {
        escena->removeItem(goku);
        delete goku;
        goku = nullptr;
    }

    for (auto* nube : listaNubes) {
        if (nube && escena) escena->removeItem(nube);
        delete nube;
    }
    listaNubes.clear();

    for (auto* fondo : listaFondos) {
        if (fondo && escena) escena->removeItem(fondo);
        delete fondo;
    }
    listaFondos.clear();

    if (overlayGameOver) {
        overlayGameOver->hide();
        overlayGameOver->deleteLater();
    }
}

// Devuelve el espacio reservado para el HUD (barra de vida, progreso, etc.)
int Nivel::getMargenHUD() const {
    return margenHUD;
}

// Genera múltiples nubes en la escena, con escalas y posiciones aleatorias
// Valida la carga de imagen
void Nivel::generarNubes()
{
    // Carga la imagen de la nube desde recursos
    nube = QPixmap(":/images/nube.png");

    // Validación más explícita
    if (nube.isNull()) {
        qCritical() << "Error: No se pudo cargar imagen de nube en nivel" << numeroNivel;
        throw std::runtime_error("Nivel: No se pudo cargar la imagen de la nube.");
    }

    // Genera múltiples nubes distribuidas horizontalmente
    for (int i = 0; i < 35; i++) {
        for (int j = 3; j > 0; --j) {
            float escala = j * 0.05f;
            int ancho = nube.width() * escala;
            int alto  = nube.height() * escala;

            QPixmap nubeEscalada = nube.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            int x = i * 250 + QRandomGenerator::global()->bounded(-60, 100);
            int y = QRandomGenerator::global()->bounded(0, 80);

            if (x + nubeEscalada.width() <= escena->width()) {
                contNubes += 1;
                QGraphicsPixmapItem *_nube = new QGraphicsPixmapItem(nubeEscalada);
                _nube->setPos(x, y);
                escena->addItem(_nube);
                listaNubes.push_back(_nube);
            }
        }
    }

    // Timer ahora se desconecta en destructor
    timerNubes = new QTimer(this);
    connect(timerNubes, &QTimer::timeout, this, &Nivel::moverNubes);
    timerNubes->start(45);
}

// Mueve todas las nubes hacia la izquierda y las reposiciona si salen de la escena
// Valida punteros
void Nivel::moverNubes()
{

    //qDebug() << "timermover nubes en nivel llamado  "<<contador++;
    const int velocidadNube = 2;

    for (auto *nubeItem : listaNubes) {
        if (!nubeItem || !escena) continue;  // Validación defensiva

        nubeItem->setPos(nubeItem->x() - velocidadNube, nubeItem->y());

        if (nubeItem->x() + nubeItem->pixmap().width() < 0) {
            int nuevaX = escena->width();
            int nuevaY = QRandomGenerator::global()->bounded(0, 100);
            nubeItem->setPos(nuevaX, nuevaY);
        }
    }
}

// Muestra una imagen de "Game Over" sobre la vista del nivel actual
// Verifica vista y evita duplicados
void Nivel::mostrarGameOver()
{
    if (overlayGameOver || !vista) return;  // Evita recrear si ya existe

    try {
        overlayGameOver = new QLabel(vista);
        QPixmap gameOverImg(":/images/gameOver.png");
        if (gameOverImg.isNull()) throw std::runtime_error("Imagen no encontrada");

        overlayGameOver->setPixmap(gameOverImg.scaled(vista->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        overlayGameOver->setAlignment(Qt::AlignCenter);
        overlayGameOver->setAttribute(Qt::WA_TransparentForMouseEvents);
        overlayGameOver->setAttribute(Qt::WA_DeleteOnClose);
        overlayGameOver->show();

    } catch (const std::exception& e) {
        qCritical() << "Error al mostrar Game Over:" << e.what();
    }
}
