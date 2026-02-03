#version 150 core

in vec3 worldPosition;
in vec3 worldNormal;
in vec4 worldTangent;
in vec2 texCoord;
in vec2 waveTexCoord;
in vec2 movtexCoord;
in vec2 multexCoord;
in vec2 skyTexCoord;

in vec3 vpos;
in vec3 color;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;
uniform sampler2D waveTexture;
uniform sampler2D skyTexture;
uniform sampler2D foamTexture;

uniform float offsetx;
uniform float offsety;
uniform float specularity;
uniform float waveStrenght;
uniform vec4 ka;
uniform vec3 specularColor;
uniform float shininess;
uniform float normalAmount;
uniform vec3 eyePosition;

out vec4 fragColor;

// === 原 coordinatesystems.inc 的内容 ===
// 函数：计算从世界空间到切线空间的转换矩阵
mat3 calcWorldSpaceToTangentSpaceMatrix(vec3 normal, vec4 tangent)
{
    // 1. 确保输入向量是归一化的
    vec3 n = normalize(normal);
    vec3 t = normalize(tangent.xyz);
    // 2. 计算副切线 (binormal)，叉乘顺序和 tangent.w 决定了副法线方向
    vec3 b = tangent.w * cross(n, t);
    // 3. 构造切线空间矩阵 (TBN)，其转置即为世界空间到切线空间的矩阵
    mat3 TBN = mat3(t, b, n);
    return transpose(TBN);
}
// === 原 phong.inc.frag 的内容 ===
// 函数：Phong光照模型计算
vec4 phongFunction(vec4 ka, vec4 diffuse, vec4 specular, float shininess,
                   vec3 worldPosition, vec3 worldView, vec3 worldNormal)
{
    // 定义光源参数（您可以根据需要将其设为uniform变量）
    vec3 lightPos = vec3(100.0, 100.0, 100.0); // 光源在世界空间的位置
    vec3 lightColor = vec3(1.0, 1.0, 1.0);     // 光源颜色（白色）

    // 1. 环境光 (Ambient) 成分
    vec3 ambient = ka.rgb * diffuse.rgb * 0.2;

    // 2. 计算漫反射 (Diffuse) 成分
    vec3 lightDir = normalize(lightPos - worldPosition); // 光线方向
    float diff = max(dot(worldNormal, lightDir), 0.0);   // 兰伯特因子
    vec3 diffuseResult = diff * lightColor * diffuse.rgb;

    // 3. 计算镜面高光 (Specular) 成分
    vec3 reflectDir = reflect(-lightDir, worldNormal);          // 反射光方向
    float spec = pow(max(dot(worldView, reflectDir), 0.0), shininess); // Phong高光系数
    vec3 specularResult = spec * lightColor * specular.rgb * specular.a;

    // 4. 合并三种光照成分并返回（Alpha通道来自漫反射）
    return vec4(ambient + diffuseResult + specularResult, diffuse.a);
}
// === 您的原始主函数从这里开始 ===
void main()
{
    // Move waveTexCoords
    vec2 waveMovCoord = waveTexCoord;
    waveMovCoord.x += offsetx;
    waveMovCoord.y -= offsety;
    vec4 wave = texture(waveTexture, waveMovCoord);

    //Wiggle the newCoord by r and b colors of waveTexture
    vec2 newCoord = texCoord;
    newCoord.x += wave.r * waveStrenght;
    newCoord.y -= wave.b * waveStrenght;

    // Sample the textures at the interpolated texCoords
    // Use default texCoord for diffuse (it does not move on x or y, so it can be used as "ground under the water").
    vec4 diffuseTextureColor = texture(diffuseTexture, texCoord);
    // 2 Animated Layers of specularTexture mixed with the newCoord
    vec4 specularTextureColor = texture( specularTexture, multexCoord+newCoord) + (texture( specularTexture, movtexCoord+newCoord ));
    // 2 Animated Layers of normalTexture mixed with the newCoord
    vec3 tNormal = normalAmount * texture( normalTexture, movtexCoord+newCoord ).rgb - vec3( 1.0 )+(normalAmount * texture( normalTexture, multexCoord+newCoord ).rgb - vec3( 1.0 ));
    // Animated skyTexture layer
    vec4 skycolor = texture(skyTexture, skyTexCoord);
    skycolor = skycolor * 0.4;
    //Animated foamTexture layer
    vec4 foamTextureColor = texture(foamTexture, texCoord);

    mat3 tangentMatrix = calcWorldSpaceToTangentSpaceMatrix(worldNormal, worldTangent);
    mat3 invertTangentMatrix = transpose(tangentMatrix);

    vec3 wNormal = normalize(invertTangentMatrix * tNormal);
    vec3 worldView = normalize(eyePosition - worldPosition);

    vec4 diffuse = vec4(diffuseTextureColor.rgb, vpos.y);
    vec4 specular = vec4(specularTextureColor.a*specularity);
    vec4 outputColor = phongFunction(ka, diffuse, specular, shininess, worldPosition, worldView, wNormal);

    outputColor += vec4(skycolor.rgb, vpos.y);
    outputColor += (foamTextureColor.rgba*vpos.y);

    fragColor = vec4(outputColor.rgb,1.0);
}
// #version 150 core

