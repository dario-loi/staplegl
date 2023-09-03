#pragma once

#define GLCORE_QUAD_VERTICES 20

float quadVertices[GLCORE_QUAD_VERTICES] = {
    // positions        // texture Coords
    -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, //
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, //
    1.0f, 1.0f, 0.0f, 1.0f, 1.0f, //
    1.0f, -1.0f, 0.0f, 1.0f, 0.0f //
};