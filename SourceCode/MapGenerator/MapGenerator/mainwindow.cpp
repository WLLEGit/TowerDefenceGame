#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->sizeButton, &QPushButton::clicked, this, &MainWindow::configSize);
    connect(ui->colorBtn, &QPushButton::clicked, this, [=](){
        curColor = (Color)((curColor + 1) % 5);
        ui->label->setText(tostring[curColor]);
    });
    connect(ui->generateBtn, &QPushButton::clicked, this, [=](){
#ifdef QT_DEBUG
        QFile fout("D:/HomeWork/GaoCheng/Project/Code/TowerDefence/SourceCode/MapConfig/newMap");
#else
        QFile fout("./newMap");
#endif

        fout.open(QIODevice::WriteOnly);
        QTextStream out(&fout);
        out << r << " " << c << endl;

        out << "Red " << paths[Red].length() << endl;
        for(auto& xy : paths[Red])
            out << xy.first << " " << xy.second << endl;

        out << "Green " << paths[Green].length() << endl;
        for(auto& xy : paths[Green])
            out << xy.first << " " << xy.second << endl;

        out << "Blue " << paths[Blue].length() << endl;
        for(auto& xy : paths[Blue])
            out << xy.first << " " << xy.second << endl;

        out << "White " << paths[White].length() << endl;
        for(auto& xy : paths[White])
            out << xy.first << " " << xy.second << endl;

        out << "Placable " << paths[Tower].length() << endl;
        for(auto& xy : paths[Tower])
            out << xy.first << " " << xy.second << endl;
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnBtnClicked(MyButton *btn)
{
    paths[curColor].push_back(QPair<int, int>(btn->r, btn->c));
    btn->setStyleSheet("background-color:" + tostring[curColor]);
}

void MainWindow::configSize()
{
    r = ui->rowLineEdit->text().toInt();
    c = ui->colLineEdit->text().toInt();
    for(auto& vec : map)
        for(auto& cell : vec)
            delete cell;
    map.resize(r);
    for(int i = 0; i < r; ++i)
    {
        map[i].resize(c);
        for(int j = 0; j < c; ++j)
        {
            map[i][j] = new MyButton(this, i, j);
            map[i][j]->setGeometry(j*CELLWIDTH, i*CELLWIDTH, CELLWIDTH, CELLWIDTH);
            map[i][j]->setText(QString("(%1, %2)").arg(i).arg(j));
            map[i][j]->setStyleSheet("background-color:Gray");
            map[i][j]->show();
            connect(map[i][j], &MyButton::mouseClicked, this, &MainWindow::OnBtnClicked);
        }
    }
}

