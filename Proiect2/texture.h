#pragma once

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

class Texture
{
public:
    unsigned int id;
    unsigned int width, height;
    unsigned int internal_format;
    unsigned int image_format;

    unsigned int wrap_s;
    unsigned int wrap_t;

    unsigned int filter_min;
    unsigned int filter_max;

    Texture();

    void Generate(unsigned int width, unsigned int height, unsigned char* data);

	void Bind() const;
};

#endif;

