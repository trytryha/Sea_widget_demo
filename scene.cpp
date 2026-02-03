#include "Scene.h"
#include "WaterMaterial.h"


#include <Qt3DRender/QDirectionalLight>
#include <Qt3DRender/QPointLight>

#include <Qt3DExtras/QForwardRenderer>

#include <QVBoxLayout>

#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QParallelAnimationGroup>

#include <QDebug>
#include <QFile>

Scene::Scene(QWidget *parent)
    : QWidget(parent)
    , view(nullptr)
    , rootEntity(nullptr)
    , camera(nullptr)
    , cameraController(nullptr)
    , directLightEntity(nullptr)
    , pointLightEntity(nullptr)
    , modelEntity(nullptr)
    , modelTransform(nullptr)
    , modelMaterial(nullptr)
    , objMesh(nullptr)
    , waterMaterial(nullptr)

{
    resize(1400, 800);

    view = new Qt3DExtras::Qt3DWindow();

    view->defaultFrameGraph()->setClearColor(Qt::white);

    // 包装Qt3DWindow到Qwidget
    QWidget *container = QWidget::createWindowContainer(view, this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(container);

    setupScene();
}

Scene::~Scene()
{
}

void Scene::setupScene()
{
    rootEntity = new Qt3DCore::QEntity();

    setupCamera();
    setupLight();
    //setupSphere();
    //loadOBJ(":/models/toyplane.obj");
    createWater();

    if (waterMaterial) {
        // 创建顺序动画组
        QSequentialAnimationGroup *waveAnimationGroup = new QSequentialAnimationGroup(rootEntity);

        // 第一阶段：从当前值（默认200）到200
        QPropertyAnimation *toHighAnimation = new QPropertyAnimation(waterMaterial, "vertYPos");
        toHighAnimation->setDuration(200000);  // 200秒
        toHighAnimation->setEndValue(200.0f);
        toHighAnimation->setEasingCurve(QEasingCurve::Linear);

        // 第二阶段：从200到2
        QPropertyAnimation *toLowAnimation = new QPropertyAnimation(waterMaterial, "vertYPos");
        toLowAnimation->setDuration(200000);  // 200秒
        toLowAnimation->setEndValue(2.0f);
        toLowAnimation->setEasingCurve(QEasingCurve::Linear);

        // 将动画添加到序列
        waveAnimationGroup->addAnimation(toHighAnimation);
        waveAnimationGroup->addAnimation(toLowAnimation);

        // 设置无限循环
        waveAnimationGroup->setLoopCount(-1);  // -1表示无限循环

        // 启动动画
        waveAnimationGroup->start();
    }

    // ==================== 添加 offsetX/offsetY 动画 ====================
    if (waterMaterial) {
        // 创建顺序动画组
        QSequentialAnimationGroup *offsetAnimation = new QSequentialAnimationGroup(rootEntity);
        offsetAnimation->setLoopCount(-1);  // 无限循环

        // 第一阶段：并行动画，offsetX和offsetY从0到10，持续100秒
        QParallelAnimationGroup *parallelAnim1 = new QParallelAnimationGroup(offsetAnimation);

        // offsetX 动画：从当前值(0)到10
        QPropertyAnimation *offsetXAnim1 = new QPropertyAnimation(waterMaterial, "offsetX");
        offsetXAnim1->setDuration(100000);  // 100秒
        offsetXAnim1->setEndValue(10.0f);
        offsetXAnim1->setEasingCurve(QEasingCurve::Linear);

        // offsetY 动画：从当前值(0)到10
        QPropertyAnimation *offsetYAnim1 = new QPropertyAnimation(waterMaterial, "offsetY");
        offsetYAnim1->setDuration(100000);  // 100秒
        offsetYAnim1->setEndValue(10.0f);
        offsetYAnim1->setEasingCurve(QEasingCurve::Linear);

        // 将动画添加到第一个并行组
        parallelAnim1->addAnimation(offsetXAnim1);
        parallelAnim1->addAnimation(offsetYAnim1);

        // 第二阶段：并行动画，瞬间重置为0（duration设为0）
        QParallelAnimationGroup *parallelAnim2 = new QParallelAnimationGroup(offsetAnimation);

        // offsetX 瞬间重置为0
        QPropertyAnimation *offsetXAnim2 = new QPropertyAnimation(waterMaterial, "offsetX");
        offsetXAnim2->setDuration(0);  // 0毫秒，瞬间完成
        offsetXAnim2->setEndValue(0.0f);

        // offsetY 瞬间重置为0
        QPropertyAnimation *offsetYAnim2 = new QPropertyAnimation(waterMaterial, "offsetY");
        offsetYAnim2->setDuration(0);  // 0毫秒，瞬间完成
        offsetYAnim2->setEndValue(0.0f);

        // 将动画添加到第二个并行组
        parallelAnim2->addAnimation(offsetXAnim2);
        parallelAnim2->addAnimation(offsetYAnim2);

        // 将两个并行动画组添加到顺序动画组
        offsetAnimation->addAnimation(parallelAnim1);
        offsetAnimation->addAnimation(parallelAnim2);

        // 启动 offsetX/offsetY 动画
        offsetAnimation->start();
        qDebug() << "Offset X/Y animation started";
    }

    view->setRootEntity(rootEntity);
}

void Scene::setupCamera()
{
    camera = view->camera();
    camera->setProjectionType(Qt3DRender::QCameraLens::PerspectiveProjection);
    camera->setFieldOfView(45.0f);
    camera->setNearPlane(0.1f);
    camera->setFarPlane(1000.0f);
    camera->setPosition(QVector3D(0.0f, 4.0f, 15.0f));
    camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    camera->setViewCenter(QVector3D(0.0f, -2.0f, 0.0f));

    cameraController = new Qt3DExtras::QFirstPersonCameraController(rootEntity);
    cameraController->setCamera(camera);
}

void Scene::setupLight()
{
    directLightEntity = new Qt3DCore::QEntity(rootEntity);

    Qt3DRender::QDirectionalLight *directionalLight =
        new Qt3DRender::QDirectionalLight(directLightEntity);
    QVector3D dir(0.3f, -1.0f, 5.0f);
    dir.normalize();
    directionalLight->setWorldDirection(dir);
    directionalLight->setColor(QColor(255, 242, 163));  // #fff2a3
    directionalLight->setIntensity(1.0f);

    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(directLightEntity);
    lightTransform->setTranslation(QVector3D(0.0f, 50.0f, 60.0f));

    directLightEntity->addComponent(directionalLight);
    directLightEntity->addComponent(lightTransform);

    pointLightEntity = new Qt3DCore::QEntity(rootEntity);

    Qt3DRender::QPointLight *pointLight =
        new Qt3DRender::QPointLight(pointLightEntity);

    pointLight->setColor(QColor(255, 242, 163));
    Qt3DCore::QTransform *pointLightTransform = new Qt3DCore::QTransform(pointLightEntity);
    pointLightTransform->setTranslation(QVector3D(0.0f, 4.0f, 15.0f));

    pointLightEntity->addComponent(pointLight);
    pointLightEntity->addComponent(pointLightTransform);
}

bool Scene::loadOBJ(const QString &filePath){
    if(!QFile::exists(filePath)){
        qDebug() << "文件不存在:" << filePath;
        return false;
    }

    clearModel();

    modelEntity = new Qt3DCore::QEntity(rootEntity);

    objMesh = new Qt3DRender::QMesh(modelEntity);
    objMesh->setSource(QUrl::fromLocalFile(filePath));

    connect(objMesh, &Qt3DRender::QMesh::statusChanged, this, [filePath](Qt3DRender::QMesh::Status status) {
        switch (status) {
        case Qt3DRender::QMesh::None:
            qDebug() << "模型状态: 未加";
            break;
        case Qt3DRender::QMesh::Loading:
            qDebug() << "模型状态: 加载中...";
            break;
        case Qt3DRender::QMesh::Ready:
            qDebug() << "模型状态: 加载完成";
            // 自动调整相机位置
            break;
        case Qt3DRender::QMesh::Error:
            qDebug() << "模型状态: 加载错误";
            break;
        }
    });

    modelMaterial = new Qt3DExtras::QPhongMaterial(modelEntity);
    modelMaterial->setDiffuse(Qt::red);
    modelMaterial->setAmbient(QColor(100, 0, 0));
    modelMaterial->setSpecular(Qt::white);
    modelMaterial->setShininess(50.0f);

    modelTransform = new Qt3DCore::QTransform(modelEntity);
    modelTransform->setTranslation(QVector3D(1.0f, 0.0f, 2.0f));
    modelTransform->setScale(0.1f);

    modelEntity->addComponent(objMesh);
    modelEntity->addComponent(modelMaterial);
    modelEntity->addComponent(modelTransform);

    currentModelFile = filePath;

    qDebug() << "OBJ模型已加载:" << filePath;
    return true;
}

void Scene::clearModel()
{
    if (modelEntity) {
        // 从场景中移除实体
        modelEntity->setParent(static_cast<Qt3DCore::QNode*>(nullptr));

        // 删除实体及其组件
        delete modelEntity;
        modelEntity = nullptr;
        modelTransform = nullptr;
        modelMaterial = nullptr;
        objMesh = nullptr;
    }

    currentModelFile.clear();
}


void Scene::createWater(){

    Qt3DCore::QEntity *waterEntity = new Qt3DCore::QEntity(rootEntity);

    Qt3DRender::QMesh *waterMesh = new Qt3DRender::QMesh;
    waterMesh->setSource(QUrl("qrc:/models/waterPlane.obj"));

    Qt3DCore::QTransform *waterTransform = new Qt3DCore::QTransform;
    waterTransform->setTranslation(QVector3D(0.0f, 0.0f, 0.0f));
    waterTransform->setScale(2.0f);

    // 创建 WaterMaterial
    waterMaterial = new WaterMaterial;

    // 将组件添加到水面实体
    waterEntity->addComponent(waterMesh);
    waterEntity->addComponent(waterTransform);
    waterEntity->addComponent(waterMaterial);
}

void Scene::setModelY(float y)
{
    if(!modelTransform) return;

    QVector3D currentPos = modelTransform->translation();
    currentPos.setY(y);
    modelTransform->setTranslation(currentPos);

}

void Scene::setModelX(float x)
{
    if(!modelTransform) return;

    QVector3D currentPos = modelTransform->translation();
    currentPos.setX(x);
    modelTransform->setTranslation(currentPos);

}

void Scene::setModelZ(float z)
{
    if(!modelTransform) return;

    QVector3D currentPos = modelTransform->translation();
    currentPos.setZ(z);
    modelTransform->setTranslation(currentPos);

}
