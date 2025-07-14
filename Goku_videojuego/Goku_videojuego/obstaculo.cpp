#include "obstaculo.h"
#include "qgraphicsitem.h"
#include "qtimer.h"
#include <QRandomGenerator>

// Inicialización del contador
int obstaculo::contador = 0;
int obstaculo::contObsta=0;

/**
@brief Constructor de la clase obstaculo.

Inicializa un obstáculo gráfico con tipo específico, velocidad y gestión automática del movimiento y animación.
El obstáculo es representado mediante un sprite gráfico agregado directamente a la escena suministrada.

@param scene Puntero a la escena gráfica donde se añadirá el obstáculo.
@param tipo Enumeración que indica el tipo específico del obstáculo (Ave, Montaña, Roca).
@param velocidad Valor entero que determina la velocidad de desplazamiento horizontal del obstáculo.
@param parent Objeto padre que se encargará de la gestión de memoria (generalmente la escena o ventana principal).

@details

Crea el sprite gráfico del obstáculo y lo añade a la escena.

Carga automáticamente las imágenes adecuadas según el tipo del obstáculo.

Configura temporizadores internos para manejar el desplazamiento horizontal y la animación en caso del tipo Ave.

Incrementa un contador interno para llevar registro de los obstáculos existentes.
*/
obstaculo::obstaculo(QGraphicsScene *scene, Tipo tipo, int velocidad, QObject *parent)
    : QObject(parent),
    sprite(nullptr),
    scene(scene),
    frames(),
    timerAnimacion(nullptr),
    timerMovimiento(nullptr),
    frameActual(0),
    velocidad(velocidad),
    coordX(0),
    coordY(0),
    fotogWidth(0),
    fotogHeight(0),
    tipo(tipo)
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

    contObsta +=1;

    //qDebug() << "obstaculos existentes" << contObsta;
}

/**
@brief Inicializa la posición y comienza el movimiento y animación del obstáculo.

Establece la posición inicial del sprite del obstáculo en la escena e inicia los temporizadores necesarios para controlar su desplazamiento horizontal y, si es un ave, la animación de sus frames.

@param x Coordenada horizontal inicial del obstáculo.
@param y Coordenada vertical inicial del obstáculo.

@details

Posiciona el sprite en las coordenadas dadas.

Activa un temporizador que mueve horizontalmente el obstáculo cada 60 ms.

Si el obstáculo es del tipo Ave, inicia otro temporizador adicional que cambia la imagen del sprite cada 100 ms para simular el movimiento de alas.
*/
void obstaculo::iniciar(int x, int y)
{
    sprite->setPos(x, y);  // Posicionar el sprite

    timerMovimiento->start(60);  // Inicia el movimiento del obstáculo

    if (tipo == Ave)
        timerAnimacion->start(100);  // Inicia la animación de frames si es un ave
}

/**
@brief Carga las imágenes necesarias según el tipo específico del obstáculo.

Este método inicializa los recursos gráficos adecuados dependiendo del tipo del obstáculo (Ave, Montaña o Roca). En caso de ser un Ave, se cargan múltiples frames para la animación; para Montaña y Roca se cargan imágenes individuales con altura variable para dar variedad visual.

@details

Para el tipo Ave:

Carga una hoja de sprites y extrae cuatro frames individuales para simular el vuelo mediante animación.

Verifica que la imagen haya cargado correctamente antes de usarla.

Para los tipos Montaña y Roca:

Carga una imagen única del tipo respectivo y escala su altura de manera aleatoria para generar obstáculos visualmente diversos.

@note Es fundamental que las imágenes estén correctamente ubicadas en la ruta de recursos para evitar errores.

*/
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
        int alturaMontana = 200 + QRandomGenerator::global()->bounded(100, 200);
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

/**
@brief Destructor de la clase obstaculo.

Gestiona la liberación segura de recursos asociados al obstáculo, incluyendo gráficos (sprite) y temporizadores. Garantiza que todos los objetos dinámicos sean destruidos y que se detengan correctamente las animaciones y movimientos activos.

@details

Detiene y desconecta los temporizadores activos (timerMovimiento y timerAnimacion).

Elimina el sprite gráfico del obstáculo y lo retira de la escena para evitar fugas de memoria.

Reduce el contador global de obstáculos activos para mantener la coherencia interna del sistema.
*/
obstaculo::~obstaculo()
{
    //qDebug() << "Destructor de obs llamado";
    delete sprite;
    sprite = nullptr;

    // 1. Detener y desconectar timers
    if (timerMovimiento) {
        disconnect(timerMovimiento, nullptr, this, nullptr);
        timerMovimiento->stop();
        delete timerMovimiento;
        timerMovimiento = nullptr;
    }

    if (timerAnimacion) {
        disconnect(timerAnimacion, nullptr, this, nullptr);
        timerAnimacion->stop();
        delete timerAnimacion;
        timerAnimacion = nullptr;
    }

    // 2. Remover sprite de la escena si ambos existen
    if (scene && sprite) {
        scene->removeItem(sprite);
        delete sprite;
        sprite = nullptr;
    }

    contObsta -= 1;
    //qDebug() << "obstaculos restantes" << contObsta;
}

/**
@brief Mueve horizontalmente el obstáculo hacia la izquierda.

Desplaza el sprite del obstáculo a la izquierda en función de la velocidad configurada. Este método es llamado periódicamente por un temporizador.

@details

Si el obstáculo alcanza el borde izquierdo de la pantalla (desaparece del área visible), se detienen los temporizadores de movimiento y animación (si aplica), y el sprite se oculta para optimizar el rendimiento.

@note El desplazamiento negativo indica movimiento hacia la izquierda.
*/
void obstaculo::mover()
{

    //qDebug() << "timer mvto en obstaculo llamado  "<<contador++;
    sprite->moveBy(-velocidad, 0);  // Desplazar el sprite a la izquierda

    // Si ya salió de la pantalla, detener timers y ocultar el sprite
    if (sprite->x() + sprite->pixmap().width() < 0) {
        timerMovimiento->stop();

        if (tipo == Ave)
            timerAnimacion->stop();

        sprite->hide();
    }
}

/**
@brief Actualiza el frame del sprite para animar el obstáculo tipo Ave.

Este método se llama periódicamente mediante un temporizador para cambiar cíclicamente entre distintos frames almacenados, simulando el movimiento o animación visual del obstáculo Ave.

@details

Verifica primero que haya frames disponibles para la animación.

Incrementa el índice del frame actual de forma cíclica.

Actualiza el sprite con el nuevo frame correspondiente al índice actual.
*/
void obstaculo::actualizar()
{

    //qDebug() << "timer animacion ave obstaculo llamado   "<<contador++;
    if (frames.isEmpty()) return;

    frameActual = (frameActual + 1) % frames.size();  // Avanzar al siguiente frame
    sprite->setPixmap(frames[frameActual]);           // Actualizar el sprite con el nuevo frame
}

/**
@brief Devuelve la altura actual del sprite del obstáculo.

Permite conocer dinámicamente la altura del obstáculo gráfico, útil para detección de colisiones o posicionamiento relativo dentro del juego.

@return Altura del sprite en píxeles.
*/
int obstaculo::getAltura() const
{
    return sprite->pixmap().height();
}
