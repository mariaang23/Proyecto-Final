#include "nivel.h"
#include "carro.h"
#include "goku.h"
#include "vida.h"
#include "robot.h"
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

        // timer que mira los eventos del nivel
        timerNivel = new QTimer(this);
        connect(timerNivel, &QTimer::timeout, this, &nivel::actualizarNivel);
        timerNivel->start(20);
      
        // Timer para actualizar la barra de progreso
        QTimer *timerProgreso = new QTimer(this);
        connect(timerProgreso, &QTimer::timeout, this, [=]() {
            if (goku && carroFinal && barraProgreso) {
                float posGoku = goku->x();
                float inicio = 0;
                float fin = carroFinal->getSprite()->pos().x();
                barraProgreso->actualizarProgreso(posGoku, inicio, fin);
            }
        });
        timerProgreso->start(50); // Actualiza cada 50 ms
    }
    else if (numeroNivel == 2){
        // Lógica para nivel 2
    }
}

// Destructor del nivel: liberar memoria de objetos dinámicos
nivel::~nivel()
{
    delete camara;
    delete goku;
    delete barraVida;  //Asegúrate de liberar también la memoria de la barra de vida

    // Eliminar todas las nubes
    for (auto *item : listaNubes)
        delete item;

    // Eliminar todos los fondos
    for (auto *item : listaFondos)
        delete item;

    if(numeroNivel==1){
        delete carroFinal;
        delete barraProgreso;

        // Eliminar todos los obstáculos
        for (obstaculo* obj : listaObstaculos)
            delete obj;

    }else if(numeroNivel==2){
        // Implementar eliminar robot y pociones
    }
}

