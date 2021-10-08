#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPushButton* btn = new QPushButton("start", this);
    gameWindow = new GameWindow(this);
    gameWindow->setAttribute(Qt::WA_DeleteOnClose);

    connect(btn, &QPushButton::clicked, this, [=](){this->hide();gameWindow->show(); gameWindow->gameStatus=GameWindow::Running;});
}

MainWindow::~MainWindow()
{
    delete ui;
}

