#pragma once  
// tells compiler to only include this file once
// prevents errors if multiple files include the same header

#include <QMainWindow>   // Qt's main window class we inherit from
#include <QLineEdit>     // text input - we use it so declare it here
#include <QPushButton>   // button - same reason
#include <QLabel>        // text label - same reason

class MainWindow : public QMainWindow  
// define our class
// "public QMainWindow" means we INHERIT from QMainWindow
// we get all of QMainWindow's features for free
// and add our own stuff on top
{
    Q_OBJECT  
    // a Qt macro - required for any class that uses
    // signals and slots (Qt's way of handling events)
    // always put this when inheriting from a Qt class

public:
    explicit MainWindow(QWidget* parent = nullptr);
    // constructor - called when you write "MainWindow window"
    // parent = nullptr means no parent widget by default
    // explicit means you can't accidentally convert other types to MainWindow

private:
    // these are MEMBER VARIABLES
    // they belong to this class and are accessible anywhere in mainwindow.cpp
    // declared here in .h so the compiler knows they exist
    // actually created in .cpp constructor with "new"
    QLabel*      m_usernameLabel;  // "Username:" text
    QLabel*      m_passwordLabel;  // "Password:" text
    QLineEdit*   m_usernameInput;  // username text field
    QLineEdit*   m_passwordInput;  // password text field
    QPushButton* m_loginButton;    // login button

    // the m_ prefix is a naming convention
    // it means "member variable" so you know it belongs to the class
};