//////////////////////////////////////////////////////////////////////////////////
//
// 	Copyright (c) 2024 Hatim Belahmer
//
// 	Permission is hereby granted, free of charge, to any person obtaining a copy
// 	of this software and associated documentation files (the "Software"), to deal
// 	in the Software without restriction, including without limitation the rights
// 	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// 	copies of the Software, and to permit persons to whom the Software is
// 	furnished to do so, subject to the following conditions:
//
// 	The above copyright notice and this permission notice shall be included in all
// 	copies or substantial portions of the Software.
//
// 	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// 	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// 	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// 	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// 	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// 	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// 	SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////////////

#ifndef __PPM6_H__
#define __PPM6_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct PPM6_Pixel { unsigned char r, g, b; };

struct PPM6_Image {
	size_t width, height;
	struct PPM6_Pixel *data;
};

#define SET_PIXEL(R, G, B) (struct PPM6_Pixel) { (unsigned char) R % 255, (unsigned char) G % 255, (unsigned char) B % 255 }

int PPM6_init_image(struct PPM6_Image *img, const size_t width, const size_t height) {
	img->width = width;
	img->height = height;
	img->data = (struct PPM6_Pixel*) malloc(width * height * sizeof(struct PPM6_Pixel));
	if (!img->data) { return -1; }
	return 0;
}

void PPM6_free_image(struct PPM6_Image *img) {
	if (img->data) free((void*) img->data);
}

int PPM6_set_pixel(struct PPM6_Image *img, struct PPM6_Pixel *pixel, size_t x, size_t y) {
	if (x >= img->width || y >= img->height) { return -1; }
	img->data[y * img->width + x].r = pixel->r;
	img->data[y * img->width + x].g = pixel->g;
	img->data[y * img->width + x].b = pixel->b;
	return 0;
}

int PPM6_load_image(const char *filename, struct PPM6_Image *img) {
	FILE *fp = fopen(filename, "rb");
	if (!fp) { return -1; }
	char format[3];
	if (fscanf(fp, "%2s", format) != 1 || strcmp(format, "P6") != 0) {
		fclose(fp);
		return -2; 
	}
	int c;
	while ((c = fgetc(fp)) == '#') {
		while (fgetc(fp) != '\n');
	}
	ungetc(c, fp);
	if (fscanf(fp, "%lu %lu\n255\n", &img->width, &img->height) != 2) {
		fclose(fp);
		return -3;
	}
	img->data = (struct PPM6_Pixel*) malloc(img->width * img->height * sizeof(struct PPM6_Pixel));
	if (!img->data) {
		fclose(fp);
		return -1;
	}
	if (fread(img->data, sizeof(struct PPM6_Pixel), img->width * img->height, fp) != img->width * img->height) {
		free(img->data);
		fclose(fp);
		return -1;
	}
	fclose(fp);
	return 0;
}

int PPM6_save_image(const char *filename, struct PPM6_Image *img) {
	FILE *fp = fopen(filename, "wb");
	if (!fp) { return -1; }
	fprintf(fp, "P6\n%ld %ld\n255\n", img->width, img->height);
	fwrite(img->data, sizeof(struct PPM6_Pixel), img->width * img->height, fp);
	fclose(fp);
	return 0;
}

struct PPM6_Pixel* PPM6_str_to_pixels(const char *message, size_t *num_pixels) {
	size_t length = strlen(message);
	size_t capacity = length + 3 - (length % 3);
	size_t number_of_triplets = capacity / 3;
	size_t counter = 0;
	struct PPM6_Pixel *result = (struct PPM6_Pixel*) malloc(sizeof(struct PPM6_Pixel) * number_of_triplets);
	if (!result) return NULL;
	if (num_pixels) *num_pixels = number_of_triplets;
	for (size_t i = 0; i < length; i += 3) {
		struct PPM6_Pixel p = SET_PIXEL(message[i], message[i+1], message[i+2]);
		result[counter] = p;
		counter++;
	}
	return result;
}

char *PPM6_image_to_str(struct PPM6_Image *img) {
    if (!img->data) return NULL;
    size_t total_length = img->width * img->height * 3;
    char *message = malloc(total_length + 1);
    assert(message != NULL);
    char *ptr = message;
    for (size_t i = 0; i < img->width * img->height; i++) {
        struct PPM6_Pixel pixel = img->data[i];
        *ptr++ = pixel.r;
        *ptr++ = pixel.g;
        *ptr++ = pixel.b;
    }
    *ptr = '\0';
    return message;
}

#endif