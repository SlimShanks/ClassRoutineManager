#include <QApplication>  // Qt's app manager
#include "mainwindow.h"  // import our menu so we can use MainWindow

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);  
    // every Qt app needs this ONE object
    // it handles the app lifecycle, events, etc
    // argc and argv are command line arguments passed from OS

    MainWindow window;  
    // create our window using the class we defined
    // this calls MainWindow::MainWindow() constructor in mainwindow.cpp

    window.show();      
    // make the window visible on screen

    return app.exec();  
    // start the event loop
    // this keeps the app alive, waiting for user input
    // when user closes the window, exec() returns and app exits
}