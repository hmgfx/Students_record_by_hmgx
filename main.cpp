#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Student Management System");// Function recall kiya gaya he
    app.setOrganizationName("HaroonMughalGFX"); //By my name
    app.setStyle("Fusion");

    MainWindow w;
    w.show();

    return app.exec();
}
