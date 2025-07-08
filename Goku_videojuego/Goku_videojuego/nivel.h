#ifndef NIVEL_H
#define NIVEL_H

#include <QWidget>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <vector>
#include "goku.h"
#include "obstaculo.h"
#include "vida.h"
#include "progreso.h"

/**
 * Clase base abstracta para los niveles del juego.
 * Proporciona la estructura común y métodos virtuales puros que deben implementar
 * los niveles concretos (Nivel1, Nivel2, etc.).
 */
class Nivel : public QWidget
{
    Q_OBJECT

public:
    explicit Nivel(QGraphicsScene* escena, QGraphicsView* view, QWidget* parent, int numero);
    virtual ~Nivel() override;

    // Métodos virtuales puros que deben implementar las subclases
    virtual void iniciarNivel() = 0;
    virtual void cargarFondoNivel(const QString& ruta) = 0;
    virtual void agregarGoku() = 0;
    virtual void actualizarNivel() = 0;
    virtual void gameOver() = 0;

    // Métodos comunes
    int getMargenHUD() const;
    virtual Goku* getGoku() const = 0;
    virtual bool haTerminado() const = 0;

    // Gestión de recursos
    virtual void limpiarEscena();

    // Miembro estático
    static int contNubes; // Contador global de nubes activas

signals:
    void gokuMurio();     // Señal emitida cuando el jugador pierde
    void nivelCompletado(); // Señal emitida al completar el nivel

protected:
    // Elementos gráficos
    QGraphicsView* vista;
    QGraphicsScene* escena;

    // Componentes del juego
    Goku* goku = nullptr;
    Vida* barraVida = nullptr;
    Progreso* barraProgreso = nullptr;

    // Temporizadores
    QTimer* timerNivel = nullptr;
    QTimer* timerNubes = nullptr;

    // Elementos visuales
    std::vector<QGraphicsPixmapItem*> listaFondos;
    std::vector<QGraphicsPixmapItem*> listaNubes;
    std::vector<obstaculo*> listaObstaculos;
    QLabel* overlayGameOver = nullptr;

    // Recursos compartidos
    QPixmap nube;
    QPixmap background;

    // Configuración
    int numeroNivel;
    const int margenHUD = 70;

    // Métodos protegidos
    void generarNubes();
    void mostrarGameOver();
    void moverNubes();

private:
    // Bloqueamos copia y asignación
    Nivel(const Nivel&) = delete;
    Nivel& operator=(const Nivel&) = delete;
};

#endif // NIVEL_H
