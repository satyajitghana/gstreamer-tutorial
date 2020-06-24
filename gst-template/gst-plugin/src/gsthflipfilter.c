/* GStreamer
 * Copyright (C) 2020 FIXME <fixme@example.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-gsthflipfilter
 *
 * The hflipfilter element does FIXME stuff.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v fakesrc ! hflipfilter ! FIXME ! fakesink
 * ]|
 * FIXME Describe what the pipeline does.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/video/gstvideofilter.h>
#include <gst/video/video.h>

#include "gsthflipfilter.h"

GST_DEBUG_CATEGORY_STATIC(gst_hflipfilter_debug_category);
#define GST_CAT_DEFAULT gst_hflipfilter_debug_category

G_DEFINE_TYPE_WITH_CODE(GstHflipfilter, gst_hflipfilter, GST_TYPE_VIDEO_FILTER,
                        GST_DEBUG_CATEGORY_INIT(gst_hflipfilter_debug_category, "hflipfilter", 0,
                                                "debug category for hflipfilter element"));

/* prototypes */

static void gst_hflipfilter_set_property(GObject *object,
                                         guint property_id, const GValue *value, GParamSpec *pspec);
static void gst_hflipfilter_get_property(GObject *object,
                                         guint property_id, GValue *value, GParamSpec *pspec);
static void gst_hflipfilter_dispose(GObject *object);
static void gst_hflipfilter_finalize(GObject *object);

static gboolean gst_hflipfilter_start(GstBaseTransform *trans);
static gboolean gst_hflipfilter_stop(GstBaseTransform *trans);
static gboolean gst_hflipfilter_set_info(GstVideoFilter *filter, GstCaps *incaps,
                                         GstVideoInfo *in_info, GstCaps *outcaps, GstVideoInfo *out_info);
static GstFlowReturn gst_hflipfilter_transform_frame(GstVideoFilter *filter,
                                                     GstVideoFrame *inframe, GstVideoFrame *outframe);
static GstFlowReturn gst_hflipfilter_transform_frame_ip(GstVideoFilter *filter,
                                                        GstVideoFrame *frame);

enum {
    PROP_0
};

/* pad templates */

/* FIXME: add/remove formats you can handle */
#define VIDEO_SRC_CAPS                                               \
    GST_VIDEO_CAPS_MAKE(                                             \
        "{ AYUV, "                                                   \
        "ARGB, BGRA, ABGR, RGBA, Y444, xRGB, RGBx, xBGR, BGRx, "     \
        "RGB, BGR, I420, YV12, IYUV, YUY2, UYVY, YVYU, NV12, NV21, " \
        "GRAY8, GRAY16_BE, GRAY16_LE }")

/* FIXME: add/remove formats you can handle */
#define VIDEO_SINK_CAPS                                              \
    GST_VIDEO_CAPS_MAKE(                                             \
        "{ AYUV, "                                                   \
        "ARGB, BGRA, ABGR, RGBA, Y444, xRGB, RGBx, xBGR, BGRx, "     \
        "RGB, BGR, I420, YV12, IYUV, YUY2, UYVY, YVYU, NV12, NV21, " \
        "GRAY8, GRAY16_BE, GRAY16_LE }")

