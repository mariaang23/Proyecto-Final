#include "nivel.h"
#include <QRandomGenerator>
#include <QGraphicsPixmapItem>
#include <stdexcept>  // Para lanzar excepciones estándar
#include <QDebug>

// Inicialización del contador de nubes compartido entre niveles
int Nivel::contNubes = 0;
// Inicialización del contador
int Nivel::contador = 0;

/**
@brief Constructor de la clase base `Nivel`, utilizada para representar un nivel del juego.

Inicializa los recursos comunes a todos los niveles, como la escena, vista y temporizador principal del juego.

- Valida que `escena` y `view` no sean nulos.
- Inicializa un temporizador (`timerNivel`) que llama al método virtual `actualizarNivel()` cada 20 ms, permitiendo la ejecución periódica de lógica personalizada en subclases (`Nivel1`, `Nivel2`).
- Almacena el número del nivel (`numeroNivel`) para identificar el nivel cargado.

@param escena Puntero a la escena gráfica donde se dibujan los objetos del juego.
@param view Puntero a la vista (`QGraphicsView`) que muestra la escena.
@param parent Widget padre opcional.
@param numero Número que identifica el nivel (1, 2, etc.).

@throw std::invalid_argument Si `escena` o `view` son nulos.
*/
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

/**
@brief Destructor de la clase base `Nivel`, responsable de liberar recursos comunes a todos los niveles.

Este método garantiza una limpieza segura y ordenada al destruir un nivel, ejecutando las siguientes acciones:

1. Detiene y desconecta todos los temporizadores (`timerNivel`, `timerNubes`).
2. Elimina objetos gráficos directamente relacionados, como `goku`, `nubes`, y fondos (`listaNubes`, `listaFondos`).
3. Libera la barra de vida y la barra de progreso si existen, y las desvincula de sus vistas correspondientes.
4. Libera la memoria de los temporizadores y de la superposición de “Game Over” si está activa.

Este destructor debe ser invocado automáticamente al eliminar un objeto `Nivel`, o de forma indirecta al eliminar `Nivel1` o `Nivel2`.

@see Nivel::limpiarEscena
*/
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

/**
@brief Limpia visualmente la escena del nivel eliminando personajes, fondos y efectos gráficos.

Este método borra todos los elementos gráficos agregados por el nivel a la escena, sin destruir el objeto `Nivel` en sí.
Está diseñado para usarse durante el transcurso del juego cuando se requiere un reinicio visual sin invocar el destructor.

- Elimina a `goku`, las nubes (`listaNubes`), y los fondos (`listaFondos`) de la escena.
- Oculta y programa la eliminación de la superposición de "Game Over" si está activa.

@note Este método es útil para reiniciar o recargar un nivel mientras el objeto `Nivel` sigue existiendo.

@see Nivel::~Nivel
*/
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

/**
@brief Devuelve el margen vertical superior reservado para la interfaz HUD del nivel.

Este margen se utiliza para evitar que los elementos del juego (como personajes o enemigos) se superpongan
con la barra de vida, barra de progreso u otros indicadores visuales del HUD.

@return Entero que representa el espacio en píxeles reservado en la parte superior de la escena.
*/
int Nivel::getMargenHUD() const {
    return margenHUD;
}

/**
@brief Genera y anima múltiples nubes en la escena del nivel como fondo dinámico.

Este método carga una imagen de nube desde los recursos y genera instancias escaladas horizontalmente distribuidas
a lo largo de la escena. Las nubes se colocan en distintas posiciones y tamaños para crear un efecto visual de profundidad.

- Se generan 35 columnas de nubes, cada una con 3 nubes de distinta escala.
- Las nubes se agregan a la escena y se almacenan en `listaNubes`.
- Se inicia un temporizador (`timerNubes`) que las moverá lateralmente mediante `moverNubes()`.

@throw std::runtime_error Si no se puede cargar la imagen de la nube.

@see Nivel::moverNubes
*/
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

/**
@brief Mueve todas las nubes del fondo hacia la izquierda y las reposiciona cuando salen de la escena.

Este método es llamado periódicamente por el temporizador `timerNubes` y da la ilusión de movimiento continuo en el cielo del nivel.

- Desplaza cada nube `velocidadNube` píxeles hacia la izquierda.
- Si una nube sale completamente del borde izquierdo, se reposiciona al borde derecho con una nueva altura aleatoria.

@note Este efecto mejora la ambientación visual del juego sin afectar la jugabilidad.

@see Nivel::generarNubes
*/
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

/**
@brief Muestra una superposición de "Game Over" sobre la vista actual del nivel.

Este método crea un `QLabel` centrado sobre la vista (`vista`) con la imagen `gameOver.png` escalada al tamaño completo de la ventana.

- La imagen se alinea al centro y se muestra de forma no interactiva.
- Si ya existe una superposición activa (`overlayGameOver`), no se crea otra.
- La superposición se destruye automáticamente al cerrarse (`WA_DeleteOnClose`).

@note Si la imagen no se encuentra, se registra un error crítico en la consola.

@see Nivel::limpiarEscena
*/
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
