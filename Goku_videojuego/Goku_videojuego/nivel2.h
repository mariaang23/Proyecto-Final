#ifndef NIVEL2_H
#define NIVEL2_H

#include "nivel.h"
#include "pocion.h"
#include "progreso.h"
#include "robot.h"
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
    void agregarRobot();  //para agregar robot

private:
    void agregarPociones();
    void agregarRobotInicial();

    QVector<QPixmap> framesPocion;
    std::vector<Pocion*> listaPociones;

    bool robotInicialCreado = false;
    bool pocionesAgregadas = false;

    Progreso* barraProgreso = nullptr;
    const int totalPociones = 20;

    QTimer* temporizadorPociones = nullptr;
    void agregarPocionAleatoria();

    Robot *robot;

};

#endif // NIVEL2_H
