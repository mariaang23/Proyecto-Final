#include "vida.h"
#include <QGraphicsDropShadowEffect>  // Para aplicar sombra al texto y barra
#include <QFont>
#include <QHBoxLayout>
#include <QDebug>
#include <stdexcept>

/**
@brief Constructor de la clase Vida.

Inicializa un widget que muestra el estado de vida con una etiqueta y una barra gráfica.

@param parent Puntero al widget padre.

@details

Crea y configura una etiqueta con el texto "HEALTH" en fuente Arial, tamaño 20 y negrita, con color rojo brillante.

Aplica una sombra negra al texto para mejorar el contraste visual.

Crea una barra de progreso que representa la vida, con rango de 0 a 100 y valor inicial al máximo.

Configura la barra para que no muestre el porcentaje numérico y le aplica un estilo visual con borde negro, fondo gris y barra verde.

Añade una sombra negra también a la barra de vida.

Organiza horizontalmente el texto y la barra dentro del widget sin márgenes y con separación entre ellos.

Establece el fondo del widget como transparente.

Lanza una excepción si no se puede crear la barra.
*/
Vida::Vida(QWidget *parent)
    : QWidget(parent), vidaActual(vidaMaxima) // Se inicia con la vida completa
{
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

/**
@brief Destructor de la clase Vida.

Gestiona la destrucción del widget y sus componentes hijos.
No requiere acciones explícitas adicionales ya que Qt libera automáticamente los objetos hijos.
*/
Vida::~Vida() {
    //qDebug() << "Destructor de vida llamado";
}

/**
@brief Reduce la cantidad actual de vida en el valor especificado.

@param cantidad Entero positivo que indica cuánto se debe restar de la vida actual.

@details

Lanza una excepción si se intenta restar una cantidad negativa.

Actualiza el valor interno de vida, asegurando que no sea menor que cero.

Refleja el nuevo valor en la barra gráfica.

Cambia el color de la barra según el nivel de vida actual para indicar estados críticos.
*/
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

/**
@brief Restablece la vida al valor máximo.

@details

Restaura el valor interno de vida a su máximo permitido.

Actualiza visualmente la barra para reflejar la vida completa.

Actualiza el color de la barra para indicar estado saludable.
*/
void Vida::reiniciar()
{
    vidaActual = vidaMaxima;                  // Restaura vida completa
    barra->setValue(vidaActual);              // Actualiza visual
    actualizarColor();                        // Restablece el color verde
}

/**
@brief Obtiene la cantidad actual de vida.

@return Valor entero que representa la vida actual.
*/
int Vida::obtenerVida() const
{
    return vidaActual;
}

/**
@brief Actualiza el color de la barra de vida según el nivel actual de vida.

@details

Si la vida es menor a 40, el color cambia a rojo intenso (indica peligro).

Si la vida está entre 40 y 70, el color cambia a amarillo (advertencia).

Si la vida es mayor o igual a 70, el color es verde (estado saludable).

Aplica el estilo visual correspondiente al widget de la barra con el color dinámico.
*/
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
