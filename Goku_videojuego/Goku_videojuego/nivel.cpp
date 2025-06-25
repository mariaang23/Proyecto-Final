#include "nivel.h"
#include "carro.h"
#include "goku.h"
#include "QRandomGenerator"
#include <QMessageBox>

// Constructor del nivel
nivel::nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent, int numero)
    : QWidget(parent), numeroNivel(numero), vista(view), escena(escena)
{
    if (numeroNivel == 1){
        int velocidad = 6;  // Velocidad con la que se mueve la cámara

        // Crear la cámara lógica que seguirá a Goku
        camara = new camaraLogica(vista, velocidad, this);
        camara->iniciarMovimiento();  // Comienza a mover la cámara
    }
    else if (numeroNivel == 2){
        //Logica
    }
}

// Destructor del nivel: liberar memoria de objetos dinámicos
nivel::~nivel()
{
    delete camara;
    delete goku;

    // Eliminar todas las nubes
    for (auto *item : listaNubes)
        delete item;

    // Eliminar todos los fondos
    for (auto *item : listaFondos)
        delete item;

    if(numeroNivel==1){
        delete carroFinal;

        // Eliminar todos los obstáculos
        for (obstaculo* obj : listaObstaculos)
            delete obj;

    }else if(numeroNivel==2){
        //implementar eliminar robot y pociones

    }
}

// Cargar el fondo del nivel y generar nubes decorativas
void nivel::cargarFondoNivel(const QString &ruta)
{
    if (numeroNivel == 1){

        background = QPixmap(ruta);  // Cargar imagen del fondo

        // Agregar el fondo 3 veces, uno junto al otro
        for (int i = 0; i < 4; i++) {
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

                int x = i * 250 + QRandomGenerator::global()->bounded(-60, 100);  // Posición x aleatoria
                int y = QRandomGenerator::global()->bounded(0, 80);              // Posición y aleatoria

                // Agregar la nube si está dentro del ancho del nivel
                if (x + nubeEscalada.width() <= 1536 * 4) {
                    QGraphicsPixmapItem *_nube = new QGraphicsPixmapItem(nubeEscalada);
                    _nube->setPos(x, y);
                    escena->addItem(_nube);
                    listaNubes.push_back(_nube);
                }
            }
        }
    }

    else {

        background = QPixmap(":/images/background2.png");

        QGraphicsPixmapItem *_background = new QGraphicsPixmapItem(background);
        _background->setPos(0, 0);
        escena->addItem(_background);
        listaFondos.push_back(_background);

        // Nubes (más simples, menos cantidad, pero siguen moviéndose)
        nube = QPixmap(":/images/nube.png");

        for (int i = 0; i < 50; i++) { // Cambiar 50 segun la logica
            float escala = 0.1f + QRandomGenerator::global()->bounded(0.1f);
            int ancho = nube.width() * escala;
            int alto = nube.height() * escala;

            QPixmap nubeEscalada = nube.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            int x = i * 300 + QRandomGenerator::global()->bounded(-30, 30);
            int y = QRandomGenerator::global()->bounded(0, 150);

            QGraphicsPixmapItem *_nube = new QGraphicsPixmapItem(nubeEscalada);
            _nube->setPos(x, y);
            escena->addItem(_nube);
            listaNubes.push_back(_nube);
        }

        if (!timerNubes) {
            timerNubes = new QTimer(this);
            connect(timerNubes, &QTimer::timeout, this, &nivel::moverNubes);
            timerNubes->start(45);  // cada 30 ms
        }

    }
}

void nivel::moverNubes()
{
    int velocidadNube = 2; // velocidad de desplazamiento de las nubes hacia la izquierda

    for (auto *nubeItem : listaNubes) {
        if (!nubeItem) continue;

        // Mover la nube hacia la izquierda
        nubeItem->setPos(nubeItem->x() - velocidadNube, nubeItem->y());

        // Si la nube salió de la escena (por la izquierda), reaparecer a la derecha
        if (nubeItem->x() + nubeItem->pixmap().width() < 0) {
            int nuevaX = escena->width(); // reaparece al ancho total de la escena
            int nuevaY = QRandomGenerator::global()->bounded(0, 100);
            nubeItem->setPos(nuevaX, nuevaY);
        }
    }
}

// Agregar obstáculos aleatorios al nivel
void nivel::agregarObstaculos()
{
    int xActual = 1000;       // Posición inicial de los obstáculos
    int contador = 0;
    int velocidad = 12;

    while (contador < 21) {  // Número de obstáculos a generar
        int tipo = QRandomGenerator::global()->bounded(0, 3);  // Tipo aleatorio
        obstaculo *obj = nullptr;

        switch (tipo) {
        case 0: {  // Ave
            int y = QRandomGenerator::global()->bounded(20, 170);
            obj = new obstaculo(escena, obstaculo::Ave, velocidad, this);
            obj->iniciar(xActual, y);
            xActual += 700; // + QRandomGenerator::global()->bounded(0, 100);  // pruebas de espaciado estatico
            break;
        }
        case 1: {  // Montaña
            obj = new obstaculo(escena, obstaculo::Montania, velocidad, this);
            int y = 200;  // Posición en el suelo
            obj->iniciar(xActual, y);
            xActual += 700 ; //+ QRandomGenerator::global()->bounded(0, 150);
            break;
        }
        case 2: {  // Roca
            int y = QRandomGenerator::global()->bounded(350, 500);
            obj = new obstaculo(escena, obstaculo::Roca, velocidad, this);
            obj->iniciar(xActual, y);
            xActual += 700; //+ QRandomGenerator::global()->bounded(0, 100);
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
    int velocidadGoku = 6;

    // Crear e iniciar el personaje Goku
    goku = new Goku(escena, velocidadGoku, 250, 308, 1, this);
    goku->iniciar(vista->sceneRect().left() + 10, posY);
    goku->setFocus();

    //camara->seguirAGoku(goku); //para que goku siempre se muestre en la vista
}

// Agrega el carro final que Goku debe alcanzar
void nivel::agregarCarroFinal()
{
    int x = 4500;
    int y = 500;

    carroFinal = new Carro(escena, 0, this);
    carroFinal->iniciar(x, y);
}

