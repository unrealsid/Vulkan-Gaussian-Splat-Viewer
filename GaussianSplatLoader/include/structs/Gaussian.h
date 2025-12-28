#pragma once

struct Gaussian
{
    float x, y, z;
    float nx, ny, nz;

    float f_dc[3];        // f_dc_0 .. f_dc_2
    float f_rest[45];     // f_rest_0 .. f_rest_44

    float opacity;

    float scale[3];       // scale_0 .. scale_2
    float rotation[4];    // rot_0 .. rot_3
};
