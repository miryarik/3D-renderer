// seek - 43:00
// fix x-inversion

#include <stdio.h>
#include <raylib.h>
#include <stdlib.h>
#include <math.h>

#define ARRAY_LEN(x) (sizeof(x)/sizeof((x)[0]))

#define VECTOR_IMPLEMENTATION
#include "vector.h"
#undef VECTOR_IMPLEMENTATION

#define MATRIX_IMPLEMENTATION
#include "matrix.h"
#undef MATRIX_IMPLEMENTATION

typedef struct {
   vec3d p[3];
   Color color;
} triangle;

typedef struct {
    triangle *tris;
    int count;
} mesh;



bool LoadModelFromObjectFile(mesh* model, char* filepath) {
    FILE *f = fopen(filepath, "r");
    if(f == NULL) {
        fprintf(stderr, "ERROR: failed to open file!");
        return false;
    }

    int vertCount = 0;
    int triCount = 0;

    char c;
    while((c = fgetc(f)) != EOF) {
        if(c == 'v' && fgetc(f) == ' ') {
            vertCount++;
        }
        else if(c == 'f' && fgetc(f) == ' ') {
            triCount++;
        }
    }

    if(vertCount == 0) {
        return false;
    }

    triangle *tris = (triangle*)malloc(triCount * sizeof(triangle));
    vec3d *vertices = (vec3d*)malloc(vertCount * sizeof(vec3d));
    int vertIdx = 0;
    int triIdx = 0;
    fseek(f, 0, SEEK_SET);

    while((c = fgetc(f)) != EOF) {
        if(c == 'v' && fgetc(f) == ' ') {
            vec3d vert;
            fscanf(f, "%f %f %f", &vert.x, &vert.y, &vert.z);
            vertices[vertIdx] = vert;
            vertIdx++;
        }

        else if(c == 'f' && fgetc(f) == ' ') {
            int idx1, idx2, idx3;
            fscanf(f, "%d %d %d", &idx1, &idx2, &idx3);
            tris[triIdx].p[0] = vertices[idx1-1];
            tris[triIdx].p[1] = vertices[idx2-1];
            tris[triIdx].p[2] = vertices[idx3-1];
            triIdx++;
            
        }
    }

    printf("Vert count: %d\n", vertCount);
    printf("Tri count: %d\n", triCount);
    printf("verts read: %d\n", vertIdx);
    printf("tris read: %d\n", triIdx);

    if(triCount == triIdx) {
        model->count = triCount;
    }
    else {
        return false;
    }

    free(vertices);

    model->tris = tris;    
    return true;
}


void drawTriangle(float x1, float x2, float x3, float y1, float y2, float y3, Color color) {
    DrawTriangleLines((Vector2){x1, y1}, (Vector2){x2, y2}, (Vector2){x3, y3}, color);
}

void fillTriangle(float x1, float x2, float x3, float y1, float y2, float y3, Color color) {
    // Calculate the signed area of the triangle
    float area = (x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1);

    // If the area is positive, swap two vertices to make the order clockwise
    if (area > 0) {
        float tempX = x2, tempY = y2;
        x2 = x3; y2 = y3;
        x3 = tempX; y3 = tempY;
    }

    DrawTriangle((Vector2){x1, y1}, (Vector2){x2, y2}, (Vector2){x3, y3}, color);
}

int compareTris(const void *a, const void *b) {
    const triangle *triA = (const triangle *) a;
    const triangle *triB = (const triangle *) b;
    float z1 = (triA->p[0].z + triA->p[1].z + triA->p[2].z) / 3.0f;
    float z2 = (triB->p[0].z + triB->p[1].z + triB->p[2].z) / 3.0f;

    return (z1 < z2) - (z1 > z2);
}


