#include "juego.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    juego w;
    w.show();
    return a.exec();
}
