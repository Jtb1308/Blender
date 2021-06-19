uniform vec4 mat_vecs[4];
uniform float zheight;
uniform bool flag;

flat out vec4 finalColor;

const float PI = 3.141592653589;

const mat4 rot_mat = mat4(vec4(1.0, 0.0, 0.0, 0.0),
                          vec4(0.0, 0.0, -1.0, 0.0),
                          vec4(0.0, 1.0, 0.0, 0.0),
                          vec4(0.0, 0.0, 0.0, 1.0));

const int indices[36] = int[36](0,12,1,1,12,2,2,12,3,3,12,4,4,12,5,5,12,6,6,12,7,7,12,8,8,12,9,9,12,10,10,12,11,11,12,0);

void main(void)
{
    float height;
    if(flag){
        height = 0.0;
    }
    else{
        height = zheight;
    }
    mat4 mat = mat4(mat_vecs[0], mat_vecs[1], mat_vecs[2], mat_vecs[3]);
    vec3 color = vec3(0.2,0.1,1.0);
    vec4 p[13];
    p[0] = vec4(1.000000, 0.000000, height, 1.0);
    p[1] = vec4(0.866025, 0.500000, height, 1.0);
    p[2] = vec4(0.500000, 0.866025, height, 1.0);
    p[3] = vec4(0.000000, 1.000000, height, 1.0);
    p[4] = vec4(-0.500000, 0.866025, height, 1.0);
    p[5] = vec4(-0.866025, 0.500000, height, 1.0);
    p[6] = vec4(-1.000000, 0.000000, height, 1.0);
    p[7] = vec4(-0.866025, -0.500000, height, 1.0);
    p[8] = vec4(-0.500000, -0.866025, height, 1.0);
    p[9] = vec4(-0.000000, -1.000000, height, 1.0);
    p[10] = vec4(0.500000, -0.866025, height, 1.0);
    p[11] = vec4(0.866025, -0.500000, height, 1.0);
    p[12] = vec4(0.0, 0.0, height, 1.0);
    int ind = indices[gl_VertexID % 36];
    vec4 transform;
    if(flag){
        p[ind] = rot_mat * p[ind];
    }
    transform =  mat * p[ind];
    vec3 pos = transform.xyz;
    finalColor = vec4(color,1.0);
    vec3 world_pos = point_object_to_world(pos);
    gl_Position = point_world_to_ndc(world_pos);
}
