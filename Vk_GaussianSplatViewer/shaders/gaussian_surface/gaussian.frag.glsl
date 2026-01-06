#version 460
#extension GL_ARB_separate_shader_objects : enable
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_scalar_block_layout : require

layout (location = 0) in vec3 fragColor;

layout (location = 1) in vec3 position;
layout (location = 2) in  vec3 ellipsoidCenter;
layout (location = 3) in  vec3 ellipsoidScale;
layout (location = 4) in  mat3 ellipsoidRot;
layout (location = 7) in  float ellipsoidAlpha;

layout (location = 0) out vec4 outColor;

layout(buffer_reference, std430) readonly buffer CameraData
{
    mat4 projection;
    mat4 view;
    vec4 camera_position;
};

layout(push_constant) uniform PushConstants
{
    CameraData camera_data_adddress;
} pc;

vec3 sphereIntersect(vec3 c, vec3 ro, vec3 p, out vec3 normal)
{
    mat3 ellipsoidRotationT = transpose(ellipsoidRot);

    vec3 rd = vec3(ellipsoidRotationT * normalize(p - ro)) / vec3(ellipsoidScale);
    vec3 u = (ellipsoidRotationT * vec3(ro - c)) / vec3(ellipsoidScale); // ro relative to c

    float a = dot(rd, rd);
    float b = 2.0 * dot(u, rd);
    float cc = dot(u, u) - 1.0;

    float discriminant = b * b - 4 * a * cc;

    // no intersection
    if (discriminant < 0.0)
    {
        return vec3(0.0);
    }

    float t1 = (-b + sqrt(discriminant)) / (2.0 * a);
    float t2 = (-b - sqrt(discriminant)) / (2.0 * a);
    float t = min(t1, t2);
    vec3 intersection = ro + ellipsoidRot * (vec3(t * rd) * ellipsoidScale);
    vec3 localIntersection = ((mat3(ellipsoidRotationT) * (intersection - c)) / ellipsoidScale);

    normal = ellipsoidRot * localIntersection;
    return intersection;
}


void main () 
{
    if (ellipsoidAlpha < 0.3)
    {
        discard;
    }

    CameraData matrices = CameraData(pc.camera_data_adddress);

    vec3 normal = vec3(0.0);

    vec3 camPos = vec3(matrices.camera_position);
    vec3 intersection = sphereIntersect(ellipsoidCenter, camPos, position, normal);

    // Discard if there's no intersections
    if (intersection == vec3(0.0))
    {
        discard;
    }

    vec3 rd = normalize(camPos - intersection);
    float align = max(dot(rd, normal), 0.1);

    vec4 newPos = matrices.projection * matrices.view * vec4(intersection, 1.0);
    newPos /= newPos.w;
    gl_FragDepth = newPos.z;

    // Lightly shade it by making it darker around the scraping angles.
    outColor = vec4(align * fragColor, 1.0);
}