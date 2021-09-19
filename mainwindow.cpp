#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPushButton* btn = new QPushButton("start", this);
    gameWindow = new GameWindow(this);

    connect(btn, &QPushButton::clicked, this, [=](){this->hide();gameWindow->show();});
}

MainWindow::~MainWindow()
{
    delete ui;
}

