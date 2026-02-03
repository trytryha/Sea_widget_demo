#ifndef WATERMATERIAL_H
#define WATERMATERIAL_H

#include <Qt3DRender/QMaterial>

class WaterMaterial : public Qt3DRender::QMaterial
{
    Q_OBJECT
    Q_PROPERTY(float vertYPos READ vertYPos WRITE setVertYPos)
    Q_PROPERTY(float offsetX READ offsetX WRITE setOffsetX)
    Q_PROPERTY(float offsetY READ offsetY WRITE setOffsetY)

public:
    explicit WaterMaterial(Qt3DCore::QNode *parent = nullptr);

    // vertYPos 方法
    float vertYPos() const;
    void setVertYPos(float value);

    // offsetX 方法
    float offsetX() const;
    void setOffsetX(float value);

    // offsetY 方法
    float offsetY() const;
    void setOffsetY(float value);

private:
    // 成员变量
    float m_vertYpos;
    float m_offsetX;
    float m_offsetY;
};

#endif // WATERMATERIAL_H
