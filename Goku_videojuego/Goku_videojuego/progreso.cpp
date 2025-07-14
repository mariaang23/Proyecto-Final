#include "progreso.h"
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <stdexcept>  // Para lanzar excepciones estándar

/**
@brief Constructor de la clase Progreso.

Crea un widget gráfico que muestra una barra de progreso junto con un ícono representativo. Puede funcionar para mostrar progreso horizontal o avance en recolección de pociones.

@param tipo Tipo de progreso que se mostrará (Horizontal o Pociones).
@param rutaIcono Ruta del archivo de imagen que se usará como ícono del progreso.
@param parent Puntero al widget padre.

@details

Establece un tamaño fijo para el widget que contiene la barra y el ícono.

Carga y valida la imagen del ícono, lanzando excepción si falla la carga.

Escala el ícono manteniendo la proporción para ajustarlo al tamaño del widget.

Centra verticalmente el ícono dentro del área del widget.
*/
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

/**
@brief Destructor de la clase Progreso.

Libera los recursos asociados al widget.
No requiere eliminación explícita de los objetos hijos, ya que Qt gestiona automáticamente la memoria de los widgets con padres asignados.
*/
Progreso::~Progreso()
{
    //qDebug() << "Destructor de progreso llamado";
    // No se necesita delete explícito: QLabel tiene parent (this)
}

/**
@brief Actualiza el porcentaje de progreso basado en la posición actual de Goku.

Este método calcula y actualiza el progreso horizontal relativo a un rango definido por inicio y fin, correspondiente a la posición de Goku en el nivel.

@param posicionGoku Posición actual de Goku en el eje horizontal.
@param inicio Posición mínima o inicial del rango del nivel.
@param fin Posición máxima o final del rango del nivel.

@details

Solo se aplica si el tipo de progreso es Horizontal.

Calcula el porcentaje como proporción normalizada dentro del rango [inicio, fin].

Usa qBound para asegurar que el porcentaje esté entre 0.0 y 1.0.

Solicita el repintado del widget para reflejar visualmente el nuevo progreso.
*/
void Progreso::actualizarProgreso(float posicionGoku, float inicio, float fin)
{
    // Solo aplica si el tipo es Horizontal (Nivel 1)
    if (tipo != Horizontal || fin == inicio)
        return;

    porcentaje = (posicionGoku - inicio) / (fin - inicio);   // Calcular progreso como proporción entre 0 y 1
    porcentaje = qBound(0.0f, porcentaje, 1.0f);             // Limitar entre 0% y 100% usando función de Qt
    update();                                                // Solicita repintado del widget
}

/**
@brief Configura la cantidad total de pociones necesarias para completar el progreso.

@param total Número total de pociones que se deben recolectar.

@details

Solo afecta si el tipo de progreso es Pociones.

Valida que el total sea un número positivo, lanzando una excepción si no lo es.

Reinicia el contador de pociones recolectadas y el porcentaje de progreso.

Actualiza la visualización del widget para reflejar los cambios.
*/
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

/**
@brief Incrementa el conteo de pociones recolectadas y actualiza el progreso.

@details

Solo se aplica si el tipo de progreso es Pociones.

Incrementa el contador interno de pociones recolectadas siempre que no se haya alcanzado el total.

Calcula el nuevo porcentaje de progreso basado en la proporción entre pociones recolectadas y el total.

Solicita la actualización visual del widget para reflejar el nuevo estado.
*/
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

/**
@brief Método de evento que dibuja la barra de progreso personalizada.

Se encarga de renderizar el widget, incluyendo el fondo, el relleno que representa el avance actual y el borde de la barra de progreso.

@param event Evento de pintura (no utilizado explícitamente).

@details

Aplica antialiasing para suavizar bordes.

Calcula el área disponible para la barra considerando el tamaño del ícono y un margen.

Dibuja un rectángulo redondeado gris como fondo.

Dibuja un rectángulo redondeado azul que representa el porcentaje de progreso actual.

Dibuja el borde negro alrededor de la barra para definir visualmente su contorno.
*/
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

/**
@brief Devuelve el porcentaje actual de progreso.

@return Valor flotante entre 0.0 y 1.0 que representa el avance del progreso.
*/
float Progreso::getPorcentaje() const {
    return porcentaje;
}
