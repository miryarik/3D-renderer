#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float x, y, z, w;
} vec3d;

vec3d vec3d_add(vec3d a, vec3d b);
vec3d vec3d_sub(vec3d a, vec3d b);
vec3d vec3d_get_scaled(vec3d a, float b);
float vec3d_length(vec3d a);
vec3d vec3d_get_normalized(vec3d a);
void vec3d_normalize(vec3d *a);
float vec3d_dot(vec3d a, vec3d b);
vec3d vec3d_cross(vec3d a, vec3d b);
vec3d vec3d_intersect_plane(vec3d plane_p, vec3d plane_n, vec3d lineStart, vec3d lineEnd);
#endif // VECTOR_H

#ifdef VECTOR_IMPLEMENTATION

vec3d vec3d_intersect_plane(vec3d plane_p, vec3d plane_n, vec3d lineStart, vec3d lineEnd) {
    
    plane_n = vec3d_get_normalized(plane_n);
    float plane_d = -vec3d_dot(plane_n, plane_p);
    float ad = vec3d_dot(lineStart, plane_n);
    float bd = vec3d_dot(lineEnd, plane_n);
    float t = (-plane_d - ad) / (bd - ad);
    vec3d lineStartToEnd = vec3d_sub(lineEnd, lineStart);
    vec3d lineToIntersect = vec3d_get_scaled(lineStartToEnd, t);
    return vec3d_add(lineStart, lineToIntersect);
}

vec3d vec3d_add(vec3d a, vec3d b) {
    return (vec3d) {a.x + b.x, a.y + b.y, a.z + b.z};
}

vec3d vec3d_sub(vec3d a, vec3d b) {
    return (vec3d) {a.x - b.x, a.y - b.y, a.z - b.z};
}

vec3d vec3d_get_scaled(vec3d a, float b) {
    return (vec3d) {a.x * b, a.y * b, a.z * b};
}

float vec3d_length(vec3d a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

vec3d vec3d_get_normalized(vec3d a) {
    float len = vec3d_length(a);
    return (vec3d) {a.x / len, a.y / len, a.z / len};
}

void vec3d_normalize(vec3d *a) {
    float len = vec3d_length(*a);
    a->x /= len;
    a->y /= len;
    a->z /= len;
}

float vec3d_dot(vec3d a, vec3d b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3d vec3d_cross(vec3d a, vec3d b) {
    return (vec3d) {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

#endif // VECTOR_IMPLEMENTATION