#include "WaterMaterial.h"
#include <QMatrix4x4>
#include <QFile>
#include <QDebug>
#include <QUrl>

#include <Qt3DRender/QParameter>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureImage>
#include <Qt3DRender/QTextureWrapMode>

WaterMaterial::WaterMaterial(Qt3DCore::QNode *parent)
    : Qt3DRender::QMaterial(parent)
    , m_vertYpos(2.0f)  // 设置默认值为2
    , m_offsetX(0.0f)    // offsetx 默认值
    , m_offsetY(0.0f)    // offsety 默认值
{
    // 1. 创建参数
    Qt3DRender::QParameter *colorParam = new Qt3DRender::QParameter("maincolor", QVector3D(1.0f, 0.0f, 0.0f));
    addParameter(colorParam);

    // ka
    Qt3DRender::QParameter *kaParam = new Qt3DRender::QParameter("ka", QVector3D(0.15f, 0.35f, 0.50f));
    addParameter(kaParam);

    // specularColor - 高光颜色
    Qt3DRender::QParameter *specularColorParam = new Qt3DRender::QParameter("specularColor", QVector3D(0.2f, 0.2f, 0.2f));
    addParameter(specularColorParam);

    // shininess - 高光指数
    Qt3DRender::QParameter *shininessParam = new Qt3DRender::QParameter("shininess", 100.0f);
    addParameter(shininessParam);

    // texCoordScale - 纹理坐标缩放
    Qt3DRender::QParameter *texCoordScaleParam = new Qt3DRender::QParameter("texCoordScale", 1.0f);
    addParameter(texCoordScaleParam);

    // offsetx - X轴偏移
    Qt3DRender::QParameter *offsetxParam = new Qt3DRender::QParameter("offsetx", m_offsetX);
    addParameter(offsetxParam);

    // offsety - Y轴偏移
    Qt3DRender::QParameter *offsetyParam = new Qt3DRender::QParameter("offsety", m_offsetY);
    addParameter(offsetyParam);

    // vertYpos - 垂直位置/波浪缩放
    // 使用成员变量m_vertYpos的初始值
    Qt3DRender::QParameter *vertYposParam = new Qt3DRender::QParameter("vertYpos", m_vertYpos);
    addParameter(vertYposParam);

    // specularity - 高光强度
    Qt3DRender::QParameter *specularityParam = new Qt3DRender::QParameter("specularity", 0.75f);
    addParameter(specularityParam);

    // waveheight - 波浪高度
    Qt3DRender::QParameter *waveheightParam = new Qt3DRender::QParameter("waveheight", 0.2f);
    addParameter(waveheightParam);

    // waveStrenght - 波浪强度
    Qt3DRender::QParameter *waveStrenghtParam = new Qt3DRender::QParameter("waveStrenght", 0.015f);
    addParameter(waveStrenghtParam);

    // waveRandom - 波浪随机性
    Qt3DRender::QParameter *waveRandomParam = new Qt3DRender::QParameter("waveRandom", 1.0f);
    addParameter(waveRandomParam);

    // normalAmount - 法线贴图强度
    Qt3DRender::QParameter *normalAmountParam = new Qt3DRender::QParameter("normalAmount", 2.2f);
    addParameter(normalAmountParam);

    // ... (以下是您原有的纹理创建代码，保持不变)
    // 泡沫纹理
    Qt3DRender::QTexture2D *foamTexture = new Qt3DRender::QTexture2D();
    foamTexture->setMinificationFilter(Qt3DRender::QTexture2D::LinearMipMapLinear);
    foamTexture->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);

    Qt3DRender::QTextureWrapMode foamWrapMode;
    foamWrapMode.setX(Qt3DRender::QTextureWrapMode::Repeat);
    foamWrapMode.setY(Qt3DRender::QTextureWrapMode::Repeat);
    foamTexture->setWrapMode(foamWrapMode);

    foamTexture->setGenerateMipMaps(true);
    foamTexture->setMaximumAnisotropy(16.0f);

    Qt3DRender::QTextureImage *foamTextureImage = new Qt3DRender::QTextureImage();
    // 设置默认纹理路径，实际使用时可以根据需要修改
    foamTextureImage->setSource(QUrl("qrc:/textures/foam.jpg"));
    foamTexture->addTextureImage(foamTextureImage);

    Qt3DRender::QParameter *foamTextureParam = new Qt3DRender::QParameter("foamTexture", QVariant::fromValue(foamTexture));
    addParameter(foamTextureParam);

    // 天空纹理参数
    Qt3DRender::QTexture2D *skyTexture = new Qt3DRender::QTexture2D();
    skyTexture->setMinificationFilter(Qt3DRender::QTexture2D::LinearMipMapLinear);
    skyTexture->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);

    Qt3DRender::QTextureWrapMode skyWrapMode;
    skyWrapMode.setX(Qt3DRender::QTextureWrapMode::Repeat);
    skyWrapMode.setY(Qt3DRender::QTextureWrapMode::Repeat);
    skyTexture->setWrapMode(skyWrapMode);

    skyTexture->setGenerateMipMaps(true);
    skyTexture->setMaximumAnisotropy(16.0f);

    Qt3DRender::QTextureImage *skyTextureImage = new Qt3DRender::QTextureImage();
    skyTextureImage->setSource(QUrl("qrc:/textures/sky.jpg"));
    skyTexture->addTextureImage(skyTextureImage);

    Qt3DRender::QParameter *skyTextureParam = new Qt3DRender::QParameter("skyTexture", QVariant::fromValue(skyTexture));
    addParameter(skyTextureParam);

    // 波浪纹理参数
    Qt3DRender::QTexture2D *waveTexture = new Qt3DRender::QTexture2D();
    waveTexture->setMinificationFilter(Qt3DRender::QTexture2D::LinearMipMapLinear);
    waveTexture->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);

    Qt3DRender::QTextureWrapMode waveWrapMode;
    waveWrapMode.setX(Qt3DRender::QTextureWrapMode::Repeat);
    waveWrapMode.setY(Qt3DRender::QTextureWrapMode::Repeat);
    waveTexture->setWrapMode(waveWrapMode);

    waveTexture->setGenerateMipMaps(true);
    waveTexture->setMaximumAnisotropy(16.0f);

    Qt3DRender::QTextureImage *waveTextureImage = new Qt3DRender::QTextureImage();
    waveTextureImage->setSource(QUrl("qrc:/textures/Waterwave.jpg"));
    waveTexture->addTextureImage(waveTextureImage);

    Qt3DRender::QParameter *waveTextureParam = new Qt3DRender::QParameter("waveTexture", QVariant::fromValue(waveTexture));
    addParameter(waveTextureParam);

    // 高光纹理参数
    Qt3DRender::QTexture2D *specularTexture = new Qt3DRender::QTexture2D();
    specularTexture->setMinificationFilter(Qt3DRender::QTexture2D::LinearMipMapLinear);
    specularTexture->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);

    Qt3DRender::QTextureWrapMode specularWrapMode;
    specularWrapMode.setX(Qt3DRender::QTextureWrapMode::Repeat);
    specularWrapMode.setY(Qt3DRender::QTextureWrapMode::Repeat);
    specularTexture->setWrapMode(specularWrapMode);

    specularTexture->setGenerateMipMaps(true);
    specularTexture->setMaximumAnisotropy(16.0f);

    Qt3DRender::QTextureImage *specularTextureImage = new Qt3DRender::QTextureImage();
    specularTextureImage->setSource(QUrl("qrc:/textures/WaterSpecular.jpg"));
    specularTexture->addTextureImage(specularTextureImage);

    Qt3DRender::QParameter *specularTextureParam = new Qt3DRender::QParameter("specularTexture", QVariant::fromValue(specularTexture));
    addParameter(specularTextureParam);

    // 漫反射纹理参数
    Qt3DRender::QTexture2D *diffuseTexture = new Qt3DRender::QTexture2D();
    diffuseTexture->setMinificationFilter(Qt3DRender::QTexture2D::LinearMipMapLinear);
    diffuseTexture->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);

    Qt3DRender::QTextureWrapMode diffuseWrapMode;
    diffuseWrapMode.setX(Qt3DRender::QTextureWrapMode::Repeat);
    diffuseWrapMode.setY(Qt3DRender::QTextureWrapMode::Repeat);
    diffuseTexture->setWrapMode(diffuseWrapMode);

    diffuseTexture->setGenerateMipMaps(true);
    diffuseTexture->setMaximumAnisotropy(16.0f);

    Qt3DRender::QTextureImage *diffuseTextureImage = new Qt3DRender::QTextureImage();
    diffuseTextureImage->setSource(QUrl("qrc:/textures/WaterDiffuse.jpg"));
    diffuseTexture->addTextureImage(diffuseTextureImage);

    Qt3DRender::QParameter *diffuseTextureParam = new Qt3DRender::QParameter("diffuseTexture", QVariant::fromValue(diffuseTexture));
    addParameter(diffuseTextureParam);

    // 法线纹理参数
    Qt3DRender::QTexture2D *normalTexture = new Qt3DRender::QTexture2D();
    normalTexture->setMinificationFilter(Qt3DRender::QTexture2D::LinearMipMapLinear);
    normalTexture->setMagnificationFilter(Qt3DRender::QTexture2D::Linear);

    Qt3DRender::QTextureWrapMode normalWrapMode;
    normalWrapMode.setX(Qt3DRender::QTextureWrapMode::Repeat);
    normalWrapMode.setY(Qt3DRender::QTextureWrapMode::Repeat);
    normalTexture->setWrapMode(normalWrapMode);

    normalTexture->setGenerateMipMaps(true);
    normalTexture->setMaximumAnisotropy(16.0f);

    Qt3DRender::QTextureImage *normalTextureImage = new Qt3DRender::QTextureImage();
    normalTextureImage->setSource(QUrl("qrc:/textures/WaterNormal.jpg"));
    normalTexture->addTextureImage(normalTextureImage);

    Qt3DRender::QParameter *normalTextureParam = new Qt3DRender::QParameter("normalTexture", QVariant::fromValue(normalTexture));
    addParameter(normalTextureParam);

    // 2. 创建着色器程序
    Qt3DRender::QShaderProgram *shader = new Qt3DRender::QShaderProgram();
    shader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl("qrc:/shaders/SimpleWater.vert")));
    shader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl("qrc:/shaders/SimpleWater.frag")));

    // 3. 创建渲染通道
    Qt3DRender::QRenderPass *renderPass = new Qt3DRender::QRenderPass();
    renderPass->setShaderProgram(shader);

    // 4. 创建过滤键
    Qt3DRender::QFilterKey *filterKey = new Qt3DRender::QFilterKey();
    filterKey->setName("renderingStyle");
    filterKey->setValue("forward");

    // 5. 创建技术
    Qt3DRender::QTechnique *technique = new Qt3DRender::QTechnique();
    Qt3DRender::QGraphicsApiFilter *apiFilter = technique->graphicsApiFilter();
    apiFilter->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    apiFilter->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
    apiFilter->setMajorVersion(3);
    apiFilter->setMinorVersion(2);

    technique->addFilterKey(filterKey);
    technique->addRenderPass(renderPass);

    // 6. 创建效果
    Qt3DRender::QEffect *effect = new Qt3DRender::QEffect();
    effect->addTechnique(technique);

    // 7. 设置材质效果
    setEffect(effect);
}

