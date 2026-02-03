#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QPushButton>
#include <QSlider>

#include <QLabel>

#include <QVBoxLayout>

#include <QSplitter>

class Scene;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onLoadOBJModel();
    void onClearScene();
    void onChangeWindow();
    // void onHeightSliderChanged(int value);

private:

    void setupUI();
    void setupConnect();

    QPushButton *button_Create_Water;
    QPushButton *button_Import_OBJ;
    QPushButton *button_Clear;

    QPushButton *button_Change_Window;

    QSlider *ySlider;
    QSlider *xSlider;
    QSlider *zSlider;

    QLabel *modelInfoLabel;

    QVBoxLayout *buttonLayout;

    QWidget *leftMenu;
    QWidget *centralWidget;


    Scene *scene3D;

};

#endif // MAINWINDOW_H
