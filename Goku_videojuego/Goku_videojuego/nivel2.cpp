#include "nivel2.h"
#include "pocion.h"
#include <QRandomGenerator>

Nivel2::Nivel2(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent)
    : Nivel(escena, vista, parent, 2),           // Llamar al constructor base con el número de nivel 2
    robotInicialCreado(false),
    pocionesAgregadas(false)
{}


Nivel2::~Nivel2()
{
    for (auto* pocion : listaPociones)           // Libera memoria de cada poción
        delete pocion;

    listaPociones.clear();                       // Limpia la lista para evitar punteros colgantes
}

// Método principal que se llama para iniciar el nivel 2
void Nivel2::iniciarNivel()
{
    cargarFondoNivel(":/images/background2.png");  // Carga el fondo del nivel 2
    generarNubes();                                // Generar nubes (definido en clase base)
    agregarPociones();                             // Colocar las pociones en escena
}


void Nivel2::cargarFondoNivel(const QString &ruta)
{
    background = QPixmap(ruta);
    QGraphicsPixmapItem* _background = new QGraphicsPixmapItem(background); // Crea un item gráfico
    _background->setPos(0, 0);                  // Posición inicial en (0,0)
    escena->addItem(_background);               // Agrega el fondo a la escena
    listaFondos.push_back(_background);         // Guarda el fondo en la lista
}


void Nivel2::agregarGoku()
{
    // Implementar
}

void Nivel2::agregarRobotInicial()
{
    // Implementar
}

// Agrega varias pociones distribuidas por el nivel
void Nivel2::agregarPociones()
{
    QPixmap hojaSprites(":/images/pocion.png");   // Carga la hoja de sprites de pociones

    int anchoSprite = 65;                         // Ancho de cada sprite
    int altoSprite = 64;                          // Alto de cada sprite

    //Extraer los 6 frames desde la hoja de sprites
    for (int i = 0; i < 6; ++i) {
        QPixmap frame = hojaSprites.copy(i * anchoSprite, 0, anchoSprite, altoSprite); // Recorta cada frame
        framesPocion.push_back(frame);                                                // Los guarda para animación
    }

    // Cantidad total de pociones y configuración de la matriz
    int cantidad = 14;
    int anchoVista = vista->width();

    int columnas = 7;
    int filas = 2;

    // Crear las pociones y distribuirlas
    for (int fila = 0; fila < filas; ++fila) {
        for (int col = 0; col < columnas; ++col) {
            if (static_cast<int>(listaPociones.size()) >= cantidad)
                break; // Detiene si ya se alcanzó la cantidad deseada

            // Crea nueva poción y la posiciona usando su constructor
            Pocion* pocion = new Pocion(framesPocion, anchoVista, fila, col, columnas);

            escena->addItem(pocion);            // La agrega a la escena
            listaPociones.push_back(pocion);    // La guarda para futura gestión
        }
    }
}


void Nivel2::actualizarNivel()
{
    // Implementar
}
