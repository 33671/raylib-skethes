precision highp float;
// float map(vec2 p, float r, float t) {
//     return distance(0.4*vec2(cos(3.0*t), sin(1.0*t)), p) - r;
// }
float random (float v) {
    return fract(sin(v * 12.9898) * 43758.5453);
}
// uniform vec2 u_resolution;
// 6t^5 - 15t^4 + 10t^3
float fade(float t) {
    return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float noise(float x)
{
    float i = floor(x);
    float x_fract = fract(x);
    float g0 = 2.0*random(i) - 1.0; //[-1,1]
    float g1 = 2.0*random(i+1.0) - 1.0; //[-1,1]
    float d0 = x_fract; // [0,1]
    float d1 = x_fract - 1.0; // [-1,0]
    float a = g0 * d0;
    float b = g1 * d1; 
    float t = fade(x_fract); // [0,1]
    return lerp(a,b,t);
}
float map(vec2 p, float r, float t) {
    return distance(1.0*vec2(noise(1.0*t), noise(t+1.0)), p) - r;
}
uniform vec2 u_resolution;
uniform float u_time;
void main()
{
    const int l = 500;
   
    vec2 uv = (gl_FragCoord.xy-u_resolution.xy*.5)/u_resolution.y;

    vec3 col =vec3(0.0);
    
    for(int i=0; i<l; i++) { 
        col += vec3(step(.0,-map(uv, 0.004, u_time-float(i)*0.5)));
    }


    gl_FragColor = vec4(col,1.0);
}