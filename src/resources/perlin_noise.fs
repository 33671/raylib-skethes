#version 330

// 输入坐标
in vec2 fragCoord;
// 输出颜色
out vec4 finalColor;

// 统一变量
uniform float iTime;
uniform vec2 iResolution;

// 哈希函数
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

// 生成梯度向量
vec2 gradient(vec2 p) {
    float h = hash(p);
    float angle = 6.28318530718 * h;
    return vec2(cos(angle), sin(angle));
}

// Perlin噪声函数
float perlinNoise(vec2 pos) {
    vec2 i = floor(pos);
    vec2 f = fract(pos);
    
    // 计算四个角点的梯度
    vec2 g00 = gradient(i + vec2(0.0, 0.0));
    vec2 g10 = gradient(i + vec2(1.0, 0.0));
    vec2 g01 = gradient(i + vec2(0.0, 1.0));
    vec2 g11 = gradient(i + vec2(1.0, 1.0));
    
    // 计算距离向量
    vec2 d00 = f - vec2(0.0, 0.0);
    vec2 d10 = f - vec2(1.0, 0.0);
    vec2 d01 = f - vec2(0.0, 1.0);
    vec2 d11 = f - vec2(1.0, 1.0);
    
    // 计算点积
    float p00 = dot(g00, d00);
    float p10 = dot(g10, d10);
    float p01 = dot(g01, d01);
    float p11 = dot(g11, d11);
    
    // 缓和曲线
    vec2 u = f * f * (3.0 - 2.0 * f);
    
    // 双线性插值
    return mix(mix(p00, p10, u.x), mix(p01, p11, u.x), u.y);
}

void main() {

    // if (iTime < 0.5) { // 初始0.5秒显示坐标
    //     finalColor = vec4(fragCoord/iResolution, 0.0, 1.0);
    //     return;
    // }
    // 修正后的坐标转换
    vec2 uv = fragCoord / iResolution.y * 5.0;
    uv.x += iTime;
    
    // 调试步骤：显示原始坐标（取消注释以下两行）
    // finalColor = vec4(fragCoord/iResolution, 0.0, 1.0);
    // return;

    float noise = perlinNoise(uv);
    noise = (noise + 1.0) * 0.5;  // 映射到[0,1]
    
    finalColor = vec4(noise, noise, noise, 1.0);
}