// #version 330
precision highp float;
uniform float u_time;
float random (float v) {
    return fract(sin(v * 12.9898) * 43758.5453);
}
uniform vec2 u_resolution;
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

void main()
{
    vec2 uv = (gl_FragCoord.xy-u_resolution.xy*.5)/u_resolution.y;
    vec2 perin_x = uv * 10.0;
    float time = u_time * 0.5;
    float y = noise(perin_x.x+time);

    float noise_curve = smoothstep(.01,.0,abs(y - uv.y));
    float stripe =  fract(perin_x.x+time);
    float aaWidth = 0.01; 
    float grid_line = smoothstep(0.0, 0.01, stripe) - smoothstep(0.01, 0.06, stripe);
    float y_axis = (1.0-step(0.002, abs(uv.y)));
    gl_FragColor  = vec4(vec3(max(max(noise_curve,grid_line),y_axis)),1.0);
}