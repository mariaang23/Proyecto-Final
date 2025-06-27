#include "progreso.h"
#include <QPainter>
#include <QBrush>
#include <QColor>
#include <QPixmap>

// Constructor: se inicializa en 0% de avance
Progreso::Progreso(QWidget *parent) : QWidget(parent), porcentaje(0.0f)
{
    // Tamaño fijo del widget (incluye espacio para el ícono)
    setFixedSize(220, 25); // Ancho mayor para incluir ícono

    // Crear el ícono del carro a la izquierda de la barra
    iconoCarro = new QLabel(this);
    QPixmap pixmap(":/images/icono_carro.png");
    iconoCarro->setPixmap(pixmap.scaled(35, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconoCarro->move(0, (height() - iconoCarro->height()) / 2); // Centrado verticalmente
    iconoCarro->show();
}

// Método para actualizar el porcentaje de progreso
void Progreso::actualizarProgreso(float posicionGoku, float inicio, float fin)
{
    if (fin == inicio) return; // Evitar división por cero

    // Calcular el porcentaje de avance
    porcentaje = (posicionGoku - inicio) / (fin - inicio);
    porcentaje = qBound(0.0f, porcentaje, 1.0f); // Limitar entre 0 y 1

    update(); //llama automáticamente a paintEvent(...) para redibujar la barra
}

// Método que dibuja la barra de progreso
void Progreso::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event); // macro de Qt para evitar advertencias del compilador por no usar variable event

    QPainter painter(this);

    int margen = 5;
    int xInicioBarra = iconoCarro->width() + margen;
    int anchoBarra = width() - xInicioBarra;
    int altoBarra = height();

    // Fondo gris
    painter.setBrush(Qt::lightGray);
    painter.setPen(Qt::NoPen);
    painter.drawRect(xInicioBarra, 0, anchoBarra, altoBarra);

    // Barra de progreso azul
    painter.setBrush(Qt::blue);
    painter.drawRect(xInicioBarra, 0, anchoBarra * porcentaje, altoBarra);
}
