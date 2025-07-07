#include "pocion.h"
#include <QRandomGenerator>
#include <QGraphicsScene>

// Constante fija para limitar el ancho horizontal
const int LimiteAnchoX = 1036;

// Constructor de la clase Pocion
Pocion::Pocion(const QVector<QPixmap>& framesOriginales, int fila, int columna, int columnas, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent),                // Establece el elemento gráfico como hijo del item padre
    indiceFrame(0),                               // Frame inicial para la animación
    fila(fila), columna(columna),                 // Posición lógica en la grilla - matriz (para organización de pociones)
    columnasTotales(columnas)                     // Total de columnas en la grilla - matriz                    // Ancho de la vista del nivel
{
    //Escalar y almacenar los frames de animación
    for (const QPixmap& original : framesOriginales) {
        // Escala cada frame al 80% de su tamaño original, manteniendo la relación de aspecto
        frames.append(original.scaled(original.width() * 0.8,
                                      original.height() * 0.8,
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation));
    }

    setPixmap(frames[0]);   // Establece el primer frame visible como imagen inicial
    setZValue(1);           // Define la prioridad de dibujo (por encima del fondo)

    // Ancho del sprite escalado
    int anchoSprite = frames[0].width();

    // Distruibuir las pociones en columnas con cierta aleatoriedad
    int espacioX = LimiteAnchoX / columnasTotales;  // Espacio entre columnas
    int baseX = columna * espacioX;               // Centro de la columna actual
    int minX = std::max(0, baseX - 15);           // Límite izquierdo con margen
    int maxX = std::min(LimiteAnchoX - anchoSprite, baseX + 15); // Límite derecho con margen

    // Posición X aleatoria dentro de los márgenes de su columna
    int x = QRandomGenerator::global()->bounded(minX, maxX + 1);

    // Se ubican en diferentes filas con variación vertical aleatoria
    int espacioY = 300;  // Separación vertical entre filas
    int offsetY = QRandomGenerator::global()->bounded(-300, 300);  // Aleatoriedad
    int y = -400 + fila * espacioY + offsetY;  // Posición final en Y (comienza fuera de pantalla)

    setPos(x, y); // Establece la posición inicial

    timer = new QTimer(this);  // Temporizador con this como padre
    connect(timer, &QTimer::timeout, this, &Pocion::moverYAnimar); // Conecta la señal al slot
    timer->start(100);  // Llama moverYAnimar() cada 100 ms
}

// Destructor: detiene el timer si aún está activo
Pocion::~Pocion()
{
    qDebug() << "Destructor de pocion llamado";
    if (timer)
        timer->stop();
}

// Mvto y animación continua
void Pocion::moverYAnimar()
{
    // Mueve la poción 3 píxeles hacia abajo en cada llamada
    moveBy(0, 3);

    // Cambia el frame para crear efecto de rotación o animación
    indiceFrame = (indiceFrame + 1) % frames.size();
    setPixmap(frames[indiceFrame]);

    // Si la poción sale de la pantalla por abajo, la reposiciona arriba
    if (scene() && y() > scene()->height()) {
        // Recalcula la posición horizontal como al inicio
        int anchoSprite = frames[0].width();
        int espacioX = LimiteAnchoX / columnasTotales;
        int baseX = columna * espacioX;
        int minX = std::max(0, baseX - 15);
        int maxX = std::min(LimiteAnchoX - anchoSprite, baseX + 15);
        int x = QRandomGenerator::global()->bounded(minX, maxX + 1);

        // Recalcula posición vertical con menos variación (más controlado)
        int espacioY = 100;
        int offsetY = QRandomGenerator::global()->bounded(-200, 200);
        int yNuevo = -400 + fila * espacioY + offsetY;

        // Reaparece en la parte superior de la escena
        setPos(x, yNuevo);
    }
}
