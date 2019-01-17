#include "qtuner.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTuner w;
    w.show();

    return a.exec();
}
