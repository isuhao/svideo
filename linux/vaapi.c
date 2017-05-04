#include "sysdeps.h"
#include "vaapi.h"
#include "vaapi_compat.h"

static const char *string_of_VAProfile(VAProfile profile)
{
    switch (profile) {
#define PROFILE(profile) \
        case VAProfile##profile: return "VAProfile" #profile
        PROFILE(MPEG2Simple);
        PROFILE(MPEG2Main);
        PROFILE(MPEG4Simple);
        PROFILE(MPEG4AdvancedSimple);
        PROFILE(MPEG4Main);
#if VA_CHECK_VERSION(0,32,0)
        PROFILE(JPEGBaseline);
        PROFILE(H263Baseline);
        PROFILE(H264ConstrainedBaseline);
#endif
        PROFILE(H264Baseline);
        PROFILE(H264Main);
        PROFILE(H264High);
        PROFILE(VC1Simple);
        PROFILE(VC1Main);
        PROFILE(VC1Advanced);
#undef PROFILE
    default: break;
    }
    return "<unknown>";
}

static const char *string_of_VAEntrypoint(VAEntrypoint entrypoint)
{
    switch (entrypoint) {
#define ENTRYPOINT(entrypoint) \
        case VAEntrypoint##entrypoint: return "VAEntrypoint" #entrypoint
        ENTRYPOINT(VLD);
        ENTRYPOINT(IZZ);
        ENTRYPOINT(IDCT);
        ENTRYPOINT(MoComp);
        ENTRYPOINT(Deblocking);
#if VA_CHECK_VERSION(0,32,0)
        ENTRYPOINT(EncSlice);
        ENTRYPOINT(EncPicture);
#endif
#undef ENTRYPOINT
    default: break;
    }
    return "<unknown>";
}

static const char *string_of_VADisplayAttribType(VADisplayAttribType type)
{
    switch (type) {
#define TYPE(type) \
        case VADisplayAttrib##type: return "VADisplayAttrib" #type
        TYPE(Brightness);
        TYPE(Contrast);
        TYPE(Hue);
        TYPE(Saturation);
        TYPE(BackgroundColor);
#if !VA_CHECK_VERSION(0,34,0)
        TYPE(DirectSurface);
#endif
#if VA_CHECK_VERSION(0,32,0)
        TYPE(Rotation);
#endif
#undef TYPE
    default: break;
    }
    return "<unknown>";
}

int
vaapi_init(VAAPIContext *vaapi,VADisplay display)
{
    int major_version, minor_version;
       int i, num_display_attrs, max_display_attrs;
       VADisplayAttribute *display_attrs = NULL;
       VAStatus status;

       if (!display)
       {
           goto error;
       }
       printf("VA display %p\n", display);

       status = vaInitialize(display, &major_version, &minor_version);
       if (!vaapi_check_status(status, "vaInitialize()"))
           goto error;
       printf("VA API version %d.%d\n", major_version, minor_version);

       max_display_attrs = vaMaxNumDisplayAttributes(display);
       display_attrs = (VADisplayAttribute *)malloc(max_display_attrs * sizeof(display_attrs[0]));
       if (!display_attrs)
           goto error;

       num_display_attrs = 0; /* XXX: workaround old GMA500 bug */
       status = vaQueryDisplayAttributes(display, display_attrs, &num_display_attrs);
       if (!vaapi_check_status(status, "vaQueryDisplayAttributes()"))
           goto error;
       printf("%d display attributes available\n", num_display_attrs);
       for (i = 0; i < num_display_attrs; i++) {
           VADisplayAttribute * const display_attr = &display_attrs[i];
           printf("  %-32s (%s/%s) min %d max %d value 0x%x\n",
                 //string_of_VADisplayAttribType(display_attr->type),
                  "",
                 (display_attr->flags & VA_DISPLAY_ATTRIB_GETTABLE) ? "get" : "---",
                 (display_attr->flags & VA_DISPLAY_ATTRIB_SETTABLE) ? "set" : "---",
                 display_attr->min_value,
                 display_attr->max_value,
                 display_attr->value);
       }

       vaapi->display               = display;
       vaapi->config_id             = VA_INVALID_ID;
       vaapi->context_id            = VA_INVALID_ID;
       vaapi->surface_id            = VA_INVALID_ID;
       vaapi->subpic_image.image_id = VA_INVALID_ID;
       for (i = 0; i < ARRAY_ELEMS(vaapi->subpic_ids); i++)
           vaapi->subpic_ids[i]     = VA_INVALID_ID;
       vaapi->pic_param_buf_id      = VA_INVALID_ID;
       vaapi->iq_matrix_buf_id      = VA_INVALID_ID;
       vaapi->bitplane_buf_id       = VA_INVALID_ID;
       vaapi->huf_table_buf_id      = VA_INVALID_ID;
       vaapi->display_attrs         = display_attrs;
       vaapi->n_display_attrs       = num_display_attrs;
       return 0;
   error:
       free(display_attrs);
       return -1;
}

