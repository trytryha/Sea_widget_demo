#include "mainwindow.h"
#include "scene.h"

#include <QFileDialog>
#include <Qdir>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(1400, 800);

    // setAttribute(Qt::WA_TranslucentBackground);  // 设置背景透明
    // setWindowFlags(windowFlags() | Qt::FramelessWindowHint);  // 隐藏标题栏

    setupUI();

    setupConnect();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setupUI(){


    button_Create_Water = new QPushButton("创建场景", this);
    button_Import_OBJ = new QPushButton("导入模型", this);
    button_Clear = new QPushButton("清除模型", this);

    button_Create_Water->setMinimumHeight(40);
    button_Import_OBJ->setMinimumHeight(40);
    button_Clear->setMinimumHeight(40);

    ySlider = new QSlider(Qt::Horizontal, this);
    ySlider->setRange(0, 5);
    ySlider->setValue(0);
    ySlider->setEnabled(false);

    xSlider = new QSlider(Qt::Horizontal, this);
    xSlider->setRange(-10, 10);
    xSlider->setValue(0);
    xSlider->setEnabled(false);

    zSlider = new QSlider(Qt::Horizontal, this);
    zSlider->setRange(-10, 10);
    zSlider->setValue(0);
    zSlider->setEnabled(false);

    buttonLayout = new QVBoxLayout();

    buttonLayout->addWidget(button_Create_Water);
    buttonLayout->addWidget(button_Import_OBJ);
    buttonLayout->addWidget(button_Clear);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(ySlider);
    buttonLayout->addWidget(xSlider);
    buttonLayout->addWidget(zSlider);
    buttonLayout->addStretch(1);

    button_Change_Window = new QPushButton("⚙️", this);
    button_Change_Window->setFixedSize(40, 40);  // 固定大小
    button_Change_Window->setToolTip("设置");    // 鼠标悬停提示

    buttonLayout->addWidget(button_Change_Window, 0, Qt::AlignLeft);


    leftMenu = new QWidget(this);

    leftMenu->setLayout(buttonLayout);

    leftMenu->setMaximumWidth(800);
    leftMenu->setMinimumWidth(150);

    // 创建3D场景
    scene3D = new Scene(this);

    // 创建中央部件和主布局
    centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);

    splitter->addWidget(leftMenu);
    splitter->addWidget(scene3D);
    splitter->setSizes(QList<int>() << 200 << 1200);

    splitter->setHandleWidth(5);  // 设置拖动条宽度
    splitter->setStyleSheet("QSplitter::handle { background-color: #ccc; }");

    mainLayout->addWidget(splitter);
    // // 将左侧菜单和3D场景添加到主布局
    // mainLayout->addWidget(leftMenu);
    // mainLayout->addWidget(scene3D, 1);  // 第二个参数1表示场景占据剩余空间

    // 设置边距
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setCentralWidget(centralWidget);

    // centralWidget->setStyleSheet(
    //     "QWidget {"
    //     "    background-color: white;"
    //     "    border-radius: 20px;"  // 圆角半径
    //     "    border: 1px solid #e0e0e0;"  // 边框
    //     "}"
    //     );

    centralWidget->setObjectName("centralWidget");

}

void MainWindow::setupConnect()
{
    connect(button_Import_OBJ, &QPushButton::clicked, this,
            &MainWindow::onLoadOBJModel);

    connect(button_Clear, &QPushButton::clicked, this,
            &MainWindow::onClearScene);

    connect(ySlider, &QSlider::valueChanged, this, [this](int value) {
        float y = value;
        if (scene3D) {
            scene3D->setModelY(y);
        }
    });

    connect(xSlider, &QSlider::valueChanged, this, [this](int value) {
        float x = value;
        if (scene3D) {
            scene3D->setModelX(x);
        }
    });

    connect(zSlider, &QSlider::valueChanged, this, [this](int value) {
        float z = value;
        if (scene3D) {
            scene3D->setModelZ(z);
        }
    });

    connect(button_Change_Window, &QPushButton::clicked, this,
            &MainWindow::onChangeWindow);

}

void MainWindow::onLoadOBJModel()
{
    // 打开文件对话框选择OBJ文件
    QString filePath = QFileDialog::getOpenFileName(
        this,
        "选择OBJ模型文件",
        "F:/Res/Model/Test",
        "OBJ Files (*.obj);;All Files (*.*)"
        );

    if (filePath.isEmpty()) {
        qDebug() << "未选择文件";
        return;
    }

    qDebug() << "选择的文件:" << filePath;

    // 尝试加载模型
    if (scene3D && scene3D->loadOBJ(filePath)) {
        ySlider->setEnabled(true);
        xSlider->setEnabled(true);
        zSlider->setEnabled(true);
    } else {
        modelInfoLabel->setText("加载失败: 无法打开文件");
    }
}

void MainWindow::onClearScene()
{
    if (scene3D) {
        scene3D->clearModel();
        ySlider->setEnabled(false);
        xSlider->setEnabled(false);
        zSlider->setEnabled(false);
    }
}

void MainWindow::onChangeWindow()
{
    // 获取当前窗口标志
    Qt::WindowFlags flags = windowFlags();

    if (flags & Qt::FramelessWindowHint) {
        // 当前是无边框窗口，恢复标题栏
        flags &= ~Qt::FramelessWindowHint;

        // setAttribute(Qt::WA_TranslucentBackground, false);

        centralWidget->setStyleSheet(
            "#centralWidget {"
            "    background-color: white;"
            "    border-radius: 00px;"
            "    border: 3px solid #e0e0e0;"
            "}"
            );
        setFixedSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

        setWindowFlags(flags);

        show();

        resize(1400, 800);
    } else {
        // 当前有标题栏，设置为无边框
        flags |= Qt::FramelessWindowHint;

        // setAttribute(Qt::WA_TranslucentBackground, true);

        centralWidget->setStyleSheet(
            "#centralWidget {"
            "    background-color: white;"
            "    border-radius: 40px;"
            "    border: 3px solid #0000FF;"
            "}"
            );
        setFixedSize(600, 300);

        setWindowFlags(flags);

        show();
    }
}






