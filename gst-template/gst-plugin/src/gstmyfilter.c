/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2020  <<user@hostname.org>>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-myfilter
 *
 * FIXME:Describe myfilter here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! myfilter ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gst/gst.h>

#include "gstmyfilter.h"

GST_DEBUG_CATEGORY_STATIC(gst_my_filter_debug);
#define GST_CAT_DEFAULT gst_my_filter_debug

/* Filter signals and args */
enum {
    /* FILL ME */
    LAST_SIGNAL
};

enum {
    PROP_0,
    PROP_SILENT
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate gst_my_filter_src_template =
    GST_STATIC_PAD_TEMPLATE("src",
                            GST_PAD_SRC,
                            GST_PAD_ALWAYS,
                            GST_STATIC_CAPS(GST_VIDEO_CAPS_MAKE("{ AYUV, "
                                                                "ARGB, BGRA, ABGR, RGBA, Y444, xRGB, RGBx, xBGR, BGRx, "
                                                                "RGB, BGR, I420, YV12, IYUV, YUY2, UYVY, YVYU, NV12, NV21, "
                                                                "GRAY8, GRAY16_BE, GRAY16_LE }")));

static GstStaticPadTemplate gst_my_filter_sink_template =
    GST_STATIC_PAD_TEMPLATE("sink",
                            GST_PAD_SINK,
                            GST_PAD_ALWAYS,
                            GST_STATIC_CAPS(GST_VIDEO_CAPS_MAKE("{ AYUV, "
                                                                "ARGB, BGRA, ABGR, RGBA, Y444, xRGB, RGBx, xBGR, BGRx, "
                                                                "RGB, BGR, I420, YV12, IYUV, YUY2, UYVY, YVYU, NV12, NV21, "
                                                                "GRAY8, GRAY16_BE, GRAY16_LE }")));

#define gst_my_filter_parent_class parent_class
G_DEFINE_TYPE(GstMyFilter, gst_my_filter, GST_TYPE_ELEMENT);

static void gst_my_filter_set_property(GObject *object, guint prop_id,
                                       const GValue *value, GParamSpec *pspec);
static void gst_my_filter_get_property(GObject *object, guint prop_id,
                                       GValue *value, GParamSpec *pspec);

static void
gst_my_filter_flip_planar_yuv(GstMyFilter *myfilter, GstVideoFrame *dest,
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
gst_my_filter_flip_semi_planar_yuv(GstMyFilter *myfilter, GstVideoFrame *dest,
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
gst_my_filter_flip_packed_simple(GstMyFilter *myfilter, GstVideoFrame *dest,
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
gst_my_filter_flip_y422(GstMyFilter *myfilter, GstVideoFrame *dest,
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
    GstMyFilter *myfilter = GST_MYFILTER(trans);
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
    GstMyFilter *myfilter = GST_MYFILTER(trans);
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
    GstMyFilter *vf = GST_MYFILTER(trans);
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

    ret = GST_BASE_TRANSFORM_CLASS(parent_class)->src_event(trans, event);

    return ret;
}

static gboolean
gst_myfilter_sink_event(GstBaseTransform *trans, GstEvent *event) {
    GstMyFilter *vf = GST_MYFILTER(trans);
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

    ret = GST_BASE_TRANSFORM_CLASS(parent_class)->sink_event(trans, event);

    return ret;
}

static gboolean
gst_myfilter_set_info(GstVideoFilter *vfilter, GstCaps *incaps,
                      GstVideoInfo *in_info, GstCaps *outcaps, GstVideoInfo *out_info) {
    GstMyFilter *vf = GST_MYFILTER(vfilter);
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
    GstMyFilter *videoflip = GST_MYFILTER(vfilter);

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

/* initialize the myfilter's class */
static void
gst_my_filter_class_init(GstMyFilterClass *klass) {
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass *gstelement_class = GST_ELEMENT_CLASS(klass);
    GstBaseTransformClass *trans_class = GST_BASE_TRANSFORM_CLASS(klass);
    GstVideoFilterClass *vfilter_class = GST_VIDEO_FILTER_CLASS(klass);

    gobject_class->set_property = gst_my_filter_set_property;
    gobject_class->get_property = gst_my_filter_get_property;

    g_object_class_install_property(gobject_class, PROP_SILENT,
                                    g_param_spec_boolean("silent", "Silent", "Produce verbose output ?",
                                                         FALSE, G_PARAM_READWRITE));

    gst_element_class_set_details_simple(gstelement_class,
                                         "MyFilter",
                                         "FIXME:Generic",
                                         "FIXME:Generic Template Element",
                                         " <<user@hostname.org>>");

    // gst_element_class_add_static_pad_template(gstelement_class,
    //                                           &gst_my_filter_sink_template);
    // gst_element_class_add_static_pad_template(gstelement_class,
    //                                           &gst_my_filter_src_template);

    gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                       gst_pad_template_new("src", GST_PAD_SRC, GST_PAD_ALWAYS,
                                                            gst_caps_from_string(GST_VIDEO_CAPS_MAKE("{ AYUV, "
                                                                                                     "ARGB, BGRA, ABGR, RGBA, Y444, xRGB, RGBx, xBGR, BGRx, "
                                                                                                     "RGB, BGR, I420, YV12, IYUV, YUY2, UYVY, YVYU, NV12, NV21, "
                                                                                                     "GRAY8, GRAY16_BE, GRAY16_LE }"))));
    gst_element_class_add_pad_template(GST_ELEMENT_CLASS(klass),
                                       gst_pad_template_new("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
                                                            gst_caps_from_string(GST_VIDEO_CAPS_MAKE("{ AYUV, "
                                                                                                     "ARGB, BGRA, ABGR, RGBA, Y444, xRGB, RGBx, xBGR, BGRx, "
                                                                                                     "RGB, BGR, I420, YV12, IYUV, YUY2, UYVY, YVYU, NV12, NV21, "
                                                                                                     "GRAY8, GRAY16_BE, GRAY16_LE }"))));

    trans_class->transform_caps = GST_DEBUG_FUNCPTR(gst_myfilter_transform_caps);
    trans_class->before_transform = GST_DEBUG_FUNCPTR(gst_myfilter_before_transform);
    trans_class->src_event = GST_DEBUG_FUNCPTR(gst_myfilter_src_event);
    trans_class->sink_event = GST_DEBUG_FUNCPTR(gst_myfilter_sink_event);

    vfilter_class->set_info = GST_DEBUG_FUNCPTR(gst_myfilter_set_info);
    vfilter_class->transform_frame = GST_DEBUG_FUNCPTR(gst_myfilter_transform_frame);
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_my_filter_init(GstMyFilter *filter) {
    filter->silent = FALSE;
}

static void
gst_my_filter_set_property(GObject *object, guint prop_id,
                           const GValue *value, GParamSpec *pspec) {
    GstMyFilter *filter = GST_MYFILTER(object);

    switch (prop_id) {
        case PROP_SILENT:
            filter->silent = g_value_get_boolean(value);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

static void
gst_my_filter_get_property(GObject *object, guint prop_id,
                           GValue *value, GParamSpec *pspec) {
    GstMyFilter *filter = GST_MYFILTER(object);

    switch (prop_id) {
        case PROP_SILENT:
            g_value_set_boolean(value, filter->silent);
            break;
        default:
            G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
            break;
    }
}

/* GstElement vmethod implementations */

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
myfilter_init(GstPlugin *myfilter) {
    /* debug category for fltering log messages
   *
   * exchange the string 'Template myfilter' with your description
   */
    GST_DEBUG_CATEGORY_INIT(gst_my_filter_debug, "myfilter",
                            0, "Template myfilter");

    return gst_element_register(myfilter, "myfilter", GST_RANK_NONE,
                                GST_TYPE_MYFILTER);
}

/* PACKAGE: this is usually set by meson depending on some _INIT macro
 * in meson.build and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use meson to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "myfirstmyfilter"
#endif

/* gstreamer looks for this structure to register myfilters
 *
 * exchange the string 'Template myfilter' with your myfilter description
 */
GST_PLUGIN_DEFINE(
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    myfilter,
    "Template myfilter",
    myfilter_init,
    PACKAGE_VERSION,
    GST_LICENSE,
    GST_PACKAGE_NAME,
    GST_PACKAGE_ORIGIN)
