#version 330 core
precision highp float;
uniform vec2 iResolution;
uniform float iTime;
// out vec4 FragColor;
in vec2 TexCoord;
#define u_resolution iResolution
float sdSphere(vec3 position_3d,float radius)
{
    return length(position_3d)-radius;
}
float sdTorus(vec3 p,vec2 t)
{
    vec2 q=vec2(length(p.xz)-t.x,p.y);
    return length(q)-t.y;
}
float sawtooth(float x, float period) {
    return 2.0 * (mod(x, period) / period) - 1.0;
}
mat3 rotationMatrix(vec3 axis,float angle){
    axis=normalize(axis);
    float s=sin(angle);
    float c=cos(angle);
    float oc=1.-c;
    
    return mat3(oc*axis.x*axis.x+c,oc*axis.x*axis.y-axis.z*s,oc*axis.z*axis.x+axis.y*s,
        oc*axis.x*axis.y+axis.z*s,oc*axis.y*axis.y+c,oc*axis.y*axis.z-axis.x*s,
    oc*axis.z*axis.x-axis.y*s,oc*axis.y*axis.z+axis.x*s,oc*axis.z*axis.z+c);
}
vec3 rotatePoint(vec3 p, vec3 axis, float theta) {
    float cost = cos(theta);
    float sint = sin(theta);
    return p * cost + cross(axis, p) * sint + axis * dot(axis, p) * (1.0 - cost);
}
float smin(float a,float b,float k)
{
    k*=1.;
    float r=exp2(-a/k)+exp2(-b/k);
    return-k*log2(r);
}
vec3 sdfOrbit(vec3 p, float time) {
    float orbitRadius = 1.0;  
    float orbitSpeed = 1.0;   
    float spinSpeed = 2.0;    

   
    float thetaOrbit = time * orbitSpeed;
    vec3 orbitCenter = vec3(cos(thetaOrbit) * orbitRadius, 0.0, sin(thetaOrbit) * orbitRadius);

    vec3 localP = p - orbitCenter;

    float thetaSpin = time * spinSpeed;
    vec3 rotatedP = rotatePoint(localP, vec3(0.0, 1.0, 0.0), thetaSpin);
    return rotatedP;
}
float sdf_scene(vec3 position_3d)
{
    float rot_angle = sawtooth(iTime,5.0) * 2.0 * 3.1415; 
    mat3 rot=rotationMatrix(vec3(1.0,1.6,2),rot_angle);
   
    float theta = 2.0* 3.1415 * sawtooth(iTime,5.0); 
    vec3 rotatedP = sdfOrbit(position_3d-vec3(1,1,1),iTime);
   
    float torus=sdTorus(rot*rotatedP,vec2(.4,.1));
    float sphere=sdSphere(position_3d-vec3(0,0,0),.5);
    return smin(torus,sphere,.3);

    // float sphere=sdSphere(position_3d-vec3(0,0,0),.5);
    // return sphere;
}
vec3 calcNormal(vec3 p) {
    const float eps = 0.001;
    return normalize(vec3(
        sdf_scene(p + vec3(eps,0,0)) - sdf_scene(p - vec3(eps,0,0)),
        sdf_scene(p + vec3(0,eps,0)) - sdf_scene(p - vec3(0,eps,0)),
        sdf_scene(p + vec3(0,0,eps)) - sdf_scene(p - vec3(0,0,eps))
    ));
}
vec3 shading(vec3 hit_point, vec3 nom, vec3 rd) {
    
    // float rot_angle = sawtooth(u_time,3.0) * 2.0 * 3.1415; 
    // mat3 rot=rotationMatrix(vec3(0,.6,2),rot_angle);
    vec3 lightPos = vec3(2.0, 5.0, 3.0);
    vec3 lightDir = normalize(lightPos - hit_point);
    
    // 漫反射
    float diff = max(dot(nom, lightDir), .0);
    vec3 diffuse = diff * vec3(0.0902, 0.4627, 0.4549);  // 橙色材质
    
    // 环境光
    vec3 ambient = vec3(0.2);
    
    // 镜面反射
    vec3 reflectDir = reflect(lightDir, nom);
    float spec = pow(max(dot(rd, reflectDir), 0.0), 50.0);
    vec3 specular = 0.5 * spec * vec3(1.0);
    
    return ambient + diffuse + specular;
}
vec3 background(vec3 rd) {
    float t = 0.5*(rd.y + 1.0);
    // return mix(vec3(1.0), vec3(0.5, 0.7, 1.0), t);
    return vec3(0.0);
}
vec3 camera_rd(vec2 position_xy,vec3 right_nom,vec3 up_nom,vec3 forward)
{
    float zoom=1.0;
    return normalize(position_xy.x*right_nom+position_xy.y*up_nom+zoom*forward);
}

vec3 set_camera(vec2 position_xy,vec3 cam_postion,vec3 object_position){
    vec3 forward=normalize(object_position-cam_postion);
    vec3 right=normalize(cross(vec3(0, 1, 0),forward));
    vec3 up=normalize(cross(forward,right));
    return camera_rd(position_xy,right,up,forward);
}
float ray_march(vec3 ro,vec3 rd_nom)
{
    vec3 next_point=ro;
    float total_d=0.;
    for(int i=0;i<1000;i++)
    {
        float new_d=sdf_scene(next_point);
        if(new_d<.001){
            return total_d;
        }
        next_point=next_point+(rd_nom*new_d);
        total_d+=new_d;
        if(total_d>100.){
            return 2e10;
        }
    }
    return 2e10;
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K=vec4(1.,2./3.,1./3.,3.);
    vec3 p=abs(fract(c.xxx+K.xyz)*6.-K.www);
    return c.z*mix(K.xxx,clamp(p-K.xxx,0.,1.),c.y);
}

void main()
{
    float aspect=u_resolution.x/u_resolution.y;
    vec2 position=gl_FragCoord.xy/u_resolution;
    position.x=(position.x-.5)*aspect+.5;
    vec3 cam=vec3(3,2,2);
    vec3 rd=set_camera(position-vec2(.5),cam,vec3(0,0,0.5));
    float d = ray_march(cam,rd);
    vec3 hitpoint = cam + rd * d;
    vec3 color = shading(hitpoint,calcNormal(hitpoint),rd);
    if (d > 200.0){
        color = background(rd);
    }
    // vec3 rgb = pow(color, vec3(1.0/2.2)); // 近似伽马校正
    // vec3 color=hsv2rgb(vec3(d/.1));
    gl_FragColor=vec4(color,1);
}