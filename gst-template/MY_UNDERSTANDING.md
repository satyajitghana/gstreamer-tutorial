# GStreamer Plugin - My Understanding

```c
G_DECLARE_FINAL_TYPE(GstMyFilter, gst_my_filter,
                     GST, MYFILTER, GstElement)
```

observe the above in the header file

the constructor will thus be `GST_MYFILTER`, make sure it is right

use this to inspect the plugin
```shell
gst-inspect-1.0 builddir/gst-plugin/libgsthflipfilter.so       
```

to test the pipeline
```bash
gst-launch-1.0 --gst-plugin-path=$(pwd)/builddir/gst-plugin/ videotestsrc ! hflipfilter ! autovideosink      
```

# IMPORTANT

Use this for creating templates https://github.com/GStreamer/gst-plugins-bad/tree/master/tools/element-templates
