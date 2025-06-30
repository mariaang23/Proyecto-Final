#include "nivel2.h"
#include <QRandomGenerator>
#include <QTimer>

Nivel2::Nivel2(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent)
    : Nivel(escena, vista, parent, 2),
    timerNivel(nullptr),
    robotInicialCreado(false),
    pocionesAgregadas(false)
{}


// Destructor: libera memoria
Nivel2::~Nivel2()
{
}

void Nivel2::iniciarNivel()
{
    cargarFondoNivel(":/images/background2.png");

    generarNubes();
    /*
    agregarGoku();
    agregarRobotInicial();
    agregarPociones();

    timerNivel = new QTimer(this);
    connect(timerNivel, &QTimer::timeout, this, &Nivel2::actualizarNivel);
    timerNivel->start(20);
    */
}

void Nivel2::cargarFondoNivel(const QString &ruta)
{
    // Fondo estático
    background = QPixmap(ruta);
    QGraphicsPixmapItem* _background = new QGraphicsPixmapItem(background);
    _background->setPos(0, 0);
    escena->addItem(_background);
    listaFondos.push_back(_background);

    // Cargar imagen de nube
    nube = QPixmap(":/images/nube.png");
}


// Agrega a Goku con barras de vida y progreso
void Nivel2::agregarGoku()
{
    // Implementar
}

// Agrega un robot enemigo al principio del nivel
void Nivel2::agregarRobotInicial()
{
    // Implementar
}

// Agrega pociones de vida repartidas en el nivel
void Nivel2::agregarPociones()
{
    // Implementar
}

// Método principal que actualiza el estado del nivel 2
void Nivel2::actualizarNivel()
{
    // Implementar
}



