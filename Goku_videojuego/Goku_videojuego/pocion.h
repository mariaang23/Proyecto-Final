#ifndef POCION_H
#define POCION_H

#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QVector>
#include <QObject>

/**
 * Clase gráfica animada que representa una poción en la escena del juego.
 * Hereda de QObject para usar señales/slots y de QGraphicsPixmapItem para renderizado.
 */
class Pocion : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    static int contador;

    Pocion(const QVector<QPixmap>& framesOriginales, int fila, int columna, int columnas, QGraphicsItem* parent = nullptr);
    ~Pocion();
    void detener();

private slots:
    void moverYAnimar();         // Slot que mueve y cambia el frame de la poción.

private:
    QVector<QPixmap> frames;     //Frames animados escalados.
    int indiceFrame;             // Índice actual del frame mostrado.
    QTimer* timer;               // Temporizador que controla la animación.

    int fila;                    // Posición lógica (grilla) en Y.
    int columna;                 // Posición lógica (grilla) en X.
    int columnasTotales;         // Número total de columnas disponibles.
};

#endif // POCION_H
