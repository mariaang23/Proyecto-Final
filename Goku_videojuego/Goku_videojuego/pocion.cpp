#include "pocion.h"
#include <QRandomGenerator>
#include <QGraphicsScene>
#include <QDebug>
#include <stdexcept>  // Para lanzar excepciones

// Constante que define el límite horizontal máximo del escenario
const int LimiteAnchoX = 1036;

// Constructor de la clase Pocion
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
    timer->start(100);                              // Llama moverYAnimar() cada 100 ms
}

// Destructor: detiene el temporizador antes de que se destruya
Pocion::~Pocion(){
    qDebug() << "Destructor de Pocion llamado";
    if (timer) {
        disconnect(timer, nullptr, this, nullptr);
        timer->stop();
        delete timer;
        timer = nullptr;
    }
}

// Slot llamado periódicamente para mover y animar la poción
void Pocion::moverYAnimar() {
    if (!scene() || frames.isEmpty()) return;  // Verificación crítica.

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

// Método público para detener la animación externamente
void Pocion::detener()
{
    if (timer) {
        timer->stop();  // Detiene el movimiento y animación
    }
}
