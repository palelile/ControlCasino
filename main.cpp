#include <QtGui/QApplication>
#include "casino.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Casino w;
    w.show();
    
    return a.exec();
}
