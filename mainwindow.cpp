#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QWidget *centralWidget = new QWidget(this);
    gisViewWindow = new GISView(parent);
    listView = new QListView(parent);
    model = new CRSFileModel;
    listView->setModel(model);
    gisViewWindow->setCrsfiles(model->getCrsFiles());

    // 创建一个水平布局
    QHBoxLayout *layout = new QHBoxLayout(centralWidget);
    // 将 QListView 和 QOpenGLWidget 添加到布局中
    layout->addWidget(listView);
    layout->addWidget(gisViewWindow);
    // 设置控件的大小策略（可选）
    listView->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    gisViewWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 将布局设置到中心Widget
    centralWidget->setLayout(layout);

    // 将中心Widget设置为QMainWindow的中心Widget
    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
