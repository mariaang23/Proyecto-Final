#ifndef NIVEL_H
#define NIVEL_H

#include <QWidget>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <vector>

#include "camaralogica.h"
#include "goku.h"
#include "obstaculo.h"
#include "vida.h"
#include "progreso.h"

/**
 * Clase base abstracta para representar un nivel del juego.
 * Define la estructura común: escena, vista, HUD, personaje principal, nubes, lógica de actualización, etc.
 * Subclases como Nivel1, Nivel2... deben implementar los métodos virtuales obligatorios.
 */
class Nivel : public QWidget
{
    Q_OBJECT

protected:
    int numeroNivel; // Identificación del nivel actual
    const int margenHUD = 70;  // Espacio reservado arriba para HUD (barra vida, progreso, etc.)

    // Recursos gráficos compartidos
    QPixmap nube;
    QPixmap background;

    // Escena principal y vista donde se dibuja
    QGraphicsView* vista;
    QGraphicsScene* escena;

    // Componentes lógicos del nivel
    camaraLogica* camara = nullptr;     // Cámara virtual (solo nivel1)
    Goku* goku = nullptr;               // Personaje principal
    Vida* barraVida = nullptr;          // HUD: barra de vida
    Progreso* barraProgreso = nullptr;  // HUD: barra de progreso

    // Temporizadores de lógica y animación
    QTimer* timerNivel = nullptr;  // Ejecuta la lógica del juego periódicamente
    QTimer* timerNubes = nullptr;  // Mueve las nubes del fondo

    // Elementos gráficos del fondo
    std::vector<QGraphicsPixmapItem*> listaFondos; // Fondos del nivel
    std::vector<QGraphicsPixmapItem*> listaNubes;  // Nubes animadas

    std::vector<obstaculo*> listaObstaculos;  // Obstáculos presentes en el nivel

    QLabel* overlayGameOver = nullptr;        // Overlay de fin de juego

    void generarNubes();                      // Generación de elementos animados del fondo

    void mostrarGameOver();                   // Muestra el mensaje visual de "Game Over"

public:
    static int contNubes;                     // Contador global de nubes activas

    // Constructor del nivel base
    explicit Nivel(QGraphicsScene* escena, QGraphicsView* view, QWidget* parent, int numero);

    virtual ~Nivel();                 // Destructor virtual (libera recursos comunes)

    int getMargenHUD() const;         // Acceso a margen superior del HUD

    // Métodos virtuales obligatorios que deben implementar las subclases del nivel
    virtual void iniciarNivel() = 0;
    virtual void cargarFondoNivel(const QString& ruta) = 0;
    virtual void agregarGoku() = 0;
    virtual void actualizarNivel() = 0;

    Goku* getGoku() const { return goku; }   // Getter para el personaje principal

protected slots:
    void moverNubes();                       // Movimiento automático de las nubes

signals:
    void gokuMurio();                         // Señal emitida cuando Goku muere
};

#endif // NIVEL_H