static int has_profile(VAAPIContext *vaapi, VAProfile profile)
{
    VAStatus status;
    int i;

    if (!vaapi->profiles || vaapi->n_profiles == 0) {
        vaapi->profiles = (VAProfile*)calloc(vaMaxNumProfiles(vaapi->display), sizeof(vaapi->profiles[0]));

        status = vaQueryConfigProfiles(vaapi->display,
                                       vaapi->profiles,
                                       &vaapi->n_profiles);
        if (!vaapi_check_status(status, "vaQueryConfigProfiles()"))
            return 0;

        printf("%d profiles available\n", vaapi->n_profiles);
        for (i = 0; i < vaapi->n_profiles; i++)
            printf("  %s\n", string_of_VAProfile(vaapi->profiles[i]));
    }

    for (i = 0; i < vaapi->n_profiles; i++) {
        if (vaapi->profiles[i] == profile)
        {
            printf("match point:  %s\n", string_of_VAProfile(vaapi->profiles[i]));
            return 1;
        }
    }
    return 0;
}

static int has_entrypoint(VAAPIContext *vaapi, VAProfile profile, VAEntrypoint entrypoint)
{
    VAStatus status;
    int i;

    if (!vaapi->entrypoints || vaapi->n_entrypoints == 0) {
        vaapi->entrypoints = (VAEntrypoint*)calloc(vaMaxNumEntrypoints(vaapi->display), sizeof(vaapi->entrypoints[0]));

        status = vaQueryConfigEntrypoints(vaapi->display, profile,
                                          vaapi->entrypoints,
                                          &vaapi->n_entrypoints);
        if (!vaapi_check_status(status, "vaQueryConfigEntrypoints()"))
            return 0;

        printf("%d entrypoints available for %s\n", vaapi->n_entrypoints,
              string_of_VAProfile(profile));
        for (i = 0; i < vaapi->n_entrypoints; i++)
            printf("  %s\n", string_of_VAEntrypoint(vaapi->entrypoints[i]));
    }

    for (i = 0; i < vaapi->n_entrypoints; i++) {
        if (vaapi->entrypoints[i] == entrypoint)
        {
            printf("match point:  %s\n", string_of_VAEntrypoint(vaapi->entrypoints[i]));
            return 1;
        }
    }
    return 0;
}

