#ifndef PROGRESO_H
#define PROGRESO_H

#include <QWidget>
#include <QLabel>

enum TipoProgreso {
    Horizontal,
    Pociones
};

class Progreso : public QWidget
{
    Q_OBJECT

public:
    Progreso(TipoProgreso tipo, const QString& rutaIcono, QWidget *parent = nullptr);
    ~Progreso();

    void actualizarProgreso(float posicionGoku, float inicio, float fin);
    void setTotalPociones(int total);
    void sumarPocion();
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
