/* Include files */
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/fb.h>

#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#define NUM_BITS_PER_BYTE 8

/* Function Prototypes */
int output_bmp(unsigned char * buf, int x_res, int y_res, int bits_per_pixel, char * filename);
int screenshot(char *);

/* screenshot - This function to capture the current frame buffer.
 *
 * Return type: int
 *              0 - For Success
 *             -1 - Failed to open Frame Buffer device/Image file
 *             -2 - Writing image to output file failed
 *             -3 - Reading Frame Buffer failed
 */
int screenshot (char * filename)
{
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo fix;
    unsigned char *buf;
    int i;
    int screen_size;
    int fb_fd;
    unsigned char *fb_area;


    fb_fd = open("/dev/fb0", O_RDWR);
    if (!fb_fd) {
        printf("Could not open framebuffer.\n");
        return -1;
    }

    if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo)) {
       printf("Could not read variable screen info.\n");
       return -3;
    }

    printf("Screen Info\n");
    printf("vinfo.bits_per_pixel %d\n", vinfo.bits_per_pixel);
    printf("vinfo.xres %d\n", vinfo.xres);
    printf("vinfo.yres %d\n", vinfo.yres);
    screen_size = vinfo.xres*vinfo.yres*(vinfo.bits_per_pixel/NUM_BITS_PER_BYTE); 
    printf("screen_size %d\n", screen_size);
    if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &fix)) {
       printf("Could get fix screen info.\n");
       close(fb_fd);
       return -3;
    }
    printf("Line Length %d\n", fix.line_length);
    printf("Physical Address %x\n", fix.smem_start);
    printf("Buffer Length %d\n", fix.smem_len);

    fb_area = ( unsigned char * ) mmap ( 0, screen_size, PROT_READ|PROT_WRITE, MAP_SHARED, fb_fd, 0 );
    if (fb_area < 0)
    {
        printf("Memory Map for the LCD failed\n");
        close(fb_fd);
        return -1;
    }
    buf = (unsigned char *)malloc(screen_size);

    memcpy(buf, fb_area, screen_size);

    output_bmp(buf, vinfo.xres, vinfo.yres, vinfo.bits_per_pixel, filename );

    close(fb_fd);
    return 0;
}

/* main - This is the main routine which captures the frame buffer,
 *
 * Arguments: None
 * Return type: int
 *              1 - Invalid number of arguments
 *              0 - For Success
 */
int main (int argc, char **argv)
{
  if(argc < 2 || argc > 2)
  {
    printf("usage: screenshot filename\n");
    exit(1);
  }
  
  screenshot(argv[1]);
  return(0);

}


int output_bmp(unsigned char * buf, int x_res, int y_res, int bits_per_pixel, char * filename)
{
  unsigned short int type;                 /* Magic identifier            */
  unsigned int size;                       /* File size in bytes          */
  unsigned short int reserved1, reserved2;
  unsigned int offset;                     /* Offset to image data, bytes */

  unsigned int isize;               /* Header size in bytes      */
  int width,height;                /* Width and height of image */
  unsigned short int planes;       /* Number of colour planes   */
  unsigned short int bits;         /* Bits per pixel            */
  unsigned int compression;        /* Compression type          */
  unsigned int imagesize;          /* Image size in bytes       */ 
  int xresolution,yresolution;     /* Pixels per meter          */
  unsigned int ncolours;           /* Number of colours         */
  unsigned int importantcolours;   /* Important colours         */

  typedef struct {
    unsigned int red :5 __attribute__((__packed__));
    unsigned int green :6 __attribute__((__packed__));
    unsigned int blue :5 __attribute__((__packed__));
  } RGB_565;

  typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char alpha;
  } ARGB;

  FILE *outFile;
  int i,j;
  int counter=0;
  int y_off;
  ARGB image[y_res][x_res];
  ARGB * buf2;
  RGB_565  * buf3;

  buf2 = (ARGB *) buf;
  buf3 = (RGB_565 *)buf;
    
  if(bits_per_pixel == 32)
  {
    for(i=0;i<y_res;i++)
    {
      for(j=0;j<x_res;j++)
      {
        image[i][j].blue = buf2->blue;
        image[i][j].green = buf2->green;
        image[i][j].red = buf2->red;
        buf2+=1;
      }
    }
  }
  else
  {
    for(i=0;i<y_res;i++)
    {
      for(j=0;j<x_res;j++)
      {
        image[i][j].blue = buf3->red << 3;
        image[i][j].green = buf3->green << 2;
        image[i][j].red = buf3->blue << 3;
        buf3+=1;
      }
    }
  }
        
  outFile = fopen(filename, "wb");
  if(outFile == NULL)
  {
    return(2); 
  }
  type = 19778;
  offset = 54;
  reserved1 = 0;
  reserved2 = 0;
  size = y_res*x_res*3+54;

  isize = 40;
  width = x_res;
  height = y_res;
  bits = 24;
  planes = 1;
  compression = 0;
  imagesize = x_res*y_res*3;
  xresolution = 0;
  yresolution = 0;
  ncolours = 0;
  importantcolours = 0;
  fwrite(&type, sizeof(type), 1, outFile);
  fwrite(&size, sizeof(size), 1, outFile);
  fwrite(&reserved1, sizeof(reserved1), 1, outFile);
  fwrite(&reserved2, sizeof(reserved2), 1, outFile);
  fwrite(&offset, sizeof(offset), 1, outFile);

  fwrite(&isize, sizeof(isize), 1, outFile);
  fwrite(&width, sizeof(width), 1, outFile);
  fwrite(&height, sizeof(height), 1, outFile);
  fwrite(&planes, sizeof(planes), 1, outFile);
  fwrite(&bits, sizeof(bits), 1, outFile);
  fwrite(&compression, sizeof(compression), 1, outFile);
  fwrite(&imagesize, sizeof(imagesize), 1, outFile);
  fwrite(&xresolution, sizeof(xresolution), 1, outFile);
  fwrite(&yresolution, sizeof(yresolution), 1, outFile);
  fwrite(&ncolours, sizeof(ncolours), 1, outFile);
  fwrite(&importantcolours, sizeof(importantcolours), 1, outFile);
  for(i=0;i<y_res;i++)
  {
    for(j=0;j<x_res;j++)
    {
      y_off = y_res-1;
      fwrite(&image[y_off-i][j].blue, sizeof(char), 1, outFile);
      fwrite(&image[y_off-i][j].green, sizeof(char), 1, outFile);
      fwrite(&image[y_off-i][j].red, sizeof(char), 1, outFile);
    }
  }
  fclose(outFile);

  return(0);
}

