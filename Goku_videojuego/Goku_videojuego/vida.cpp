#include "vida.h"
#include <QGraphicsDropShadowEffect>  // Para aplicar sombra al texto y barra
#include <QFont>
#include <QHBoxLayout>
#include <QDebug>
#include <stdexcept>  // Para lanzar excepciones

// Constructor de la clase Vida
Vida::Vida(QWidget *parent)
    : QWidget(parent), vidaActual(vidaMaxima) // Se inicia con la vida completa
{
    // Validación crítica: asegurar de que el puntero parent es válido
    if (parent == nullptr) {
        qWarning() << "Vida: se ha creado sin un QWidget padre explícito.";
    }

    texto = new QLabel("HEALTH", this);       // Crear y configurar la etiqueta "HEALTH"

    QFont fuente("Arial", 20, QFont::Bold);   // Fuente grande y en negrita
    texto->setFont(fuente);

    texto->setStyleSheet("color: #FF3333;");  // Color rojo brillante para destacar sobre fondo azul

    // Sombra negra debajo del texto para mejorar contraste
    auto *sombraTexto = new QGraphicsDropShadowEffect(this);
    sombraTexto->setOffset(2, 2);              // Desplazamiento de la sombra
    sombraTexto->setColor(Qt::black);          // Color negro
    sombraTexto->setBlurRadius(4);             // Difuminado de la sombra
    texto->setGraphicsEffect(sombraTexto);     // Aplicar sombra al texto

    // Crear y configurar la barra de vida
    barra = new QProgressBar(this);
    if (!barra)
        throw std::runtime_error("Vida: no se pudo crear la barra de vida.");

    barra->setRange(0, vidaMaxima);            // Rango: 0 a 100
    barra->setValue(vidaActual);               // Valor inicial: vida completa
    barra->setTextVisible(false);              // Oculta el porcentaje numérico
    barra->setFixedSize(100, 14);              // Tamaño fijo de la barra
    actualizarColor();                         // Aplicar color inicial a la barra

    // Estilo visual de la barra (borde negro, fondo gris y chunk verde)
    barra->setStyleSheet(
        "QProgressBar {"
        "  background-color: gray;"
        "  border: 2px solid black;"           // Borde negro visible
        "  border-radius: 3px;"                // Bordes redondeados
        "  padding: 1px;"
        "}"
        "QProgressBar::chunk {"
        "  background-color: #00cc66;"         // Color inicial: verde
        "}");

    // Sombra negra sobre la barra de vida
    auto *sombraBarra = new QGraphicsDropShadowEffect(this);
    sombraBarra->setOffset(2, 2);              // Sombra desplazada
    sombraBarra->setColor(Qt::black);
    sombraBarra->setBlurRadius(5);
    barra->setGraphicsEffect(sombraBarra);     // Aplicar sombra a la barra

    // Disposición horizontal de los elementos
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);      // Sin márgenes
    layout->setSpacing(8);                       // Separación entre texto y barra
    layout->addWidget(texto);                    // Agregar texto al layout
    layout->addWidget(barra);                    // Agregar barra al layout
    setLayout(layout);                           // Establecer el layout principal
    setAttribute(Qt::WA_TranslucentBackground);  // Hace que el fondo del widget sea transparente
}

// Destructor: los hijos (texto, barra, layout) se destruyen automáticamente por Qt
Vida::~Vida() {
    //qDebug() << "Destructor de vida llamado";
}

// Método para reducir la vida
void Vida::restar(int cantidad)
{
    if (cantidad < 0)
        throw std::invalid_argument("Vida::restar - cantidad negativa no permitida.");

    vidaActual -= cantidad;                    // Resta vida
    if (vidaActual < 0)
        vidaActual = 0;                        // Evita valores negativos

    barra->setValue(vidaActual);              // Actualiza visualmente la barra
    actualizarColor();                        // Cambia color según el nivel de vida
}

// Método para reiniciar la vida a su valor máximo
void Vida::reiniciar()
{
    vidaActual = vidaMaxima;                  // Restaura vida completa
    barra->setValue(vidaActual);              // Actualiza visual
    actualizarColor();                        // Restablece el color verde
}

// Obtener la cantidad actual de vida
int Vida::obtenerVida() const
{
    return vidaActual;
}

// Actualiza el color de la barra según el nivel de vida
void Vida::actualizarColor()
{
    QString color;

    // Determina el color de la barra según el nivel de vida
    if (vidaActual < 40)
        color = "#ff4c4c"; // Rojo intenso (peligro)
    else if (vidaActual < 70)
        color = "#ffd700"; // Amarillo (advertencia)
    else
        color = "#00cc66"; // Verde (saludable)

    // Aplica nuevo estilo con el color dinámico
    barra->setStyleSheet(QString(
                             "QProgressBar {"
                             "  background-color: gray;"
                             "  border: 2px solid black;"
                             "  border-radius: 3px;"
                             "  padding: 1px;"
                             "}"
                             "QProgressBar::chunk {"
                             "  background-color: %1;"
                             "}").arg(color));            // %1 se reemplaza con el color definido arriba
}
