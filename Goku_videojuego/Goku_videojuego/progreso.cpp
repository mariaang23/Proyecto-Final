#include "progreso.h"
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <stdexcept>  // Para lanzar excepciones estándar

// Constructor general para ambos tipos de progreso (Horizontal o Pociones)
Progreso::Progreso(TipoProgreso tipo, const QString& rutaIcono, QWidget *parent)
    : QWidget(parent),
    tipo(tipo),
    porcentaje(0.0f),
    totalPociones(0),
    pocionesRecolectadas(0)
{
    setFixedSize(220, 25); // Tamaño fijo del widget, para contener barra e ícono

    // Crear ícono del progreso
    icono = new QLabel(this);
    QPixmap pixmap(rutaIcono);

    // Validar que la imagen se haya cargado correctamente
    if (pixmap.isNull()) {
        throw std::runtime_error("Progreso: No se pudo cargar el ícono de progreso desde la ruta proporcionada.");
    }

    // Redimensionar ícono con buena calidad
    icono->setPixmap(pixmap.scaled(35, 25, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Centrar verticalmente el ícono dentro del widget
    icono->move(0, (height() - icono->height()) / 2);
    icono->show();
}

// Destructor
Progreso::~Progreso()
{
    qDebug() << "Destructor de progreso llamado";
    // QLabel se destruye automáticamente por parentesco
}

// Actualiza el porcentaje de progreso basado en la posición de Goku
void Progreso::actualizarProgreso(float posicionGoku, float inicio, float fin)
{
    // Solo aplica si el tipo es Horizontal (Nivel 1)
    if (tipo != Horizontal || fin == inicio)
        return;

    porcentaje = (posicionGoku - inicio) / (fin - inicio);   // Calcular progreso como proporción entre 0 y 1
    porcentaje = qBound(0.0f, porcentaje, 1.0f);             // Limitar entre 0% y 100% usando función de Qt
    update();                                                // Solicita repintado del widget
}

// Configura la cantidad total de pociones necesarias para completar el nivel
void Progreso::setTotalPociones(int total)
{
    // Aplica solo si el tipo es Pociones (Nivel 2)
    if (tipo == Pociones) {

        // Validar valor positivo
        if (total <= 0) {
            throw std::invalid_argument("Progreso: El total de pociones debe ser mayor que 0.");
        }
        totalPociones = total;
        pocionesRecolectadas = 0;
        porcentaje = 0.0f;
        update();
    }
}

// Incrementa el conteo de pociones recolectadas y actualiza el progreso
void Progreso::sumarPocion()
{
    // Solo para nivel 2
    if (tipo == Pociones && pocionesRecolectadas < totalPociones) {

        // Incrementa la cuenta y actualiza porcentaje
        ++pocionesRecolectadas;
        porcentaje = static_cast<float>(pocionesRecolectadas) / totalPociones;
        update();
    }
}

// Método sobrescrito para dibujar la barra de progreso de forma personalizada
void Progreso::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event); // No se utiliza el evento

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true); // Borde suave

    // Margen entre ícono y barra
    int margen = 5;
    int xInicioBarra = icono->width() + margen;

    // Cálculo del tamaño de la barra
    int anchoBarra = width() - xInicioBarra;
    int altoBarra = height();

    // Dibujo del fondo de la barra
    painter.setBrush(Qt::lightGray);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(xInicioBarra, 2, anchoBarra, altoBarra - 4, 5, 5);

    // Dibujo del relleno (avance) de la barra
    QColor colorRelleno = Qt::blue;
    painter.setBrush(colorRelleno);
    painter.drawRoundedRect(xInicioBarra, 2, anchoBarra * porcentaje, altoBarra - 4, 5, 5);

    // Dibujo del borde final
    painter.setPen(QPen(Qt::black, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(xInicioBarra, 2, anchoBarra, altoBarra - 4, 5, 5);
}

// Devuelve el porcentaje actual del progreso
float Progreso::getPorcentaje() const {
    return porcentaje;
}
