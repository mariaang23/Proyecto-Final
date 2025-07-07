#include "nivel.h"
#include <QRandomGenerator>
#include <QGraphicsPixmapItem>
#include <stdexcept>  // Para lanzar excepciones estándar

// Inicialización del contador de nubes compartido entre niveles
int Nivel::contNubes = 0;


// Constructor base del nivel abstracto
Nivel::Nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent, int numero)
    : QWidget(parent), numeroNivel(numero), vista(view), escena(escena)
{
    // Validación de parámetros críticos
    if (!escena || !view) {
        throw std::invalid_argument("Nivel: La escena o la vista no pueden ser nulas.");
    }

    // Inicializa el temporizador principal del nivel (actualiza la lógica cada 20 ms)
    timerNivel = new QTimer(this);
    connect(timerNivel, &QTimer::timeout, this, [=]() {
        this->actualizarNivel();  // Llama al método virtual (definido por subclases)
    });
    timerNivel->start(20);
}

// Destructor del nivel base
Nivel::~Nivel()
{
    delete camara;
    camara = nullptr;

    delete goku;
    goku = nullptr;

    // Vida: hereda de QWidget
    if (barraVida && barraVida->parent() == this) {
        delete barraVida;
        barraVida = nullptr;
    }

    // Progreso: hereda de QWidget
    if (barraProgreso && barraProgreso->parent() == this) {
        delete barraProgreso;
        barraProgreso = nullptr;
    }

    // Temporizadores: QObject con parent asignado => se eliminan solos
    // Pero por seguridad, los detenemos antes
    if (timerNivel) {
        timerNivel->stop();
    }
    if (timerNubes) {
        timerNubes->stop();
    }

    // Aunque tienen parent, por claridad se eliminan explícitamente
    delete timerNivel;
    timerNivel = nullptr;

    delete timerNubes;
    timerNubes = nullptr;

    // Libera nubes gráficas y ajusta contador global
    for (auto* nube : listaNubes) {
        contNubes -= 1;
        delete nube;
    }
    listaNubes.clear();

    // Libera elementos del fondo
    for (auto* fondo : listaFondos)
        delete fondo;
    listaFondos.clear();

    // Limpieza segura del overlay GameOver si Qt no lo destruye
    if (overlayGameOver && overlayGameOver->parent() == this) {
        delete overlayGameOver;
        overlayGameOver = nullptr;
    }
}


// Devuelve el espacio reservado para el HUD (barra de vida, progreso, etc.)
int Nivel::getMargenHUD() const {
    return margenHUD;
}


// Genera múltiples nubes en la escena, con escalas y posiciones aleatorias
void Nivel::generarNubes()
{
    // Carga la imagen de la nube desde recursos
    nube = QPixmap(":/images/nube.png");

    // Verifica si la imagen se cargó correctamente
    if (nube.isNull()) {
        throw std::runtime_error("Nivel: No se pudo cargar la imagen de la nube.");
    }

    // Genera múltiples nubes distribuidas horizontalmente
    for (int i = 0; i < 35; i++) {
        for (int j = 3; j > 0; --j) {
            float escala = j * 0.05f;

            // Calcula dimensiones escaladas
            int ancho = nube.width() * escala;
            int alto  = nube.height() * escala;

            QPixmap nubeEscalada = nube.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            // Posición aleatoria dentro de la escena
            int x = i * 250 + QRandomGenerator::global()->bounded(-60, 100);
            int y = QRandomGenerator::global()->bounded(0, 80);

            if (x + nubeEscalada.width() <= escena->width()) {
                contNubes += 1;

                // Crea y posiciona la nube en la escena
                QGraphicsPixmapItem *_nube = new QGraphicsPixmapItem(nubeEscalada);
                _nube->setPos(x, y);
                escena->addItem(_nube);
                listaNubes.push_back(_nube);
            }
        }
    }

    // Crea el temporizador que moverá las nubes de forma periódica
    timerNubes = new QTimer(this);
    connect(timerNubes, &QTimer::timeout, this, &Nivel::moverNubes);
    timerNubes->start(45);
}


// Mueve todas las nubes hacia la izquierda y las reposiciona si salen de la escena
void Nivel::moverNubes()
{
    const int velocidadNube = 2; // Velocidad con la que se desplazan las nubes

    for (auto *nubeItem : listaNubes) {
        if (!nubeItem) continue;  // Validación defensiva en caso de puntero nulo

        // Desplaza la nube hacia la izquierda en el eje X
        nubeItem->setPos(nubeItem->x() - velocidadNube, nubeItem->y());

        // Si la nube ha salido completamente por la izquierda de la escena
        if (nubeItem->x() + nubeItem->pixmap().width() < 0) {
            // La reposicionamos al borde derecho con una nueva altura aleatoria
            int nuevaX = escena->width();  // La pone justo al final del ancho visible
            int nuevaY = QRandomGenerator::global()->bounded(0, 100);  // Altura aleatoria
            nubeItem->setPos(nuevaX, nuevaY);
        }
    }
}


// Muestra una imagen de "Game Over" sobre la vista del nivel actual
void Nivel::mostrarGameOver()
{
    // No crear el overlay si ya fue mostrado antes
    if (overlayGameOver) return;

    // Validación crítica: vista no puede ser nula
    if (!vista) {
        throw std::runtime_error("Nivel: No se puede mostrar 'Game Over' porque la vista es nula.");
    }

    // Crea un QLabel superpuesto a la vista del juego
    overlayGameOver = new QLabel(vista);
    overlayGameOver->setPixmap(
        QPixmap(":/images/gameOver.png")
            .scaled(vista->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    overlayGameOver->setAlignment(Qt::AlignCenter);
    overlayGameOver->setAttribute(Qt::WA_TransparentForMouseEvents);  // Ignora clics
    overlayGameOver->setAttribute(Qt::WA_DeleteOnClose);              // Se eliminará al cerrarse
    overlayGameOver->show();
}
