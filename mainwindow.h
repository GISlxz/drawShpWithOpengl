#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QHBoxLayout>
#include <QListView>
#include "gisview.h"
#include "crsfile.h"
#include "crsfilemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel *labCSCord; //当前地理坐标

public:
    GISView* gisViewWindow;
    QListView *listView;
    CRSFileModel *model;
};
#endif // MAINWINDOW_H