/* actual transformation code */
static void
gst_my_filter_flip_planar_yuv(GstHflipfilter *myfilter, GstVideoFrame *dest,
                              const GstVideoFrame *src) {
    gint x, y;
    guint8 const *s;
    guint8 *d;
    gint src_y_stride, src_u_stride, src_v_stride;
    gint src_y_height, src_u_height, src_v_height;
    gint src_y_width, src_u_width, src_v_width;
    gint dest_y_stride, dest_u_stride, dest_v_stride;
    gint dest_y_height, dest_u_height, dest_v_height;
    gint dest_y_width, dest_u_width, dest_v_width;

    src_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(src, 0);
    src_u_stride = GST_VIDEO_FRAME_PLANE_STRIDE(src, 1);
    src_v_stride = GST_VIDEO_FRAME_PLANE_STRIDE(src, 2);

    dest_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(dest, 0);
    dest_u_stride = GST_VIDEO_FRAME_PLANE_STRIDE(dest, 1);
    dest_v_stride = GST_VIDEO_FRAME_PLANE_STRIDE(dest, 2);

    src_y_width = GST_VIDEO_FRAME_COMP_WIDTH(src, 0);
    src_u_width = GST_VIDEO_FRAME_COMP_WIDTH(src, 1);
    src_v_width = GST_VIDEO_FRAME_COMP_WIDTH(src, 2);

    dest_y_width = GST_VIDEO_FRAME_COMP_WIDTH(dest, 0);
    dest_u_width = GST_VIDEO_FRAME_COMP_WIDTH(dest, 1);
    dest_v_width = GST_VIDEO_FRAME_COMP_WIDTH(dest, 2);

    src_y_height = GST_VIDEO_FRAME_COMP_HEIGHT(src, 0);
    src_u_height = GST_VIDEO_FRAME_COMP_HEIGHT(src, 1);
    src_v_height = GST_VIDEO_FRAME_COMP_HEIGHT(src, 2);

    dest_y_height = GST_VIDEO_FRAME_COMP_HEIGHT(dest, 0);
    dest_u_height = GST_VIDEO_FRAME_COMP_HEIGHT(dest, 1);
    dest_v_height = GST_VIDEO_FRAME_COMP_HEIGHT(dest, 2);

    /* Flip Y */
    s = GST_VIDEO_FRAME_PLANE_DATA(src, 0);
    d = GST_VIDEO_FRAME_PLANE_DATA(dest, 0);
    for (y = 0; y < dest_y_height; y++) {
        for (x = 0; x < dest_y_width; x++) {
            d[y * dest_y_stride + x] =
                s[y * src_y_stride + (src_y_width - 1 - x)];
        }
    }
    /* Flip U */
    s = GST_VIDEO_FRAME_PLANE_DATA(src, 1);
    d = GST_VIDEO_FRAME_PLANE_DATA(dest, 1);
    for (y = 0; y < dest_u_height; y++) {
        for (x = 0; x < dest_u_width; x++) {
            d[y * dest_u_stride + x] =
                s[y * src_u_stride + (src_u_width - 1 - x)];
        }
    }
    /* Flip V */
    s = GST_VIDEO_FRAME_PLANE_DATA(src, 2);
    d = GST_VIDEO_FRAME_PLANE_DATA(dest, 2);
    for (y = 0; y < dest_v_height; y++) {
        for (x = 0; x < dest_v_width; x++) {
            d[y * dest_v_stride + x] =
                s[y * src_v_stride + (src_v_width - 1 - x)];
        }
    }
}

static void
gst_my_filter_flip_semi_planar_yuv(GstHflipfilter *myfilter, GstVideoFrame *dest,
                                   const GstVideoFrame *src) {
    gint x, y;
    guint8 const *s;
    guint8 *d;
    gint s_off, d_off;
    gint src_y_stride, src_uv_stride;
    gint src_y_height, src_uv_height;
    gint src_y_width, src_uv_width;
    gint dest_y_stride, dest_uv_stride;
    gint dest_y_height, dest_uv_height;
    gint dest_y_width, dest_uv_width;

    src_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(src, 0);
    src_uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(src, 1);

    dest_y_stride = GST_VIDEO_FRAME_PLANE_STRIDE(dest, 0);
    dest_uv_stride = GST_VIDEO_FRAME_PLANE_STRIDE(dest, 1);

    src_y_width = GST_VIDEO_FRAME_COMP_WIDTH(src, 0);
    src_uv_width = GST_VIDEO_FRAME_COMP_WIDTH(src, 1);

    dest_y_width = GST_VIDEO_FRAME_COMP_WIDTH(dest, 0);
    dest_uv_width = GST_VIDEO_FRAME_COMP_WIDTH(dest, 1);

    src_y_height = GST_VIDEO_FRAME_COMP_HEIGHT(src, 0);
    src_uv_height = GST_VIDEO_FRAME_COMP_HEIGHT(src, 1);

    dest_y_height = GST_VIDEO_FRAME_COMP_HEIGHT(dest, 0);
    dest_uv_height = GST_VIDEO_FRAME_COMP_HEIGHT(dest, 1);

    /* Flip Y */
    s = GST_VIDEO_FRAME_PLANE_DATA(src, 0);
    d = GST_VIDEO_FRAME_PLANE_DATA(dest, 0);
    for (y = 0; y < dest_y_height; y++) {
        for (x = 0; x < dest_y_width; x++) {
            d[y * dest_y_stride + x] =
                s[y * src_y_stride + (src_y_width - 1 - x)];
        }
    }
    /* Flip UV */
    s = GST_VIDEO_FRAME_PLANE_DATA(src, 1);
    d = GST_VIDEO_FRAME_PLANE_DATA(dest, 1);
    for (y = 0; y < dest_uv_height; y++) {
        for (x = 0; x < dest_uv_width; x++) {
            d_off = y * dest_uv_stride + x * 2;
            s_off = y * src_uv_stride + (src_uv_width - 1 - x) * 2;
            d[d_off] = s[s_off];
            d[d_off + 1] = s[s_off + 1];
        }
    }
}

