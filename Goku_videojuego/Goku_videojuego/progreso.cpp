#include "progreso.h"
#include <QPainter>
#include <QPixmap>
#include <QDebug>

// Constructor general
Progreso::Progreso(TipoProgreso tipo, const QString& rutaIcono, QWidget *parent)
    : QWidget(parent),
    tipo(tipo),
    porcentaje(0.0f),
    totalPociones(0),
    pocionesRecolectadas(0)
{
    setFixedSize(220, 25);

    icono = new QLabel(this);
    QPixmap pixmap(rutaIcono);
    icono->setPixmap(pixmap.scaled(35, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    icono->move(0, (height() - icono->height()) / 2);
    icono->show();
}

Progreso::~Progreso()
{
    qDebug() << "Destructor de progreso llamado";
    delete icono;
}

// Nivel 1: actualizar según posición
void Progreso::actualizarProgreso(float posicionGoku, float inicio, float fin)
{
    if (tipo != Horizontal || fin == inicio) return;

    porcentaje = (posicionGoku - inicio) / (fin - inicio);
    porcentaje = qBound(0.0f, porcentaje, 1.0f);
    update();
}

// Nivel 2: establecer cantidad total de pociones
void Progreso::setTotalPociones(int total)
{
    if (tipo == Pociones) {
        totalPociones = total;
        pocionesRecolectadas = 0;
        porcentaje = 0.0f;
        update();
    }
}

// Nivel 2: sumar una poción recolectada
void Progreso::sumarPocion()
{
    if (tipo == Pociones && pocionesRecolectadas < totalPociones) {
        ++pocionesRecolectadas;
        porcentaje = static_cast<float>(pocionesRecolectadas) / totalPociones;
        update();
    }
}

// Dibujar la barra de progreso
// Dibujar la barra de progreso
void Progreso::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    int margen = 5;
    int xInicioBarra = icono->width() + margen;
    int anchoBarra = width() - xInicioBarra;
    int altoBarra = height();

    // Fondo gris de la barra
    painter.setBrush(Qt::lightGray);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(xInicioBarra, 2, anchoBarra, altoBarra - 4, 5, 5);

    // Color de la barra según tipo
    QColor colorRelleno = Qt::blue;
    painter.setBrush(colorRelleno);
    painter.drawRoundedRect(xInicioBarra, 2, anchoBarra * porcentaje, altoBarra - 4, 5, 5);

    // Borde opcional
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(xInicioBarra, 2, anchoBarra, altoBarra - 4, 5, 5);
}

float Progreso::getPorcentaje() const {
    return porcentaje;
}
