#ifndef MATRIX_H
#define MATRIX_H

#include "vector.h"

// matrix
typedef struct {
    float m[4][4];
} mat4x4;


mat4x4 matrix_identity();
vec3d matrix_multiply_vector(mat4x4 m, vec3d i);
mat4x4 matrix_rotate_x(float angle);
mat4x4 matrix_rotate_y(float angle);
mat4x4 matrix_rotate_z(float angle);
mat4x4 matrix_translation(float x, float y, float z);
mat4x4 matrix_projection(float fFovDeg, float fAspectRatio, float fNear, float fFar);
mat4x4 matrix_multiply_matrix(mat4x4 m1, mat4x4 m2);
mat4x4 matrix_point_at(vec3d pos, vec3d target, vec3d up);

#endif // MATRIX_H

#ifdef MATRIX_IMPLEMENTATION

vec3d matrix_multiply_vector(mat4x4 m, vec3d i) {
    vec3d v;
    v.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
    v.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
    v.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
    v.w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];  
    return v;
}   

mat4x4 matrix_identity() {
    mat4x4 m = {0};
    m.m[0][0] = 1;
    m.m[1][1] = 1;
    m.m[2][2] = 1;
    m.m[3][3] = 1;
    return m;
}

mat4x4 matrix_rotate_x(float angle) {
    mat4x4 m = {0};
    m.m[0][0] = 1;
    m.m[1][1] = cosf(angle);
    m.m[1][2] = sinf(angle);
    m.m[2][1] = -sinf(angle);
    m.m[2][2] = cosf(angle);
    m.m[3][3] = 1;
    return m;
}

mat4x4 matrix_rotate_y(float angle) {
    mat4x4 m = {0};
    m.m[0][0] = cosf(angle);
    m.m[0][2] = sinf(angle);
    m.m[2][0] = -sinf(angle);
    m.m[1][1] = 1;
    m.m[2][2] = cosf(angle);
    m.m[3][3] = 1;
    return m;
}

mat4x4 matrix_rotate_z(float angle) {
    mat4x4 m = {0};
    m.m[0][0] = cosf(angle);
    m.m[0][1] = sinf(angle);
    m.m[1][0] = -sinf(angle);
    m.m[1][1] = cosf(angle);
    m.m[2][2] = 1;
    m.m[3][3] = 1;
    return m;
}

mat4x4 matrix_translation(float x, float y, float z) {
    mat4x4 m = matrix_identity();
    m.m[3][0] = x;
    m.m[3][1] = y;
    m.m[3][2] = z;
    return m;
}

mat4x4 matrix_projection(float fFovDeg, float fAspectRatio, float fNear, float fFar) {
    mat4x4 m = {0};
    float fFovRad = 1.0f / tanf(fFovDeg * 0.5f / 180.0f * 3.14159f);
    m.m[0][0] = fAspectRatio * fFovRad;
    m.m[1][1] = fFovRad;
    m.m[2][2] = fFar / (fFar - fNear);
    m.m[3][2] = (-fFar * fNear) / (fFar - fNear);
    m.m[2][3] = 1.0f;
    m.m[3][3] = 0.0f;
    return m;
}

mat4x4 matrix_multiply_matrix(mat4x4 m1, mat4x4 m2) {
    mat4x4 m = {0};
    for (int c = 0; c < 4; c++) {
        for (int matrix = 0; matrix < 4; matrix++) {
            m.m[matrix][c] = m1.m[matrix][0] * m2.m[0][c] + m1.m[matrix][1] * m2.m[1][c] + m1.m[matrix][2] * m2.m[2][c] + m1.m[matrix][3] * m2.m[3][c];
        }
    }
    return m;
}

mat4x4 matrix_point_at(vec3d pos, vec3d target, vec3d up) {
    
    vec3d newForward = vec3d_sub(target, pos);
    newForward = vec3d_get_normalized(newForward);

    vec3d a = vec3d_get_scaled(newForward, vec3d_dot(up, newForward));
    vec3d newUp = vec3d_sub(up, a);
    newUp = vec3d_get_normalized(newUp);

    vec3d newRight = vec3d_cross(newUp, newForward);

    mat4x4 r = {0};
    r.m[0][0] = newRight.x;
    r.m[0][1] = newRight.y;
    r.m[0][2] = newRight.z;
    r.m[0][3] = 0.0f;
    r.m[1][0] = newUp.x;
    r.m[1][1] = newUp.y;
    r.m[1][2] = newUp.z;
    r.m[1][3] = 0.0f;
    r.m[2][0] = newForward.x;
    r.m[2][1] = newForward.y;
    r.m[2][2] = newForward.z;
    r.m[2][3] = 0.0f;
    r.m[3][0] = pos.x;
    r.m[3][1] = pos.y;
    r.m[3][2] = pos.z;
    r.m[3][3] = 1.0f;

    return r;
}


mat4x4 matrix_quick_inverse(mat4x4 m)
{
    mat4x4 matrix = {0};
    matrix.m[0][0] = m.m[0][0];
    matrix.m[0][1] = m.m[1][0];
    matrix.m[0][2] = m.m[2][0];
    matrix.m[0][3] = 0.0f;
    matrix.m[1][0] = m.m[0][1];
    matrix.m[1][1] = m.m[1][1];
    matrix.m[1][2] = m.m[2][1];
    matrix.m[1][3] = 0.0f;
    matrix.m[2][0] = m.m[0][2];
    matrix.m[2][1] = m.m[1][2];
    matrix.m[2][2] = m.m[2][2];
    matrix.m[2][3] = 0.0f;
    matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
    matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
    matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
    matrix.m[3][3] = 1.0f;
    return matrix;
}

#endif // MATRIX_IMPLEMENTATION