static void
gst_my_filter_flip_packed_simple(GstHflipfilter *myfilter, GstVideoFrame *dest,
                                 const GstVideoFrame *src) {
    gint x, y, z;
    guint8 const *s;
    guint8 *d;
    gint sw = GST_VIDEO_FRAME_WIDTH(src);
    gint sh = GST_VIDEO_FRAME_HEIGHT(src);
    gint dw = GST_VIDEO_FRAME_WIDTH(dest);
    gint dh = GST_VIDEO_FRAME_HEIGHT(dest);
    gint src_stride, dest_stride;
    gint bpp;

    s = GST_VIDEO_FRAME_PLANE_DATA(src, 0);
    d = GST_VIDEO_FRAME_PLANE_DATA(dest, 0);

    src_stride = GST_VIDEO_FRAME_PLANE_STRIDE(src, 0);
    dest_stride = GST_VIDEO_FRAME_PLANE_STRIDE(dest, 0);
    /* This is only true for non-subsampled formats! */
    bpp = GST_VIDEO_FRAME_COMP_PSTRIDE(src, 0);

    for (y = 0; y < dh; y++) {
        for (x = 0; x < dw; x++) {
            for (z = 0; z < bpp; z++) {
                d[y * dest_stride + x * bpp + z] =
                    s[y * src_stride + (sw - 1 - x) * bpp + z];
            }
        }
    }
}

static void
gst_my_filter_flip_y422(GstHflipfilter *myfilter, GstVideoFrame *dest,
                        const GstVideoFrame *src) {
    gint x, y;
    guint8 const *s;
    guint8 *d;
    gint sw = GST_VIDEO_FRAME_WIDTH(src);
    gint sh = GST_VIDEO_FRAME_HEIGHT(src);
    gint dw = GST_VIDEO_FRAME_WIDTH(dest);
    gint dh = GST_VIDEO_FRAME_HEIGHT(dest);
    gint src_stride, dest_stride;
    gint bpp;
    gint y_offset;
    gint u_offset;
    gint v_offset;
    gint y_stride;

    s = GST_VIDEO_FRAME_PLANE_DATA(src, 0);
    d = GST_VIDEO_FRAME_PLANE_DATA(dest, 0);

    src_stride = GST_VIDEO_FRAME_PLANE_STRIDE(src, 0);
    dest_stride = GST_VIDEO_FRAME_PLANE_STRIDE(dest, 0);

    y_offset = GST_VIDEO_FRAME_COMP_OFFSET(src, 0);
    u_offset = GST_VIDEO_FRAME_COMP_OFFSET(src, 1);
    v_offset = GST_VIDEO_FRAME_COMP_OFFSET(src, 2);
    y_stride = GST_VIDEO_FRAME_COMP_PSTRIDE(src, 0);
    bpp = y_stride;

    for (y = 0; y < dh; y++) {
        for (x = 0; x < dw; x += 2) {
            guint8 u;
            guint8 v;
            /* u/v must be calculated using the offset of the even column */
            gint even_x = ((sw - 1 - x) & ~1);

            u = (s[y * src_stride + even_x * bpp + u_offset] +
                 s[y * src_stride + even_x * bpp + u_offset]) /
                2;
            v = (s[y * src_stride + even_x * bpp + v_offset] +
                 s[y * src_stride + even_x * bpp + v_offset]) /
                2;

            d[y * dest_stride + x * bpp + u_offset] = u;
            d[y * dest_stride + x * bpp + v_offset] = v;
            d[y * dest_stride + x * bpp + y_offset] =
                s[y * src_stride + (sw - 1 - x) * bpp + y_offset];
            if (x + 1 < dw)
                d[y * dest_stride + (x + 1) * bpp + y_offset] =
                    s[y * src_stride + (sw - 1 - (x + 1)) * bpp + y_offset];
        }
    }
}

