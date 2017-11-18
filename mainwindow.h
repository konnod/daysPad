#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <fstream>
#include <vector>
#include <iostream>
#include <string>
#include <QDate>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool writeMapToFile();
    bool readMapFromFile();
    void cleanUpMap();
    void start();
    void saveToMap();

private slots:
    void on_calendarWidget_selectionChanged();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
