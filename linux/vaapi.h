#ifndef VA_H
#define VA_H
#ifdef USE_OLD_VAAPI
#include <va_x11.h>
#else
#include <va/va.h>
#include <va/va_x11.h>
#endif
typedef struct _VAAPIContext VAAPIContext;

struct _VAAPIContext {
    VADisplay           display;
    VADisplayAttribute *display_attrs;
    int                 n_display_attrs;
    VAConfigID          config_id;
    VAContextID         context_id;
    VASurfaceID         surface_id;
    VASubpictureID      subpic_ids[5];
    VAImage             subpic_image;
    VAProfile           profile;
    VAProfile          *profiles;
    int                 n_profiles;
    VAEntrypoint        entrypoint;
    VAEntrypoint       *entrypoints;
    int                 n_entrypoints;
    VAImageFormat      *image_formats;
    int                 n_image_formats;
    VAImageFormat      *subpic_formats;
    unsigned int       *subpic_flags;
    unsigned int        n_subpic_formats;
    unsigned int        picture_width;
    unsigned int        picture_height;
    VABufferID          pic_param_buf_id;
    VABufferID          iq_matrix_buf_id;
    VABufferID          bitplane_buf_id;
    VABufferID          huf_table_buf_id;
    VABufferID         *slice_buf_ids;
    unsigned int        n_slice_buf_ids;
    unsigned int        slice_buf_ids_alloc;
    void               *slice_params;
    unsigned int        slice_param_size;
    unsigned int        n_slice_params;
    unsigned int        slice_params_alloc;
    const uint8_t      *slice_data;
    unsigned int        slice_data_size;
    int                 use_glx_copy;
    void               *glx_surface;
};

int vaapi_init(VAAPIContext *vaapi,VADisplay display);
int vaapi_exit(VAAPIContext *vaapi);
int vaapi_display(VAAPIContext * const vaapi,int WinNu);
int vaapi_check_status(VAStatus status, const char *msg);

int vaapi_init_decoder(VAAPIContext * const vaapi,
                       VAProfile        profile,
                       VAEntrypoint     entrypoint,
                       unsigned int     picture_width,
                       unsigned int     picture_height);

int vaapi_decode(VAAPIContext * const vaapi);
int PutSurface(VAAPIContext * const vaapi,VASurfaceID surface_id,int hwnd,int w,int h);
VASurfaceID CreateSurface(VAAPIContext * const vaapi);
void DestroySurface(VAAPIContext * const vaapi,VASurfaceID surface_id);


#endif // VA_H
