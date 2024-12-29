#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

typedef struct {
  float real, imag;
}complex;

typedef struct  {
  int r, g, b;
} pixel;

typedef struct
{
    pixel *pixels;
    size_t width;
    size_t height;
}
bitmap;

static pixel * pixel_at (bitmap * bitmap, int x, int y){
  return bitmap->pixels + bitmap->width * y + x;
}

static int save_png_to_file (bitmap *bitmap, const char *path) {
    FILE * fp;
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_byte ** row_pointers = NULL;
    /* "status" contains the return value of this function. At first
       it is set to a value which means 'failure'. When the routine
       has finished its work, it is set to a value which means
       'success'. */
    int status = -1;
    /* The following number is set by trial and error only. I cannot
       see where it it is documented in the libpng manual.
    */
    int pixel_size = 3;
    int depth = 8;
    
    fp = fopen (path, "wb");
    if (! fp) {
        goto fopen_failed;
    }

    png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        goto png_create_write_struct_failed;
    }
    
    info_ptr = png_create_info_struct (png_ptr);
    if (info_ptr == NULL) {
        goto png_create_info_struct_failed;
    }
    
    /* Set up error handling. */

    if (setjmp (png_jmpbuf (png_ptr))) {
        goto png_failure;
    }
    
    /* Set image attributes. */

    png_set_IHDR (png_ptr,
                  info_ptr,
                  bitmap->width,
                  bitmap->height,
                  depth,
                  PNG_COLOR_TYPE_RGB,
                  PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_DEFAULT,
                  PNG_FILTER_TYPE_DEFAULT);
    
    /* Initialize rows of PNG. */

    row_pointers = png_malloc (png_ptr, bitmap->height * sizeof (png_byte *));
    for (y = 0; y < bitmap->height; y++) {
        png_byte *row = 
            png_malloc (png_ptr, sizeof (uint8_t) * bitmap->width * pixel_size);
        row_pointers[y] = row;
        for (x = 0; x < bitmap->width; x++) {
            pixel * pixel = pixel_at (bitmap, x, y);
            *row++ = pixel->r;
            *row++ = pixel->g;
            *row++ = pixel->b;
        }
    }
    
    /* Write the image data to "fp". */

    png_init_io (png_ptr, fp);
    png_set_rows (png_ptr, info_ptr, row_pointers);
    png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    /* The routine has successfully written the file, so we set
       "status" to a value which indicates success. */

    status = 0;
    
    for (y = 0; y < bitmap->height; y++) {
        png_free (png_ptr, row_pointers[y]);
    }
    png_free (png_ptr, row_pointers);
    
 png_failure:
 png_create_info_struct_failed:
    png_destroy_write_struct (&png_ptr, &info_ptr);
 png_create_write_struct_failed:
    fclose (fp);
 fopen_failed:
    return status;
}

complex sq_complex(complex c) {
  float temp_real;
  temp_real = c.real;
  c.real = (c.real * c.real) - (c.imag * c.imag);
  c.imag = 2*(temp_real * c.imag); 
  return c;
}

float absolute_value_sqrt(complex c){
  return (c.real*c.real)+(c.imag*c.imag);
}

complex add_complex(complex c, complex add) {
  c.real += add.real;
  c.imag += add.imag;
  return c;
}

void print_complex(complex c){
  printf("%f + %fi\n", c.real, c.imag);
}
// checks how meny iterations it takes
int is_in_set(float r, float i, int max_iter){
  int number_of_iter = 0;
  complex num, c;
  num.real = r;
  num.imag = i;
  c.real = r;
  c.imag = i;
  for (int a=0; a <= max_iter+1; a++){
    number_of_iter++; 
    // zn+1 = zn**2 + c
    num = sq_complex(num);
    num = add_complex(num, c);
    if (absolute_value_sqrt(num) > 4){
      break;
    }
  }
  return number_of_iter; 
}

bitmap calculate_mandelbrot(int width, int height, int max_iter) {
  bitmap mandelbrot;
  mandelbrot.width = width;
  mandelbrot.height = height;
  mandelbrot.pixels = calloc(mandelbrot.width * mandelbrot.height, sizeof(pixel));
  float i = -1.5;
  float pixel_size = 3.0f/width;

  for (int y = 0; y < mandelbrot.height; y++) {
    float r = -1.5;
    for (int x = 0; x < mandelbrot.width; x++) {
      r += pixel_size; 
      pixel * pixel = pixel_at (& mandelbrot, x, y);
      int value = is_in_set(r, i, max_iter); 
      if (value > max_iter) {
        pixel->r = 0; 
        pixel->g = 0;
        pixel->b = 0;
      } else {
        int g_value = pow(2, 8-value);
        pixel->r = 0; 
        pixel->g = g_value;
        pixel->b = 255-g_value;
      }
    }
    i += pixel_size;
  }
  return mandelbrot;
}

int main() {
  int status = 0;
  bitmap b = calculate_mandelbrot(1000, 1000, 255);
  if (save_png_to_file (&b, "mandel.png")) {
	    fprintf (stderr, "Error writing file.\n");
	    status = -1;
    }

    free (b.pixels);
  return status;
}

