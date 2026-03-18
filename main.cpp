#include "rawcleaner.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RawCleaner w;
    w.show();
    return a.exec();
}
