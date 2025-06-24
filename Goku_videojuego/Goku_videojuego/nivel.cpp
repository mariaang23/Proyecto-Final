#include "nivel.h"
#include "carro.h"
#include "goku.h"
#include "QRandomGenerator"

// Constructor del nivel
nivel::nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent)
    : QWidget(parent), vista(view), escena(escena)
{
    int velocidad = 6;  // Velocidad con la que se mueve la cámara

    // Crear la cámara lógica que seguirá a Goku
    camara = new camaraLogica(vista, velocidad, this);
    camara->iniciarMovimiento();  // Comienza a mover la cámara
}

// Destructor del nivel: liberar memoria de objetos dinámicos
nivel::~nivel()
{
    delete camara;
    delete carroFinal;
    delete goku;

    // Eliminar todos los obstáculos
    for (obstaculo* obj : listaObstaculos)
        delete obj;

    // Eliminar todas las nubes
    for (auto *item : listaNubes)
        delete item;

    // Eliminar todos los fondos
    for (auto *item : listaFondos)
        delete item;
}

// Cargar el fondo del nivel y generar nubes decorativas
void nivel::cargarFondoNivel1(const QString &ruta)
{
    background = QPixmap(ruta);  // Cargar imagen del fondo

    // Agregar el fondo 3 veces, uno junto al otro
    for (int i = 0; i < 3; i++) {
        QGraphicsPixmapItem *_background = new QGraphicsPixmapItem(background);
        _background->setPos(i * background.width(), 0);
        escena->addItem(_background);
        listaFondos.push_back(_background);
    }

    nube = QPixmap(":/images/nube.png");

    for (int i = 0; i < 20; i++) {
        // Crear nubes de 3 tamaños distintos
        for (int j = 3; j > 0; --j) {
            float escala = j * 0.05;  // Tamaño proporcional
            int ancho = nube.width() * escala;
            int alto = nube.height() * escala;

            QPixmap nubeEscalada = nube.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            int x = i * 250 + QRandomGenerator::global()->bounded(-60, 50);  // Posición x aleatoria
            int y = QRandomGenerator::global()->bounded(0, 71);              // Posición y aleatoria

            // Agregar la nube si está dentro del ancho del nivel
            if (x + nubeEscalada.width() <= 1536 * 3) {
                QGraphicsPixmapItem *_nube = new QGraphicsPixmapItem(nubeEscalada);
                _nube->setPos(x, y);
                escena->addItem(_nube);
                listaNubes.push_back(_nube);
            }
        }
    }
}

// Agregar obstáculos aleatorios al nivel
void nivel::agregarObstaculos()
{
    int xActual = 800;       // Posición inicial de los obstáculos
    int contador = 0;
    int velocidad = 12;

    while (contador < 18) {  // Número de obstáculos a generar
        int tipo = QRandomGenerator::global()->bounded(0, 3);  // Tipo aleatorio
        obstaculo *obj = nullptr;

        switch (tipo) {
        case 0: {  // Ave
            int y = QRandomGenerator::global()->bounded(0, 250);
            obj = new obstaculo(escena, obstaculo::Ave, velocidad, this);
            obj->iniciar(xActual, y);
            xActual += 300 + QRandomGenerator::global()->bounded(0, 100);  // Espaciado
            break;
        }
        case 1: {  // Montaña
            obj = new obstaculo(escena, obstaculo::Montania, velocidad, this);
            int y = escena->height() - obj->getAltura();  // Posición en el suelo
            obj->iniciar(xActual, y);
            xActual += 600 + QRandomGenerator::global()->bounded(0, 150);
            break;
        }
        case 2: {  // Roca
            int y = QRandomGenerator::global()->bounded(350, 576);
            obj = new obstaculo(escena, obstaculo::Roca, velocidad, this);
            obj->iniciar(xActual, y);
            xActual += 600 + QRandomGenerator::global()->bounded(0, 100);
            break;
        }
        }

        listaObstaculos.push_back(obj);  // Guardar referencia
        ++contador;
    }
}

// Agrega a Goku al nivel en una posición inicial
void nivel::agregarGokuNivel1()
{
    int posY = 784 / 2;  // Altura media de la escena

    // Crear e iniciar el personaje Goku
    goku = new Goku(escena, 6, 250, 308, 1, this);
    goku->iniciar(0, posY);
    goku->setFocus();
}

// Agrega el carro final que Goku debe alcanzar
void nivel::agregarCarroFinal()
{
    int x = 3600;
    int y = 430;

    carroFinal = new Carro(escena, 0, this);
    carroFinal->iniciar(x, y);
}
