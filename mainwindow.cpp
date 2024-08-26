#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    gisViewWindow = new GISView(parent);
    setCentralWidget(gisViewWindow);
}

MainWindow::~MainWindow()
{
    delete geoData;
    delete ui;
}

