#include "host.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Host w;
    w.show();
    
    return a.exec();
}