int vaapi_init_decoder(VAAPIContext * const vaapi,
                       VAProfile        profile,
                       VAEntrypoint     entrypoint,
                       unsigned int     picture_width,
                       unsigned int     picture_height){
    VAConfigAttrib attrib;
       VAConfigID config_id = VA_INVALID_ID;
       VAContextID context_id = VA_INVALID_ID;
       VASurfaceID surface_id = VA_INVALID_ID;
       VAStatus status;

       if (!vaapi)
           return -1;

       if (!has_profile(vaapi, profile))
           return -1;
       if (!has_entrypoint(vaapi, profile, entrypoint))
           return -1;

       if (vaapi->profile != profile || vaapi->entrypoint != entrypoint) {
           if (vaapi->config_id != VA_INVALID_ID)
               vaDestroyConfig(vaapi->display, vaapi->config_id);

           attrib.type = VAConfigAttribRTFormat;
           status = vaGetConfigAttributes(vaapi->display, profile, entrypoint,
                                          &attrib, 1);
           if (!vaapi_check_status(status, "vaGetConfigAttributes()"))
               return -1;
           if ((attrib.value & VA_RT_FORMAT_YUV420) == 0)
               return -1;

           status = vaCreateConfig(vaapi->display, profile, entrypoint,
                                   &attrib, 1, &config_id);
           if (!vaapi_check_status(status, "vaCreateConfig()"))
               return -1;
       }
       else
           config_id = vaapi->config_id;

       if (vaapi->picture_width != picture_width || vaapi->picture_height != picture_height) {
           if (vaapi->surface_id != VA_INVALID_ID)
               vaDestroySurfaces(vaapi->display, &vaapi->surface_id, 1);


           status = vaCreateSurfaces(vaapi->display, picture_width, picture_height,
                                     VA_RT_FORMAT_YUV420, 1, &surface_id);

           if (!vaapi_check_status(status, "vaCreateSurfaces()"))
               return -1;

           if (vaapi->context_id != VA_INVALID_ID)
               vaDestroyContext(vaapi->display, vaapi->context_id);
           status = vaCreateContext(vaapi->display, config_id,
                                    picture_width, picture_height,
                                    VA_PROGRESSIVE,
                                    &surface_id, 1,
                                    &context_id);
           if (!vaapi_check_status(status, "vaCreateContext()"))
               return -1;
       }
       else {
           context_id = vaapi->context_id;
           surface_id = vaapi->surface_id;
       }
       printf("---vaCreateSurfaces vaapi addr %p\n",vaapi);
       printf("---vaCreateSurfaces surface_id %d\n",surface_id);
       vaapi->config_id      = config_id;
       vaapi->context_id     = context_id;
       vaapi->surface_id     = surface_id;
       vaapi->profile        = profile;
       vaapi->entrypoint     = entrypoint;
       vaapi->picture_width  = picture_width;
       vaapi->picture_height = picture_height;

       return 0;

}

VASurfaceID CreateSurface(VAAPIContext * const vaapi)
{
    VAStatus status;
    VASurfaceID surface_id = VA_INVALID_ID;
    status = vaCreateSurfaces6(vaapi->display,
                              vaapi->picture_width,
                              vaapi->picture_height,
                              VA_RT_FORMAT_YUV420, 1, &surface_id);
    if (!vaapi_check_status(status, "vaCreateSurfaces()"))
        goto end;
//    printf("create surface %d\n",surface_id);
     return surface_id;
    end:
        vaDestroySurfaces(vaapi->display, &surface_id, 1);
    return -1;
}

void DestroySurface(VAAPIContext * const vaapi,VASurfaceID surface_id)
{
    vaDestroySurfaces(vaapi->display, &surface_id, 1);
}

int PutSurface(VAAPIContext * const vaapi,VASurfaceID surface_id,int hwnd,int w,int h)
{
    VAStatus status;

    status = vaPutSurface(vaapi->display, surface_id, hwnd,
                      0, 0, vaapi->picture_width, vaapi->picture_height,
                      0, 0, w,h,
                      NULL, 0,
                      VA_FRAME_PICTURE);
    if (!vaapi_check_status(status, "vaPutSurface()"))
        return -1;
    status = vaSyncSurface(vaapi->display,vaapi->context_id,
                               surface_id);
    if (!vaapi_check_status(status, "vaSyncSurface() for display"))
        return -1;
    return 0;
}

int vaapi_check_status(VAStatus status, const char *msg)
{
    if (status != VA_STATUS_SUCCESS) {
        fprintf(stderr, "[%s] %s: %s\n", PACKAGE_NAME, msg, vaErrorStr(status));
        return 0;
    }
    return 1;
}


