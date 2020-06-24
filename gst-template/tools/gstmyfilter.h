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
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _GST_MYFILTER_H_
#define _GST_MYFILTER_H_

#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>

G_BEGIN_DECLS

#define GST_TYPE_MYFILTER   (gst_myfilter_get_type())
#define GST_MYFILTER(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_MYFILTER,GstMyfilter))
#define GST_MYFILTER_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_MYFILTER,GstMyfilterClass))
#define GST_IS_MYFILTER(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_MYFILTER))
#define GST_IS_MYFILTER_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_MYFILTER))

typedef struct _GstMyfilter GstMyfilter;
typedef struct _GstMyfilterClass GstMyfilterClass;

struct _GstMyfilter
{
  GstVideoFilter base_myfilter;

};

struct _GstMyfilterClass
{
  GstVideoFilterClass base_myfilter_class;
};

GType gst_myfilter_get_type (void);

G_END_DECLS

#endif