// in vec2 texCoord;
// uniform sampler2D diffuseTexture;
// out vec4 fragColor;

// void main()
// {
//     fragColor = texture(diffuseTexture, texCoord);
// }

// #version 150 core

// uniform vec3 maincolor;
// out vec4 fragColor;

// void main()
// {
//     //output color from material
//     fragColor = vec4(maincolor,1.0);
// }

// #version 150 core

// in vec3 worldPosition;
// in vec3 worldNormal;
// in vec4 worldTangent;
// in vec2 texCoord;
// in vec2 waveTexCoord;
// in vec2 movtexCoord;
// in vec2 multexCoord;
// in vec2 skyTexCoord;

// in vec3 vpos;

// in vec3 color;

// uniform sampler2D diffuseTexture;
// uniform sampler2D specularTexture;
// uniform sampler2D normalTexture;
// uniform sampler2D waveTexture;
// uniform sampler2D skyTexture;
// uniform sampler2D foamTexture;

// uniform float offsetx;
// uniform float offsety;
// uniform float specularity;
// uniform float waveStrenght;
// uniform vec4 ka;
// uniform vec3 specularColor;
// uniform float shininess;
// uniform float normalAmount;
// uniform vec3 eyePosition;

// out vec4 fragColor;

// #pragma include phong.inc.frag
// #pragma include coordinatesystems.inc

// void main()
// {
//     // Move waveTexCoords
//     vec2 waveMovCoord = waveTexCoord;
//     waveMovCoord.x += offsetx;
//     waveMovCoord.y -= offsety;
//     vec4 wave = texture(waveTexture, waveMovCoord);

//     //Wiggle the newCoord by r and b colors of waveTexture
//     vec2 newCoord = texCoord;
//     newCoord.x += wave.r * waveStrenght;
//     newCoord.y -= wave.b * waveStrenght;

//     // Sample the textures at the interpolated texCoords
//     // Use default texCoord for diffuse (it does not move on x or y, so it can be used as "ground under the water").
//     vec4 diffuseTextureColor = texture(diffuseTexture, texCoord);
//     // 2 Animated Layers of specularTexture mixed with the newCoord
//     vec4 specularTextureColor = texture( specularTexture, multexCoord+newCoord) + (texture( specularTexture, movtexCoord+newCoord ));
//     // 2 Animated Layers of normalTexture mixed with the newCoord
//     vec3 tNormal = normalAmount * texture( normalTexture, movtexCoord+newCoord ).rgb - vec3( 1.0 )+(normalAmount * texture( normalTexture, multexCoord+newCoord ).rgb - vec3( 1.0 ));
//     // Animated skyTexture layer
//     vec4 skycolor = texture(skyTexture, skyTexCoord);
//     skycolor = skycolor * 0.4;
//     //Animated foamTexture layer
//     vec4 foamTextureColor = texture(foamTexture, texCoord);

//     mat3 tangentMatrix = calcWorldSpaceToTangentSpaceMatrix(worldNormal, worldTangent);
//     mat3 invertTangentMatrix = transpose(tangentMatrix);

//     vec3 wNormal = normalize(invertTangentMatrix * tNormal);
//     vec3 worldView = normalize(eyePosition - worldPosition);

//     vec4 diffuse = vec4(diffuseTextureColor.rgb, vpos.y);
//     vec4 specular = vec4(specularTextureColor.a*specularity);
//     vec4 outputColor = phongFunction(ka, diffuse, specular, shininess, worldPosition, worldView, wNormal);

//     outputColor += vec4(skycolor.rgb, vpos.y);
//     outputColor += (foamTextureColor.rgba*vpos.y);

//     fragColor = vec4(outputColor.rgb,1.0);
// }

