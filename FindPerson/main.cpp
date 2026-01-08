#include "findcharacters.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FindCharacters w;
    w.show();
    return a.exec();
}
