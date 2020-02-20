#include <QApplication>

//#include "Window/MainWindow.h"
#include "Window/MyGraphicView.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    MyGraphicView w;
    w.show();

    return a.exec();
}