/* GObject vmethod implementations */
static GstCaps *
gst_myfilter_transform_caps(GstBaseTransform *trans,
                            GstPadDirection direction, GstCaps *caps, GstCaps *filter) {
    GstHflipfilter *myfilter = GST_HFLIPFILTER(trans);
    GstCaps *ret;
    gint width, height, i;

    ret = gst_caps_copy(caps);

    for (i = 0; i < gst_caps_get_size(ret); i++) {
        GstStructure *structure = gst_caps_get_structure(ret, i);
        gint par_n, par_d;

        if (gst_structure_get_int(structure, "width", &width) &&
            gst_structure_get_int(structure, "height", &height)) {
            gst_structure_set(structure, "width", G_TYPE_INT, width,
                              "height", G_TYPE_INT, height, NULL);
        }
    }

    if (filter) {
        GstCaps *intersection;
        intersection = gst_caps_intersect_full(filter, ret, GST_CAPS_INTERSECT_FIRST);
        gst_caps_unref(ret);
        ret = intersection;
    }

    return ret;
}

static void
gst_myfilter_before_transform(GstBaseTransform *trans, GstBuffer *in) {
    GstHflipfilter *myfilter = GST_HFLIPFILTER(trans);
    GstClockTime timestamp, stream_time;

    timestamp = GST_BUFFER_TIMESTAMP(in);
    stream_time =
        gst_segment_to_stream_time(&trans->segment, GST_FORMAT_TIME, timestamp);

    GST_DEBUG_OBJECT(myfilter, "sync to %" GST_TIME_FORMAT,
                     GST_TIME_ARGS(timestamp));

    if (GST_CLOCK_TIME_IS_VALID(stream_time))
        gst_object_sync_values(GST_OBJECT(myfilter), stream_time);
}

static gboolean
gst_myfilter_src_event(GstBaseTransform *trans, GstEvent *event) {
    GstHflipfilter *vf = GST_HFLIPFILTER(trans);
    gdouble new_x, new_y, x, y;
    GstStructure *structure;
    gboolean ret;
    GstVideoInfo *out_info = &GST_VIDEO_FILTER(trans)->out_info;

    GST_DEBUG_OBJECT(vf, "handling %s event", GST_EVENT_TYPE_NAME(event));

    switch (GST_EVENT_TYPE(event)) {
        case GST_EVENT_NAVIGATION:
            event =
                GST_EVENT(gst_mini_object_make_writable(GST_MINI_OBJECT(event)));

            structure = (GstStructure *)gst_event_get_structure(event);
            if (gst_structure_get_double(structure, "pointer_x", &x) &&
                gst_structure_get_double(structure, "pointer_y", &y)) {
                GST_DEBUG_OBJECT(vf, "converting %fx%f", x, y);
                new_x = out_info->width - x;
                new_y = y;
                GST_DEBUG_OBJECT(vf, "to %fx%f", new_x, new_y);
                gst_structure_set(structure, "pointer_x", G_TYPE_DOUBLE, new_x,
                                  "pointer_y", G_TYPE_DOUBLE, new_y, NULL);
            }
            break;
        default:
            break;
    }

    ret = GST_BASE_TRANSFORM_CLASS(gst_hflipfilter_parent_class)->src_event(trans, event);

    return ret;
}

static gboolean
gst_myfilter_sink_event(GstBaseTransform *trans, GstEvent *event) {
    GstHflipfilter *vf = GST_HFLIPFILTER(trans);
    GstTagList *taglist;
    gchar *orientation;
    gboolean ret;

    GST_DEBUG_OBJECT(vf, "handling %s event", GST_EVENT_TYPE_NAME(event));

    switch (GST_EVENT_TYPE(event)) {
        case GST_EVENT_TAG:
            gst_event_parse_tag(event, &taglist);
            break;
        default:
            break;
    }

    ret = GST_BASE_TRANSFORM_CLASS(gst_hflipfilter_parent_class)->sink_event(trans, event);

    return ret;
}

