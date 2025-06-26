#include "obstaculo.h"
#include "qgraphicsitem.h"
#include "qtimer.h"
#include <QRandomGenerator>

// Constructor del obstáculo
obstaculo::obstaculo(QGraphicsScene *scene, Tipo tipo, int velocidad, QObject *parent)
    : QObject(parent), velocidad(velocidad), tipo(tipo), scene(scene)
{
    // Crear el objeto gráfico (sprite del obstáculo)
    sprite = new QGraphicsPixmapItem();
    scene->addItem(sprite);  // Agregar el sprite a la escena para hacerlo visible

    cargarImagenes();  // Cargar la imagen correspondiente según el tipo

    sprite->setData(0, "obstaculo");
    // Crear temporizador para mover el obstáculo y conectar su señal al slot mover()
    timerMovimiento = new QTimer(this);
    connect(timerMovimiento, &QTimer::timeout, this, &obstaculo::mover);

    // Si el tipo es Ave, necesita un temporizador adicional para animar los frames
    if (tipo == Ave) {
        timerAnimacion = new QTimer(this);
        connect(timerAnimacion, &QTimer::timeout, this, &obstaculo::actualizar);
    }
}

// Inicializa la posición del obstáculo y activa los timers
void obstaculo::iniciar(int x, int y)
{
    // Si no se define x, se genera una posición aleatoria dentro del ancho de la escena
    /*if (x == -1)
        x = QRandomGenerator::global()->bounded(scene->width());*/

    // Si no se define y, se ajusta según el tipo: aves en el aire, otros en el suelo
    /*
    if (y == -1) {
        switch (tipo) {
        case Ave:       // pájaro: altura libre (vuela)
            y = QRandomGenerator::global()->bounded(20, 200);
            break;

        case Montania:  // montaña: SIEMPRE pegada al suelo
            y = 600;
            break;

        case Roca:      // roca: puede o no tocar el suelo
            y = QRandomGenerator::global()->bounded(350,500);
            break;
        }
    }*/

    sprite->setPos(x, y);  // Posicionar el sprite

    timerMovimiento->start(60);  // Inicia el movimiento del obstáculo

    if (tipo == Ave)
        timerAnimacion->start(100);  // Inicia la animación de frames si es un ave
}

// Carga las imágenes del obstáculo según su tipo
void obstaculo::cargarImagenes()
{
    if (tipo == Ave) {
        fotogWidth = 90;
        fotogHeight = 180;

        QPixmap spriteSheet(":/images/pajaro.png");  // Hoja de sprites

        if (spriteSheet.isNull()) {
            qWarning() << "No se pudo cargar pajaro.png";
            return;
        }

        frames.clear();  // Limpiar cualquier frame anterior

        // Extraer 4 frames de la hoja de sprites (uno por ala)
        for (int i = 0; i < 4; ++i) {
            frames.append(spriteSheet.copy(i * fotogWidth, 0, fotogWidth, fotogHeight));
        }

        if (!frames.isEmpty()) {
            sprite->setPixmap(frames[0]);  // Mostrar el primer frame
            frameActual = 0;
        }
    }
    else if (tipo == Montania) {
        QPixmap montanaPixmap(":/images/montania.png");

        // Altura aleatoria para hacer la montaña más variada
        int alturaMontana =300 + QRandomGenerator::global()->bounded(100, 200);
        QPixmap montanaEscalada = montanaPixmap.scaledToHeight(alturaMontana, Qt::SmoothTransformation);

        sprite->setPixmap(montanaEscalada);  // Asignar imagen escalada al sprite
    }
    else if (tipo == Roca) {
        QPixmap rocaPixmap(":/images/roca.png");

        // Altura aleatoria para hacer la roca más variada
        int alturaRoca = QRandomGenerator::global()->bounded(80, 200);
        QPixmap rocaEscalada = rocaPixmap.scaledToHeight(alturaRoca, Qt::SmoothTransformation);

        sprite->setPixmap(rocaEscalada);  // Asignar imagen escalada al sprite
    }
}

// Destructor del obstáculo
obstaculo::~obstaculo()
{
    delete sprite;
    sprite = nullptr;

    // Detener y liberar temporizador de movimiento
    if (timerMovimiento) {
        timerMovimiento->stop();
        delete timerMovimiento;
        timerMovimiento = nullptr;
    }

    // Detener y liberar temporizador de animación si es ave
    if (timerAnimacion) {
        timerAnimacion->stop();
        delete timerAnimacion;
        timerAnimacion = nullptr;
    }
}

// Función que mueve el obstáculo hacia la izquierda
void obstaculo::mover()
{
    sprite->moveBy(-velocidad, 0);  // Desplazar el sprite a la izquierda

    // Si ya salió de la pantalla, detener timers y ocultar el sprite
    if (sprite->x() + sprite->pixmap().width() < 0) {
        timerMovimiento->stop();

        if (tipo == Ave)
            timerAnimacion->stop();

        sprite->hide();
    }
}

// Función que cambia el frame del ave para animarla
void obstaculo::actualizar()
{
    if (frames.isEmpty()) return;

    frameActual = (frameActual + 1) % frames.size();  // Avanzar al siguiente frame
    sprite->setPixmap(frames[frameActual]);           // Actualizar el sprite con el nuevo frame
}

// Devuelve la altura actual del obstáculo
int obstaculo::getAltura() const
{
    return sprite->pixmap().height();
}
