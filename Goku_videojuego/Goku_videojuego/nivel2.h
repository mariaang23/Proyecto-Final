#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include "pocion.h"
#include "robot.h"
#include "explosion.h"
#include <QVector>

class Nivel2 : public Nivel
{
    Q_OBJECT

public:
    explicit Nivel2(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent = nullptr);
    ~Nivel2() override;

    // Métodos sobrescritos de la clase base
    void iniciarNivel() override;
    void cargarFondoNivel(const QString &ruta) override;
    void agregarGoku() override;
    void actualizarNivel() override;
    void gameOver() override;
    bool haTerminado() const override;
    Goku* getGoku() const override;

    // Métodos específicos del nivel 2
    void pocionRecolectada();
    void agregarExplosion(Explosion* e);

private slots:
    void agregarPocionAleatoria();  // Genera nuevas pociones durante el juego

private:
    void agregarPociones();  // Configuración inicial de pociones
    void agregarRobot();     // Posiciona el robot enemigo

    // Elementos gráficos
    QVector<QPixmap> framesPocion;
    QVector<Pocion*> listaPociones;
    QVector<Explosion*> listaExplosiones;
    Robot* robot = nullptr;
    Progreso* barraProgreso = nullptr;

    // Temporizadores
    QTimer* temporizadorPociones = nullptr;

    // Estados del nivel
    bool robotInicialCreado = false;
    bool pocionesAgregadas = false;
    const int totalPociones = 5;
    bool perdioGoku = false;
};

#endif // NIVEL2_H
