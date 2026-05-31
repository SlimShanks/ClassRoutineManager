#include "mainwindow.h"  // include our header

#include <QVBoxLayout>   // vertical layout - stacks widgets top to bottom
#include <QWidget>       // needed for the central widget

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    // set window title and size
    setWindowTitle("Class Routine Manager");
    resize(400, 300);

    // create the UI elements
    m_usernameLabel = new QLabel("Username:");
    m_passwordLabel = new QLabel("Password:");
    m_usernameInput = new QLineEdit();
    m_passwordInput = new QLineEdit();
    m_loginButton   = new QPushButton("Login");

    // make password field hide the text with dots
    m_passwordInput->setEchoMode(QLineEdit::Password);

    // create a layout and add widgets to it top to bottom
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_usernameLabel);   // "Username:" text
    layout->addWidget(m_usernameInput);   // username text field
    layout->addWidget(m_passwordLabel);   // "Password:" text
    layout->addWidget(m_passwordInput);   // password text field
    layout->addWidget(m_loginButton);     // login button

    // QMainWindow cannot have a layout set directly
    // it needs a central widget that holds the layout
    QWidget* centralWidget = new QWidget(this);  // create a container
    centralWidget->setLayout(layout);            // put layout inside container
    setCentralWidget(centralWidget);             // give container to QMainWindow
}