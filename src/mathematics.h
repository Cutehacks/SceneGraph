#ifndef MATHEMATICS_H
#define MATHEMATICS_H

#include <math.h>

static inline float degrees_to_radians(float degrees)
{
    return degrees * M_PI / 180.0;
}

static inline float magnitude_vector(float *v)
{
    return sqrtf(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}

static inline void normalize_vector(float *v)
{
    float m = magnitude_vector(v);
    v[0] = v[0] / m;
    v[1] = v[1] / m;
    v[2] = v[2] / m;
    v[3] = v[3] / m;
}

static inline float distance_point_to_origin(float x, float y, float z)
{
    return sqrtf(x*x + y*y + z*z);
}

static inline float distance_plane_to_point(const float *plane, const float *point)
{
    return plane[0]*point[0] + plane[1]*point[1] + plane[2]*point[2] + plane[3];
}

static inline float distance_point_to_point(const float *a, const float *b)
{
    float dx = b[0] - a[0];
    float dy = b[1] - a[1];
    float dz = b[2] - a[2];
    return sqrtf(dx*dx + dy*dy + dz*dz);
}

static inline float dot_vectors(const float *a, const float *b)
{
    return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

static inline void multiply_matrices(const float *a, const float *b, float *r)
{
    for (int i = 0; i < 16; i += 4)
        for (int j = 0; j < 4; ++j)
            r[i+j] = b[i]*a[j] + b[i+1]*a[j+4] + b[i+2]*a[j+8] + b[i+3]*a[j+12];
}

static inline void multiply_quaternions(const float *a, const float *b, float *r)
{
    float w = a[0]*b[0] - a[1]*b[1] - a[2]*b[2] - a[3]*b[3];
    float x = a[0]*b[1] + a[1]*b[0] + a[2]*b[3] - a[3]*b[2];
    float y = a[0]*b[2] - a[1]*b[3] + a[2]*b[0] + a[3]*b[1];
    float z = a[0]*b[3] + a[1]*b[2] - a[2]*b[1] + a[3]*b[0];
    r[0] = w; r[1] = x; r[2] = y; r[3] = z;
}

static inline void quaternion_rotate_vector(const float *quaternion, const float *vector, float *result)
{
    float vector_quaternion[] = { 0.0, vector[0], vector[1], vector[2], vector[3] };
    float conjugated_quaternion[] = { quaternion[0], -quaternion[1], -quaternion[2], -quaternion[3] };

    multiply_quaternions(quaternion, vector_quaternion, vector_quaternion);
    multiply_quaternions(vector_quaternion, conjugated_quaternion, vector_quaternion);

    result[0] = vector_quaternion[1];
    result[1] = vector_quaternion[2];
    result[2] = vector_quaternion[3];
}

static inline void quaternion_rotation_matrix(const float *quaternion, float *matrix)
{
    float x_axis[] = { 1.0, 0.0, 0.0 };
    float y_axis[] = { 0.0, 1.0, 0.0 };
    float z_axis[] = { 0.0, 0.0, 1.0 };

    float x_rotated[3];
    float y_rotated[3];
    float z_rotated[3];

    quaternion_rotate_vector(quaternion, x_axis, x_rotated);
    quaternion_rotate_vector(quaternion, y_axis, y_rotated);
    quaternion_rotate_vector(quaternion, z_axis, z_rotated);

    matrix[0] = dot_vectors(x_axis, x_rotated);
    matrix[1] = dot_vectors(y_axis, x_rotated);
    matrix[2] = dot_vectors(z_axis, x_rotated);
    matrix[3] = 0;

    matrix[4] = dot_vectors(x_axis, y_rotated);
    matrix[5] = dot_vectors(y_axis, y_rotated);
    matrix[6] = dot_vectors(z_axis, y_rotated);
    matrix[7] = 0;

    matrix[8] = dot_vectors(x_axis, z_rotated);
    matrix[9] = dot_vectors(y_axis, z_rotated);
    matrix[10] = dot_vectors(z_axis, z_rotated);
    matrix[11] = 0;

    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 1;
}

#endif // MATHEMATICS_H
