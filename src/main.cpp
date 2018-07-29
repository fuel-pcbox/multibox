#include "core.h"
#include "info.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Core multibox;
    printf("Welcome to MultiBox\n");
    printf("Version %s\nBuilt on %s at %s\n", version, __DATE__, __TIME__);
    multibox.showMaximized();
    return app.exec();
}