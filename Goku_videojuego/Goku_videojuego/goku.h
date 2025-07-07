#ifndef GOKU_H
#define GOKU_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QTimer>
#include <QVector>
#include "vida.h"

/**
 * Clase base abstracta que representa a Goku en el videojuego.
 * Controla movimiento, animación, y lógica de vida.
 * Subclases deben implementar comportamiento específico según el nivel.
 */
class Goku : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Goku(QGraphicsScene *scene, int velocidad, int fotogWidth, int fotogHeight, QObject *parent = nullptr);
    virtual ~Goku();

    void setBarraVida(Vida* barra);          // Asocia la barra de vida externa
    void recibirDanio(int cantidad);         // Aplica daño visualmente
    int obtenerVida() const;                 // Getter para consultar vida actual

    virtual void iniciar(int x, int y) = 0;  // Posiciona e inicia lógica visual
    virtual void detener() = 0;              // Detiene cualquier animación o movimiento

protected:
    virtual void keyPressEvent(QKeyEvent *event) override = 0;     // Interacción con teclado
    virtual void keyReleaseEvent(QKeyEvent *event) override = 0;
    virtual void cargarImagen() = 0;                                // Carga imágenes desde recursos
    virtual void mover() = 0;                                       // Actualiza posición

    void actualizarFrame(int indice);       // Cambia frame de animación

    QGraphicsScene *scene;                  // Escena donde se inserta Goku
    QTimer *timerMovimiento;                // Timer para movimiento continuo
    QTimer *timerDanio;                     // Timer para recibir daño y gestionar animación

    QVector<QPixmap> frames;                // Frames de animación
    int frameActual;                        // Índice de frame actual

    int velocidad;                          // Velocidad de movimiento
    int fotogWidth, fotogHeight;            // Tamaño de sprites individuales
    bool mirandoDerecha;                    // Orientación visual actual

    Vida* vidaHUD;                          // Referencia a HUD de vida
    bool puedeRecibirDanio;                 // Lógica para inmunidad temporal (aún sin usar aquí)
};

#endif // GOKU_H
