#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QPushButton>
#include<QVector>
#include<QMap>
#include<QPair>
#include "mybutton.h"
#include<QTextStream>
#include<QFile>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

constexpr int CELLWIDTH=65;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QVector<QVector<MyButton*>> map;
    int r;
    int c;
    enum Color{Red, Green, Blue, White, Tower};
    QMap<Color, QString> tostring{{Red, "red"}, {Green, "green"}, {Blue, "blue"}, {White, "white"}, {Tower, "black"}};
    QMap<Color, QList<QPair<int, int>>> paths;
    Color curColor=Red;

public:
    void OnBtnClicked(MyButton* btn);
    void configSize();
};
#endif // MAINWINDOW_H