float WaterMaterial::vertYPos() const
{
    // 直接返回成员变量的值
    return m_vertYpos;
}

void WaterMaterial::setVertYPos(float value)
{
    // 检查值是否真的发生了变化
    if (!qFuzzyCompare(m_vertYpos, value)) {
        // 更新成员变量
        m_vertYpos = value;

        // 查找并更新对应的QParameter
        QVector<Qt3DRender::QParameter *> params = parameters();
        for (Qt3DRender::QParameter *param : params) {
            if (param->name() == QLatin1String("vertYpos")) {
                param->setValue(value);
                break; // 找到并更新后跳出循环
            }
        }

        // 注意：这里没有发射信号，因为我们没有添加NOTIFY信号
        // 如果需要信号通知，可以添加信号并在这里emit
    }
}

// ========== offsetX 方法 ==========
float WaterMaterial::offsetX() const
{
    return m_offsetX;
}

void WaterMaterial::setOffsetX(float value)
{
    if (!qFuzzyCompare(m_offsetX, value)) {
        m_offsetX = value;
        QVector<Qt3DRender::QParameter *> params = parameters();
        for (Qt3DRender::QParameter *param : params) {
            if (param->name() == QLatin1String("offsetx")) {
                param->setValue(value);
                break;
            }
        }
    }
}

// ========== offsetY 方法 ==========
float WaterMaterial::offsetY() const
{
    return m_offsetY;
}

void WaterMaterial::setOffsetY(float value)
{
    if (!qFuzzyCompare(m_offsetY, value)) {
        m_offsetY = value;
        QVector<Qt3DRender::QParameter *> params = parameters();
        for (Qt3DRender::QParameter *param : params) {
            if (param->name() == QLatin1String("offsety")) {
                param->setValue(value);
                break;
            }
        }
    }
}