static gboolean
gst_myfilter_set_info(GstVideoFilter *vfilter, GstCaps *incaps,
                      GstVideoInfo *in_info, GstCaps *outcaps, GstVideoInfo *out_info) {
    GstHflipfilter *vf = GST_HFLIPFILTER(vfilter);
    gboolean ret = FALSE;

    vf->process = NULL;

    if (GST_VIDEO_INFO_FORMAT(in_info) != GST_VIDEO_INFO_FORMAT(out_info))
        goto invalid_caps;

    /* Check that they are correct */
    if ((in_info->width != out_info->width) ||
        (in_info->height != out_info->height)) {
        GST_ERROR_OBJECT(vf,
                         "we are keeping width and height but caps "
                         "are not correct : %dx%d to %dx%d",
                         in_info->width,
                         in_info->height, out_info->width, out_info->height);
        goto beach;
    }

    ret = TRUE;

    switch (GST_VIDEO_INFO_FORMAT(in_info)) {
        case GST_VIDEO_FORMAT_I420:
        case GST_VIDEO_FORMAT_YV12:
        case GST_VIDEO_FORMAT_Y444:
            vf->process = gst_my_filter_flip_planar_yuv;
            break;
        case GST_VIDEO_FORMAT_YUY2:
        case GST_VIDEO_FORMAT_UYVY:
        case GST_VIDEO_FORMAT_YVYU:
            vf->process = gst_my_filter_flip_y422;
            break;
        case GST_VIDEO_FORMAT_AYUV:
        case GST_VIDEO_FORMAT_ARGB:
        case GST_VIDEO_FORMAT_ABGR:
        case GST_VIDEO_FORMAT_RGBA:
        case GST_VIDEO_FORMAT_BGRA:
        case GST_VIDEO_FORMAT_xRGB:
        case GST_VIDEO_FORMAT_xBGR:
        case GST_VIDEO_FORMAT_RGBx:
        case GST_VIDEO_FORMAT_BGRx:
        case GST_VIDEO_FORMAT_RGB:
        case GST_VIDEO_FORMAT_BGR:
        case GST_VIDEO_FORMAT_GRAY8:
        case GST_VIDEO_FORMAT_GRAY16_BE:
        case GST_VIDEO_FORMAT_GRAY16_LE:
            vf->process = gst_my_filter_flip_packed_simple;
            break;
        case GST_VIDEO_FORMAT_NV12:
        case GST_VIDEO_FORMAT_NV21:
            vf->process = gst_my_filter_flip_semi_planar_yuv;
            break;
        default:
            break;
    }

beach:
    return ret && (vf->process != NULL);

invalid_caps:
    GST_ERROR_OBJECT(vf, "Invalid caps: %" GST_PTR_FORMAT " -> %" GST_PTR_FORMAT,
                     incaps, outcaps);
    return FALSE;
}

static GstFlowReturn
gst_myfilter_transform_frame(GstVideoFilter *vfilter,
                             GstVideoFrame *in_frame, GstVideoFrame *out_frame) {
    GEnumClass *enum_class;
    GEnumValue *active_method_enum;
    GstHflipfilter *videoflip = GST_HFLIPFILTER(vfilter);

    if (G_UNLIKELY(videoflip->process == NULL))
        goto not_negotiated;

    enum_class = g_type_class_ref(GST_TYPE_VIDEO_ORIENTATION_METHOD);
    g_type_class_unref(enum_class);

    GST_OBJECT_LOCK(videoflip);
    videoflip->process(videoflip, out_frame, in_frame);
    GST_OBJECT_UNLOCK(videoflip);

    return GST_FLOW_OK;

not_negotiated : {
    GST_ERROR_OBJECT(videoflip, "Not negotiated yet");
    return GST_FLOW_NOT_NEGOTIATED;
}
}

/* class initialization */

static void
gst_hflipfilter_class_init(GstHflipfilterClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS(klass);
    GstVideoFilterClass *video_filter_class = GST_VIDEO_FILTER_CLASS(klass);

    /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
    gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                       gst_pad_template_new("src", GST_PAD_SRC, GST_PAD_ALWAYS,
                                                            gst_caps_from_string(VIDEO_SRC_CAPS)));
    gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                       gst_pad_template_new("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
                                                            gst_caps_from_string(VIDEO_SINK_CAPS)));

    gst_element_class_set_static_metadata(GST_ELEMENT_CLASS(klass),
                                          "FIXME Long name", "Generic", "FIXME Description",
                                          "FIXME <fixme@example.com>");

    gobject_class->set_property = gst_hflipfilter_set_property;
    gobject_class->get_property = gst_hflipfilter_get_property;
    gobject_class->dispose = gst_hflipfilter_dispose;
    gobject_class->finalize = gst_hflipfilter_finalize;
    base_transform_class->start = GST_DEBUG_FUNCPTR(gst_hflipfilter_start);
    base_transform_class->stop = GST_DEBUG_FUNCPTR(gst_hflipfilter_stop);
    base_transform_class->transform_caps = GST_DEBUG_FUNCPTR(gst_myfilter_transform_caps);
    base_transform_class->before_transform = GST_DEBUG_FUNCPTR(gst_myfilter_before_transform);
    base_transform_class->src_event = GST_DEBUG_FUNCPTR(gst_myfilter_src_event);
    base_transform_class->sink_event = GST_DEBUG_FUNCPTR(gst_myfilter_sink_event);
    video_filter_class->set_info = GST_DEBUG_FUNCPTR(gst_myfilter_set_info);
    video_filter_class->transform_frame = GST_DEBUG_FUNCPTR(gst_myfilter_transform_frame);
    video_filter_class->transform_frame_ip = GST_DEBUG_FUNCPTR(gst_hflipfilter_transform_frame_ip);
}

