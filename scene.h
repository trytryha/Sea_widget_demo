#ifndef SCENE_H
#define SCENE_H

#include <QWidget>
#include <Qt3DExtras/Qt3DWindow>

#include <Qt3DCore/QEntity>

#include <Qt3DRender/QCamera>
#include <Qt3DExtras/QFirstPersonCameraController>

#include <Qt3DRender/QMesh>
#include <Qt3DCore/QTransform>

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>


class WaterMaterial;

class Scene : public QWidget
{
    Q_OBJECT

public:
    explicit Scene(QWidget *parent = nullptr);
    ~Scene();

    void setModelY(float y);
    void setModelX(float x);
    void setModelZ(float z);

    bool loadOBJ(const QString &filePath);
    void clearModel();

private:
    void setupScene();
    void setupCamera();
    // void setupCameraController();
    void setupLight();
    void createWater();



    Qt3DExtras::Qt3DWindow *view;
    Qt3DCore::QEntity *rootEntity;

    Qt3DRender::QCamera *camera;
    Qt3DExtras::QFirstPersonCameraController *cameraController;

    Qt3DCore::QEntity *directLightEntity;
    Qt3DCore::QEntity *pointLightEntity;

    QString currentModelFile;
    Qt3DCore::QEntity *modelEntity;
    Qt3DCore::QTransform *modelTransform;
    Qt3DExtras::QPhongMaterial *modelMaterial;
    Qt3DRender::QMesh *objMesh;

    WaterMaterial *waterMaterial;

};

#endif // SCENE_H