// Cargar el fondo del nivel y generar nubes decorativas
void nivel::cargarFondoNivel(const QString &ruta)
{
    if (numeroNivel == 1){
        background = QPixmap(ruta);  // Cargar imagen del fondo

        // Agregar 4 fondos contiguos para hacer scroll horizontal
        for (int i = 0; i < 4; i++) {
            QGraphicsPixmapItem *_background = new QGraphicsPixmapItem(background);
            _background->setPos(i * background.width(), 0);
            escena->addItem(_background);
            listaFondos.push_back(_background);
        }

        nube = QPixmap(":/images/nube.png");

        for (int i = 0; i < 35; i++) {
            for (int j = 3; j > 0; --j) {
                float escala = j * 0.05;
                int ancho = nube.width() * escala;
                int alto = nube.height() * escala;

                QPixmap nubeEscalada = nube.scaled(ancho, alto, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                int x = i * 250 + QRandomGenerator::global()->bounded(-60, 100);
                int y = QRandomGenerator::global()->bounded(0, 80);

                if (x + nubeEscalada.width() <= 1536 * 4) {
                    QGraphicsPixmapItem *_nube = new QGraphicsPixmapItem(nubeEscalada);
                    _nube->setPos(x, y);
                    escena->addItem(_nube);
                    listaNubes.push_back(_nube);
                }
            }
        }
    } else {
        // Lógica para nivel 2 (más simple)

        background = QPixmap(":/images/background2.png");

        QGraphicsPixmapItem *_background = new QGraphicsPixmapItem(background);
        _background->setPos(0, 0);
        escena->addItem(_background);
        listaFondos.push_back(_background);

        nube = QPixmap(":/images/nube.png");

        for (int i = 0; i < 50; i++) {
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

        // Timer que anima el movimiento lateral de nubes
        if (!timerNubes) {
            timerNubes = new QTimer(this);
            connect(timerNubes, &QTimer::timeout, this, &nivel::moverNubes);
            timerNubes->start(45);
        }
    }
}

// Mover nubes horizontalmente para animación
void nivel::moverNubes()
{
    int velocidadNube = 2;

    for (auto *nubeItem : listaNubes) {
        if (!nubeItem) continue;

        nubeItem->setPos(nubeItem->x() - velocidadNube, nubeItem->y());

        if (nubeItem->x() + nubeItem->pixmap().width() < 0) {
            int nuevaX = escena->width();
            int nuevaY = QRandomGenerator::global()->bounded(0, 100);
            nubeItem->setPos(nuevaX, nuevaY);
        }
    }
}

// Agregar obstáculos aleatorios al nivel
void nivel::agregarObstaculos()
{
    int xActual = 2000;
    int contador = 0;
    int velocidad = 10;

    while (contador < 13) {
        int tipo = QRandomGenerator::global()->bounded(0, 3);
        obstaculo *obj = nullptr;

        switch (tipo) {
        case 0: { // Ave
            // Evita que las aves aparezcan en el área del HUD
            int y = QRandomGenerator::global()->bounded(margenHUD + 5, margenHUD + 130);
            obj = new obstaculo(escena, obstaculo::Ave, velocidad, this);
            obj->iniciar(xActual, y);
            xActual += 600;
            break;
        }
        case 1: { // Montaña (siempre en el suelo)
            obstaculo *obj = new obstaculo(escena, obstaculo::Montania, velocidad, this);
            obj->cargarImagenes(); // Cargar primero para conocer la altura real del sprite

            int alturaEscena = escena->height();
            int alturaMontana = obj->getAltura();  // método que debes implementar
            int y = alturaEscena - alturaMontana;

            obj->iniciar(xActual, y);
            xActual += 800;
            break;
        }
        case 2: { // Roca
            // Asegurarse de que no invada el HUD
            int y = QRandomGenerator::global()->bounded(std::max(350, margenHUD + 50), 550);
            obj = new obstaculo(escena, obstaculo::Roca, velocidad, this);
            obj->iniciar(xActual, y);
            xActual += 700;
            break;
        }
        }

        listaObstaculos.push_back(obj);
        ++contador;
    }
}


// Agrega a Goku y su barra de vida al nivel
void nivel::agregarGokuNivel1()
{
    int posY = std::max(margenHUD + 10, 784 / 2); // Posición vertical tomando margen HUD
    int velocidadGoku = 6;

    // Crear la barra de vida (como widget encima de la vista)
    barraVida = new Vida(vista);                     // Se mostrará encima del QGraphicsView
    barraVida->move(20, 20);                         // Posición fija en la esquina superior izquierda
    barraVida->show();       // Añadir a la escena

    barraProgreso = new Progreso(vista);
    barraProgreso->move(20, 60); //  Ubicarla debajo de la barra de vida
    barraProgreso->show();  // Añadir a la escena

    // creacion de Goku y asociacion con vida
    goku = new Goku(escena, velocidadGoku, 200, 249, 1, this);
    goku->setBarraVida(barraVida);  // Enlace para que Goku pueda modificar la barra
    goku->iniciar(vista->sceneRect().left() + 10, posY); // Posición de inicio
    goku->setFocus();

    // camara->seguirAGoku(goku); // Habilitar si quieres que la cámara siga a Goku
}

// Agrega el carro final que Goku debe alcanzar
void nivel::agregarCarroFinal()
{
    int x = 4700;
    int y = 500;

    carroFinal = new Carro(escena, 0, this);
    carroFinal->iniciar(x, y);
}

void nivel::actualizarNivel()
{
    if(numeroNivel==1){
        if (!goku) return;  //si no se creo goku aun

        //si goku toco el carro
        if (goku->haTocadoCarro() && carroFinal && !gokuYaPateo) {
            gokuYaPateo = true;                       //solo la primera vez
            goku->patadaGokuNivel1();             // (ahora ella misma se detiene)
            carroFinal->iniciarMovimientoEspiral();
        }

        //incluir robots
        if (carroFinal && carroFinal->espiralHecha  // termino espiral
            && carroFinal->haLlegadoAlSuelo()){  // toco suelo
            agregarRobotsNivel1();
        }

        //fin de juego por vidas

    }else if(numeroNivel==2){
        //implementar
    }
}
int nivel::getMargenHUD() const {
    return margenHUD;
}

void nivel::agregarRobotsNivel1()
{
    //exista carro primero
    if (!carroFinal || robotsCreados) return;
    if (!carroFinal->haLlegadoAlSuelo())   return;   // si toca suelo
    robotsCreados = true;                        // evita duplicar

    const int ySuelo = 400;          // coordenada Y del suelo
    const int vel    = 6;            // velocidad lateral

    //creacion de los robots
    Robot *robot1 = new Robot(escena, vel, 1, this);
    Robot *robot2 = new Robot(escena, vel, 2, this);
    Robot *robot3 = new Robot(escena, vel, 3, this);

    //SECUENCIAS SEGUN EL SLOT
    robot1->iniciar(5000, ySuelo, 5000);   // Robot 1: destino 5500
    robot1->desplegarRobot();                 // frame 0-1-3-2-5-4


    QTimer::singleShot(600, this, [=]() {
        robot2->iniciar(5300, ySuelo, 5300);  // destino 5300
        robot2->desplegarRobot();
    });


    QTimer::singleShot(1200, this, [=]() {
        robot3->iniciar(5600, ySuelo, 5600);  // destino 4800
        robot3->desplegarRobot();
    });

    //activar modo marcha
    QTimer::singleShot(1800,this,[=](){
        robot1->detenerMvtoRobot();
        robot2->detenerMvtoRobot();
        robot3->detenerMvtoRobot();
    });
}
