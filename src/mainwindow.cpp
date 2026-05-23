#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle("Class Routine Manager");
    resize(800, 600);
    setCentralWidget(new QLabel("Hello from Qt! Jingalala Hurhur", this));
}