int main() {
    
    int screenWidth = 800;
    int screenHeight = 800;



    mesh meshCube;

    if(!LoadModelFromObjectFile(&meshCube, "./3dModels/axis.obj")) {
        fprintf(stderr, "ERROR: unable to load model");
        return 1;
    }


    InitWindow(screenWidth, screenHeight, "Renderer");
    SetTargetFPS(60);

    vec3d vCamera = {0.0f, 0.0f, 0.0f, 1.0f};
    vec3d vLookDir;
    vec3d vUp = {0.0f, 1.0f, 0.f, 0.0f};
    float fYaw = 0.0f;
    float fTheta = 0.0f;

    while(!WindowShouldClose()) {

        

        if(IsKeyDown(KEY_UP)) {
            vCamera.y += 8.0f * GetFrameTime();
        }

        if(IsKeyDown(KEY_DOWN)) {
            vCamera.y -= 8.0f * GetFrameTime();
        }

        vec3d vLeft = vec3d_cross(vLookDir, vUp);
        vLeft = vec3d_get_normalized(vLeft);


        if(IsKeyDown(KEY_A)) {
            vCamera = vec3d_add(vCamera, vec3d_get_scaled(vLeft, 8.0f * GetFrameTime()));
        }

        if(IsKeyDown(KEY_D)) {
            vCamera = vec3d_sub(vCamera, vec3d_get_scaled(vLeft, 8.0f * GetFrameTime()));
        }

        if(IsKeyDown(KEY_W)) {
            vec3d vForward = vec3d_get_scaled(vLookDir, 8.0f * GetFrameTime());
            vCamera = vec3d_add(vCamera, vForward);
        }

        if(IsKeyDown(KEY_S)) {
            vec3d vForward = vec3d_get_scaled(vLookDir, 8.0f * GetFrameTime());
            vCamera = vec3d_sub(vCamera, vForward);
        }

        if (IsKeyDown(KEY_LEFT)) {
            fYaw -= 2.0f * GetFrameTime();
        }

        if (IsKeyDown(KEY_RIGHT)) {
            fYaw += 2.0f * GetFrameTime();
        }

        

        screenWidth = GetScreenWidth();
        screenHeight = GetScreenHeight();

        mat4x4 matProj = matrix_projection(90.0f, (float)screenHeight/(float)screenWidth, 0.1f, 1000.0f);
        // fTheta += 1.0f * GetFrameTime();
        mat4x4 matRotZ = matrix_rotate_z(fTheta * 0.5f);
        mat4x4 matRotX = matrix_rotate_x(fTheta);

        mat4x4 matTrans = matrix_translation(0.0f, 0.0f, 16.0f);

        mat4x4 matWorld = matrix_multiply_matrix(matRotZ, matRotX);
        matWorld = matrix_multiply_matrix(matWorld, matTrans);

        vLookDir = (vec3d) {0.0f, 0.0f, 1.0f, 0.0f};
        vec3d vTarget = {0.0f, 0.0f, 1.0f, 0.0f};
        mat4x4 matCameraRot = matrix_rotate_y(fYaw);
        vLookDir = matrix_multiply_vector(matCameraRot, vTarget);
        vTarget = vec3d_add(vCamera, vLookDir);

        mat4x4 matCamera = matrix_point_at(vCamera, vTarget, vUp);
        mat4x4 matView = matrix_quick_inverse(matCamera);
        // here 

        BeginDrawing();
        {
            ClearBackground(BLACK);

            triangle *trisToRaster = (triangle *)malloc(meshCube.count * sizeof(triangle));
            int nTrisToRaster = 0;

            for(int i = 0; i < meshCube.count; i++) {
                triangle tri = meshCube.tris[i];
                triangle triProjected, triTransformed, triViewed;

                triTransformed.p[0] = matrix_multiply_vector(matWorld, tri.p[0]);
                triTransformed.p[1] = matrix_multiply_vector(matWorld, tri.p[1]);
                triTransformed.p[2] = matrix_multiply_vector(matWorld, tri.p[2]);


                vec3d normal, line1, line2;
                
                line1 = vec3d_sub(triTransformed.p[1], triTransformed.p[0]);
                line2 = vec3d_sub(triTransformed.p[2], triTransformed.p[0]);

                normal = vec3d_cross(line1, line2);

                normal = vec3d_get_normalized(normal);

                vec3d vCameraRay = vec3d_sub(triTransformed.p[0], vCamera);

                if (vec3d_dot(normal, vCameraRay) < 0.0f)
                {
                    
                    // illumination
                    vec3d light_direction = {0.0f, -1.0f, -1.0f, 0.0f};
                    light_direction = vec3d_get_normalized(light_direction);
                    
                    float minLum = 0.1f;
                    float lum = vec3d_dot(normal, light_direction);
                    lum = lum < minLum ? minLum : lum;
                    Color shade = ColorFromHSV(0, 0, lum);

                    triTransformed.color = shade;

                    triViewed.p[0] = matrix_multiply_vector(matView, triTransformed.p[0]);
                    triViewed.p[1] = matrix_multiply_vector(matView, triTransformed.p[1]);
                    triViewed.p[2] = matrix_multiply_vector(matView, triTransformed.p[2]);


                    triProjected.p[0] = matrix_multiply_vector(matProj, triViewed.p[0]);
                    triProjected.p[1] = matrix_multiply_vector(matProj, triViewed.p[1]);
                    triProjected.p[2] = matrix_multiply_vector(matProj, triViewed.p[2]);



                    triProjected.p[0] = vec3d_get_scaled(triProjected.p[0], 1.0f / triProjected.p[0].w);
                    triProjected.p[1] = vec3d_get_scaled(triProjected.p[1], 1.0f / triProjected.p[1].w);
                    triProjected.p[2] = vec3d_get_scaled(triProjected.p[2], 1.0f / triProjected.p[2].w);


                    triProjected.color = triTransformed.color;

                    vec3d vOffsetView = {1.0f, 1.0f, 0.0f};

                    triProjected.p[0].y *= -1.0f;
                    triProjected.p[1].y *= -1.0f;
                    triProjected.p[2].y *= -1.0f;

                    triProjected.p[0] = vec3d_add(triProjected.p[0], vOffsetView);
                    triProjected.p[1] = vec3d_add(triProjected.p[1], vOffsetView);
                    triProjected.p[2] = vec3d_add(triProjected.p[2], vOffsetView);


                    triProjected.p[0].x *= 0.5f * (float) screenWidth;
                    triProjected.p[0].y *= 0.5f * (float) screenHeight;
                    triProjected.p[1].x *= 0.5f * (float) screenWidth;
                    triProjected.p[1].y *= 0.5f * (float) screenHeight;
                    triProjected.p[2].x *= 0.5f * (float) screenWidth;
                    triProjected.p[2].y *= 0.5f * (float) screenHeight;

                    trisToRaster[nTrisToRaster++] = triProjected;
                }

            }

            qsort(trisToRaster, nTrisToRaster, sizeof(triangle), compareTris);

            // this is messed up
            for(int i = 0; i < nTrisToRaster; i++) {
                triangle triProjected = trisToRaster[i];
                fillTriangle(triProjected.p[0].x, 
                    triProjected.p[1].x, 
                    triProjected.p[2].x, 
                    triProjected.p[0].y, 
                    triProjected.p[1].y, 
                    triProjected.p[2].y, 
                    triProjected.color);

                // drawTriangle(triProjected.p[0].x, 
                //     triProjected.p[1].x, 
                //     triProjected.p[2].x, 
                //     triProjected.p[0].y, 
                //     triProjected.p[1].y, 
                //     triProjected.p[2].y, 
                //     GREEN);
            }

            free(trisToRaster);
        }
        EndDrawing();

    }

}

