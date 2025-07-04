#ifndef PROGRESO_H
#define PROGRESO_H

#include <QWidget>
#include <QLabel>

// Tipos de progreso disponibles
enum TipoProgreso {
    Horizontal, // Nivel 1: basado en posición de Goku
    Pociones    // Nivel 2: basado en cantidad de pociones recolectadas
};

class Progreso : public QWidget
{
    Q_OBJECT

public:
    Progreso(TipoProgreso tipo, const QString& rutaIcono, QWidget *parent = nullptr);
    ~Progreso();

    // Nivel 1: actualiza el progreso según el avance horizontal
    void actualizarProgreso(float posicionGoku, float inicio, float fin);

    // Nivel 2: establece el total de pociones a recolectar
    void setTotalPociones(int total);
    void sumarPocion(); // suma una poción recolectada

    float getPorcentaje() const;


protected:
    void paintEvent(QPaintEvent *event) override;

private:
    TipoProgreso tipo;
    float porcentaje;

    QLabel *icono;
    int totalPociones;
    int pocionesRecolectadas;
};

#endif // PROGRESO_H