static void
gst_hflipfilter_init(GstHflipfilter *hflipfilter) {
}

void gst_hflipfilter_set_property(GObject *object, guint property_id,
                                  const GValue *value, GParamSpec *pspec) {
    GstHflipfilter *hflipfilter = GST_HFLIPFILTER(object);

    GST_DEBUG_OBJECT(hflipfilter, "set_property");

    switch (property_id) {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

void gst_hflipfilter_get_property(GObject *object, guint property_id,
                                  GValue *value, GParamSpec *pspec) {
    GstHflipfilter *hflipfilter = GST_HFLIPFILTER(object);

    GST_DEBUG_OBJECT(hflipfilter, "get_property");

    switch (property_id) {
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
            break;
    }
}

void gst_hflipfilter_dispose(GObject *object) {
    GstHflipfilter *hflipfilter = GST_HFLIPFILTER(object);

    GST_DEBUG_OBJECT(hflipfilter, "dispose");

    /* clean up as possible.  may be called multiple times */

    G_OBJECT_CLASS(gst_hflipfilter_parent_class)->dispose(object);
}

void gst_hflipfilter_finalize(GObject *object) {
    GstHflipfilter *hflipfilter = GST_HFLIPFILTER(object);

    GST_DEBUG_OBJECT(hflipfilter, "finalize");

    /* clean up object here */

    G_OBJECT_CLASS(gst_hflipfilter_parent_class)->finalize(object);
}

static gboolean
gst_hflipfilter_start(GstBaseTransform *trans) {
    GstHflipfilter *hflipfilter = GST_HFLIPFILTER(trans);

    GST_DEBUG_OBJECT(hflipfilter, "start");

    return TRUE;
}

static gboolean
gst_hflipfilter_stop(GstBaseTransform *trans) {
    GstHflipfilter *hflipfilter = GST_HFLIPFILTER(trans);

    GST_DEBUG_OBJECT(hflipfilter, "stop");

    return TRUE;
}

static gboolean
gst_hflipfilter_set_info(GstVideoFilter *filter, GstCaps *incaps,
                         GstVideoInfo *in_info, GstCaps *outcaps, GstVideoInfo *out_info) {
    GstHflipfilter *hflipfilter = GST_HFLIPFILTER(filter);

    GST_DEBUG_OBJECT(hflipfilter, "set_info");

    return TRUE;
}

/* transform */
static GstFlowReturn
gst_hflipfilter_transform_frame(GstVideoFilter *filter, GstVideoFrame *inframe,
                                GstVideoFrame *outframe) {
    GstHflipfilter *hflipfilter = GST_HFLIPFILTER(filter);

    GST_DEBUG_OBJECT(hflipfilter, "transform_frame");

    return GST_FLOW_OK;
}

static GstFlowReturn
gst_hflipfilter_transform_frame_ip(GstVideoFilter *filter, GstVideoFrame *frame) {
    GstHflipfilter *hflipfilter = GST_HFLIPFILTER(filter);

    GST_DEBUG_OBJECT(hflipfilter, "transform_frame_ip");

    return GST_FLOW_OK;
}

static gboolean
plugin_init(GstPlugin *plugin) {
    /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
    return gst_element_register(plugin, "hflipfilter", GST_RANK_NONE,
                                GST_TYPE_HFLIPFILTER);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.0.FIXME"
#endif
#ifndef PACKAGE
#define PACKAGE "FIXME_package"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "FIXME_package_name"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "http://FIXME.org/"
#endif

GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
                  GST_VERSION_MINOR,
                  hflipfilter,
                  "FIXME plugin description",
                  plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)
