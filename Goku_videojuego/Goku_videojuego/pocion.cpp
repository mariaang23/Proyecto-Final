#include "pocion.h"
#include <QRandomGenerator>
#include <QGraphicsScene>
#include <QDebug>
#include <stdexcept>  // Para lanzar excepciones

// Inicialización del contador
int Pocion::contador = 0;

// Constante que define el límite horizontal máximo del escenario
const int LimiteAnchoX = 1036;

/**
@brief Constructor de la clase Pocion.

Crea y posiciona una poción animada en una grilla específica del escenario. La animación consta de múltiples frames escalados al 80% del tamaño original.

@param framesOriginales Vector con los frames originales para la animación de la poción.
@param fila Fila lógica donde se posiciona la poción dentro de la grilla.
@param columna Columna lógica dentro de la grilla donde aparecerá la poción.
@param columnas Cantidad total de columnas en la grilla del escenario.
@param parent Objeto padre gráfico (generalmente la escena o ítem gráfico contenedor).

@details

Verifica que los parámetros sean válidos, lanzando excepciones si no se cumplen condiciones básicas (frames disponibles y columnas positivas).

Posiciona la poción horizontalmente de forma aleatoria dentro de su columna asignada, y verticalmente según la fila y un desplazamiento adicional aleatorio.

Inicia un temporizador interno que controla la animación y desplazamiento vertical de la poción cada 100 ms.

Aplica un valor Z (profundidad gráfica) que asegura que la poción esté visualmente sobre otros elementos del fondo.
*/
Pocion::Pocion(const QVector<QPixmap>& framesOriginales, int fila, int columna, int columnas, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent),   // Establece el padre gráfico
    indiceFrame(0),                  // Comienza en el primer frame
    fila(fila),                      // Fila lógica en la grilla
    columna(columna),                // Columna lógica
    columnasTotales(columnas)        // Total de columnas disponibles
{
    // Validación de entrada: debe haber al menos un frame
    if (framesOriginales.isEmpty()) {
        throw std::invalid_argument("Pocion: los frames no pueden estar vacíos.");
    }

    // Validación de grilla: columnas debe ser positivo
    if (columnasTotales <= 0) {
        throw std::invalid_argument("Pocion: el número de columnas debe ser mayor que cero.");
    }

    // Escala cada imagen de la animación al 80% de su tamaño original
    for (const QPixmap& original : framesOriginales) {
        frames.append(original.scaled(original.width() * 0.8,
                                      original.height() * 0.8,
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation));
    }

    setPixmap(frames[0]); // Establece el primer frame como imagen inicial
    setZValue(1);         // Aparece por encima de otros elementos del fondo

    // Cálculo de posición horizontal aleatoria dentro de su columna
    int anchoSprite = frames[0].width();
    int espacioX = LimiteAnchoX / columnasTotales;
    int baseX = columna * espacioX;
    int minX = std::max(0, baseX - 15);
    int maxX = std::min(LimiteAnchoX - anchoSprite, baseX + 15);

    // Validación defensiva del rango
    if (minX >= maxX) {
        throw std::runtime_error("Pocion: los márgenes de posicionamiento horizontal son inválidos.");
    }

    int x = QRandomGenerator::global()->bounded(minX, maxX + 1);  // Posición X aleatoria

    // Posición Y determinada por su fila, con variación aleatoria
    int espacioY = 300;
    int offsetY = QRandomGenerator::global()->bounded(-300, 300);
    int y = -400 + fila * espacioY + offsetY;       // Inicia fuera del área visible

    setPos(x, y);                                   // Posiciona la poción en la escena

    // Crea el temporizador de animación
    timer = new QTimer(this);                       // Qt se encargará de destruirlo
    connect(timer, &QTimer::timeout, this, &Pocion::moverYAnimar);
    timer->start(100);    // Llama moverYAnimar() cada 100 ms

    //contador+=1;
    //qDebug() << "Pociones creadas  "<<contador;
}

/**
@brief Destructor de la clase Pocion.

Libera los recursos utilizados por la poción, principalmente el temporizador encargado de la animación y el movimiento vertical del objeto gráfico.

@details

Detiene y desconecta de forma segura el temporizador interno si está activo.

Libera la memoria utilizada por dicho temporizador, garantizando la correcta gestión de recursos.
*/
Pocion::~Pocion(){
    //qDebug() << "Destructor de Pocion llamado";
    if (timer) {
        disconnect(timer, nullptr, this, nullptr);
        timer->stop();
        delete timer;
        timer = nullptr;
    }

    //contador-=1;
    //qDebug() << "Pociones eliminadas  "<<contador;
}

/**
@brief Desplaza verticalmente y anima la poción en la escena.

Este método se ejecuta periódicamente mediante un temporizador interno para producir el movimiento descendente y la animación cíclica del sprite. Además, reposiciona la poción cuando esta desaparece por la parte inferior del área visible.

@details

Mueve la poción hacia abajo 3 píxeles en cada llamada.

Cambia cíclicamente el frame de animación del sprite.

Si la poción sale del límite inferior de la pantalla, reaparece en una posición horizontal aleatoria dentro de su columna asignada y con una nueva posición vertical superior, proporcionando así un comportamiento continuo.
*/
void Pocion::moverYAnimar()
{
    //qDebug() << "timer animar en pocion llamado  "<<contador++;

    moveBy(0, 3);  // Desplaza hacia abajo.

    // Animación cíclica.
    indiceFrame = (indiceFrame + 1) % frames.size();
    setPixmap(frames[indiceFrame]);

    // Reaparecer si sale de la pantalla.
    if (y() > scene()->height()) {
        int anchoSprite = frames[0].width();
        int espacioX = LimiteAnchoX / columnasTotales;
        int baseX = columna * espacioX;
        int minX = std::max(0, baseX - 15);
        int maxX = std::min(LimiteAnchoX - anchoSprite, baseX + 15);

        if (minX < maxX) {  // Validación adicional.
            int x = QRandomGenerator::global()->bounded(minX, maxX + 1);
            int yNuevo = -400 + fila * 100 + QRandomGenerator::global()->bounded(-200, 200);
            setPos(x, yNuevo);
        }
    }
}

/**
@brief Detiene la animación y el movimiento de la poción.

Este método detiene el temporizador interno que controla el desplazamiento y la animación de la poción, congelando su estado visual y posicional.
*/
void Pocion::detener()
{
    if (timer) {
        timer->stop();  // Detiene el movimiento y animación
    }
}
