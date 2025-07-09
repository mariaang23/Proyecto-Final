#ifndef NIVEL1_H
#define NIVEL1_H

#include "nivel.h"
#include "robot.h"
#include "carro.h"
#include "camaralogica.h"

class Nivel1 : public Nivel
{
    Q_OBJECT

public:
    static int contador;

    explicit Nivel1(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent = nullptr);
    ~Nivel1() override;

    // Métodos sobrescritos de la clase base
    void iniciarNivel() override;
    void cargarFondoNivel(const QString &ruta) override;
    void agregarGoku() override;
    void actualizarNivel() override;
    void gameOver() override;
    bool haTerminado() const override;
    Goku* getGoku() const override;

    // Métodos específicos del nivel 1
    void agregarObstaculos();
    void agregarCarroFinal();
    void quitarCarroVista();
    bool getPerdioGoku() const;

private slots:
    void agregarRobots(); // Crea los robots enemigos cuando el carro aterriza

private:
    // Elementos del nivel
    camaraLogica* camara;
    Carro* carroFinal = nullptr;
    Robot* r1 = nullptr;
    Robot* r2 = nullptr;
    Robot* r3 = nullptr;

    // Estados del nivel
    bool gokuYaPateo = false;
    bool robotsCreados = false;
    bool nivelTerminado = false;
    bool perdioGoku = false;
    bool gameOverProcesado = false; // para solo llamar una vez el game over
};

#endif // NIVEL1_H
