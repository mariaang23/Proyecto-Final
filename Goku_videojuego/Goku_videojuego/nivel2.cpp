#include "nivel2.h"
#include "pocion.h"
#include <QRandomGenerator>

Nivel2::Nivel2(QGraphicsScene* escena, QGraphicsView* vista, QWidget* parent)
    : Nivel(escena, vista, parent, 2),
    robotInicialCreado(false),
    pocionesAgregadas(false)
{}

Nivel2::~Nivel2() {
    qDebug() << "Destructor de Nivel2 llamado";

    for (auto* pocion : listaPociones)
        delete pocion;

    listaPociones.clear();
}

void Nivel2::iniciarNivel() {
    cargarFondoNivel(":/images/background2.png");
    generarNubes();
    agregarPociones();
    agregarGoku();
}

void Nivel2::cargarFondoNivel(const QString &ruta) {
    background = QPixmap(ruta);
    QGraphicsPixmapItem* _background = new QGraphicsPixmapItem(background);
    _background->setPos(0, 0);
    escena->addItem(_background);
    listaFondos.push_back(_background);
}

void Nivel2::agregarGoku() {
    int anchoFrame = 218;
    int altoFrame = 298;
    int velocidad = 12;

    goku = new Goku(escena, velocidad, anchoFrame, altoFrame, 2);

    int xInicial = 100;
    int yInicial = vista->height() - altoFrame - 30;

    goku->setSueloY(yInicial);
    goku->iniciar(xInicial, yInicial);
}

void Nivel2::agregarRobotInicial() {
    // Por implementar
}

void Nivel2::agregarPociones() {
    QPixmap hojaSprites(":/images/pocion.png");

    int anchoSprite = 65;
    int altoSprite = 64;

    for (int i = 0; i < 6; ++i) {
        QPixmap frame = hojaSprites.copy(i * anchoSprite, 0, anchoSprite, altoSprite);
        framesPocion.push_back(frame);
    }

    int cantidad = 14;
    int anchoVista = vista->width();

    int columnas = 7;
    int filas = 2;

    for (int fila = 0; fila < filas; ++fila) {
        for (int col = 0; col < columnas; ++col) {
            if (static_cast<int>(listaPociones.size()) >= cantidad)
                break;

            Pocion* pocion = new Pocion(framesPocion, anchoVista, fila, col, columnas);
            escena->addItem(pocion);
            listaPociones.push_back(pocion);
        }
    }
}

void Nivel2::actualizarNivel() {
    // Por implementar
}
