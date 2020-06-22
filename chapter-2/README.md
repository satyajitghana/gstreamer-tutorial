# Chapter 2

link: https://thiblahute.github.io/GStreamer-doc/tutorials/basic/concepts.html?gi-language=python

- added `vertigotv` effect
- elements that are to be linked together have to be in the same `bin`
- many gst functions support `varargs` hence the last arg of the list of args should be `NULL`
- pipeline and bus must be unreffed

Questions
- should GstElement be unreffed as well ?