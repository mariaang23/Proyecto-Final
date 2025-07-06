#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include "pocion.h"
#include "progreso.h"
#include "robot.h"
#include "explosion.h"
#include <vector>
#include <QVector>

class Nivel2 : public Nivel
{
    Q_OBJECT

public:
    Nivel2(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent = nullptr);
    ~Nivel2();

    void iniciarNivel() override;
    void cargarFondoNivel(const QString &ruta) override;
    void agregarGoku() override;
    void actualizarNivel() override;
    Goku* getGoku() const;

    void pocionRecolectada();
    void agregarExplosion(Explosion* e);

private:
    void agregarPociones();
    void agregarRobot();
    void agregarExplosion();
    void gameOver();

    QVector<QPixmap> framesPocion;
    std::vector<Pocion*> listaPociones;
    std::vector<Explosion*> listaExplosiones;

    bool robotInicialCreado = false;
    bool pocionesAgregadas = false;

    Progreso* barraProgreso = nullptr;
    const int totalPociones = 20;

    QTimer* temporizadorPociones = nullptr;
    void agregarPocionAleatoria();

    Robot *robot;
    Explosion *explosion;

    bool perdioGoku = false;

signals:
    void nivelCompletado();
};

#endif // NIVEL2_H
