#include "mainwindow.h"
#include "dbmanager.h"

#include <QVBoxLayout>    // vertical layout - stacks widgets top to bottom
#include <QWidget>        // needed for pages inside the stack
#include <QMessageBox>    // popup message boxes for errors
#include <QStackedWidget> // lets us switch between pages

MainWindow::MainWindow(DBManager* db, QWidget* parent) : QMainWindow(parent)
{
    m_db = db;  // store the db pointer

    setWindowTitle("Class Routine Manager");
    resize(400, 400);

    // create the stack — this holds all our pages
    m_stack = new QStackedWidget(this);
    setCentralWidget(m_stack);  // stack is the central widget now

    // build each page and add to stack
    setupLoginPage();    // page 0 — login form
    setupDashboard();    // page 1 — admin dashboard

    // start on the login page
    m_stack->setCurrentIndex(0);
}

void MainWindow::setupLoginPage()
{
    // create a container widget for the login page
    QWidget* loginPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(loginPage);
    layout->setContentsMargins(60, 60, 60, 60);  // padding around the form
    layout->setSpacing(10);                        // space between widgets

    // create widgets
    m_usernameLabel = new QLabel("Username:");
    m_passwordLabel = new QLabel("Password:");
    m_usernameInput = new QLineEdit();
    m_passwordInput = new QLineEdit();
    m_loginButton   = new QPushButton("Login");

    // hide password text
    m_passwordInput->setEchoMode(QLineEdit::Password);

    // add to layout
    layout->addWidget(m_usernameLabel);
    layout->addWidget(m_usernameInput);
    layout->addWidget(m_passwordLabel);
    layout->addWidget(m_passwordInput);
    layout->addWidget(m_loginButton);

    // connect button to slot
    connect(m_loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);

    // add this page to the stack
    m_stack->addWidget(loginPage);
}

void MainWindow::setupDashboard()
{
    // create a container widget for the dashboard
    QWidget* dashPage = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(dashPage);
    layout->setContentsMargins(60, 60, 60, 60);
    layout->setSpacing(20);

    // title label
    QLabel* titleLabel = new QLabel("Admin Dashboard");
    titleLabel->setAlignment(Qt::AlignCenter);  // center the text

    // create buttons
    m_addTeacherButton  = new QPushButton("Add Teacher");
    m_makeRoutineButton = new QPushButton("Make Routine");

    // add to layout
    layout->addWidget(titleLabel);
    layout->addWidget(m_addTeacherButton);
    layout->addWidget(m_makeRoutineButton);
    layout->addStretch();  // pushes everything to the top

    // add this page to the stack
    m_stack->addWidget(dashPage);
}

void MainWindow::onLoginClicked()
{
    QString username = m_usernameInput->text();
    QString password = m_passwordInput->text();

    if (m_db->checkLogin(username, password)) {
        onLoginSuccess();  // go to dashboard
    } else {
        // show error popup
        QMessageBox::warning(this, "Login Failed", "Wrong username or password!");
    }
}

void MainWindow::onLoginSuccess()
{
    // switch to dashboard page (index 1)
    m_stack->setCurrentIndex(1);
}