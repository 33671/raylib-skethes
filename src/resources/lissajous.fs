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
// Pentagram - exact   (https://www.shadertoy.com/view/t3X3z4)

float sdPentagram(in vec2 p, in float r )
{
    const float k1x = 0.809016994; // cos(π/ 5) = ¼(√5+1)
    const float k2x = 0.309016994; // sin(π/10) = ¼(√5-1)
    const float k1y = 0.587785252; // sin(π/ 5) = ¼√(10-2√5)
    const float k2y = 0.951056516; // cos(π/10) = ¼√(10+2√5)
    const float k1z = 0.726542528; // tan(π/ 5) = √(5-2√5)
    const vec2  v1  = vec2( k1x,-k1y);
    const vec2  v2  = vec2(-k1x,-k1y);
    const vec2  v3  = vec2( k2x,-k2y);
    
    p.x = abs(p.x);
    p -= 2.0*max(dot(v1,p),0.0)*v1;
    p -= 2.0*max(dot(v2,p),0.0)*v2;
    p.x = abs(p.x);
    p.y -= r;
    return length(p-v3*clamp(dot(p,v3),0.0,k1z*r))
           * sign(p.y*v3.x-p.x*v3.y);
}

// float map(vec2 p, float r, float t) {
//     return distance(1.0*vec2(noise(1.0*t), noise(t+2.0)), p) - r;
// }
float map(vec2 p, float r, float t) {
    return sdPentagram(1.0*vec2(noise(1.0*t), noise(t+1.0)) - p,r);
}
uniform vec2 u_resolution;
uniform float u_time;
void main()
{
    const int l = 500;
   
    vec2 uv = (gl_FragCoord.xy-u_resolution.xy*.5)/u_resolution.y;

    vec3 col =vec3(0.0);
    
    for(int i=0; i<l; i++) { 
        col += vec3(step(.0,-map(uv, 0.02, u_time-float(i)*1.0)));
    }


    gl_FragColor = vec4(col,1.0);
}