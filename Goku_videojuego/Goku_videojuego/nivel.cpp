#include "nivel.h"
#include "carro.h"
#include "goku.h"
#include "QRandomGenerator"
#include <QDebug>

// Recibe un puntero a la escena, la vista y el objeto padre
nivel::nivel(QGraphicsScene *escena, QGraphicsView *view, QWidget *parent)
    : QWidget(parent), vista(view), escena(escena)
{
    int velocidad = 6;  // velocidad desplazamiento camara
    camara = new camaraLogica(vista, velocidad, this); // this: nivel padre de la cámara
    camara->iniciarMovimiento();
    //camara->seguirAGoku(goku);
}

nivel::~nivel()
{
    delete camara;
    if (carroFinal) {
        delete carroFinal;
    }
}

void nivel::cargarFondoNivel1(const QString &ruta)
{
    background = QPixmap(ruta);
    // Agregar la misma imagen 3 veces, colocandola una al lado de la otra
    for (int i = 0; i <3; i++){
        QGraphicsPixmapItem *_background = new QGraphicsPixmapItem(background);
        _background -> setPos(i * background.width(), 0);   // Lo posiciona a la derecha del anterior
        escena->addItem(_background); // Se agrega a la escena, para que sea visible
    }

    //QList<QGraphicsPixmapItem*> listaNubes;
    nube = QPixmap(":/images/nube.png");

    for (int i = 0; i < 20; i++) {
        // Generar un tamaño aleatorio para cada nube
        for (int j = 3; j > 0; --j) {
            float escala = j*0.05;
            int ancho = nube.width() * escala;
            int alto = nube.height() * escala;

            // Escalar la nube
            QPixmap nubeEscalada = nube.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            int x = i * 250 + QRandomGenerator::global()->bounded(-60, 50); // Varía el espacio en x
            int y = QRandomGenerator::global()->bounded(0, 71);             // Varía el espacio en y

            if (x + nubeEscalada.width() <= 1536 * 3){  // Se agrega las nubes escaladas, siempre que no sobrepasen el ancho de la escena
                QGraphicsPixmapItem *_nube = new QGraphicsPixmapItem(nubeEscalada);
                _nube->setPos(x, y);
                escena->addItem(_nube);
                //listaNubes.append(_nube);
            }
        }
    }
}


void nivel::agregarObstaculos()
{
    int xActual = 800;  // punto inicial desde donde empiezan a agregarse obstáculos
    int anchoEscena = escena->width();
    int contador = 0;
    int velocidad = 12;

    while (contador < 18) { // se agregan obstaculos hasta que Goku no choque con el carro
        int tipo = QRandomGenerator::global()->bounded(0, 3); // 0 = Ave, 1 = Montania, 2 = Roca
        obstaculo *obj = nullptr;

        switch (tipo) {
        case 0: // Ave
        {
            int y = QRandomGenerator::global()->bounded(0, 250);  // posicion aleatoria en y para el ave, segun limites
            obj = new obstaculo(escena, obstaculo::Ave, velocidad, this);
            obj->iniciar(xActual, y);
            xActual += 300 + QRandomGenerator::global()->bounded(0, 100); // posicion x: distancia mínima 300 + distancia aleatoria (entre 0 y 100)
            break;
        }
        case 1: // Montania
        {
            obj = new obstaculo(escena, obstaculo::Montania, velocidad, this);

            // Posicionar la montaña en el suelo segun la altura de la escena
            int y = escena->height() - obj->getAltura();

            obj->iniciar(xActual, y);
            xActual += 600 + QRandomGenerator::global()->bounded(0, 150);
            break;
        }
        case 2: // Roca
        {
            int y = QRandomGenerator::global()->bounded(350, 576);
            obj = new obstaculo(escena, obstaculo::Roca, velocidad, this);
            obj->iniciar(xActual, y);
            xActual += 600 + QRandomGenerator::global()->bounded(0, 100); // distancia mínima 600
            break;
        }
        }

        ++contador;
    }
}

void nivel::agregarGokuNivel1(){
    // Calcular la posición central en y
    int posY = 784 / 2; //Alto de escena/2

    goku = new Goku(escena, 6, 250, 308, 1, this);
    goku -> iniciar(0, posY);
}

void nivel::agregarCarroFinal()
{
    int x = 3600;  // justo antes del borde
    int y = 430;   // abajo, pero sin salirse

    carroFinal = new Carro(escena, 0, this);
    carroFinal->iniciar(x, y);
    //carroFinal->rotar();
}

