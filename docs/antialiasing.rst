.. _antialiasing:

Anti-aliasing
=============

SPIERSview supports hardware anti-aliasing (jagged edge reduction)
through OpenGL; not all hardware will support this facility, and on some
systems antialiasing will not work in QuadBuffer Stereo mode.
Anti-aliasing is off by default, but can be set to x2, x4, x8 or x16
using the view menu. Higher anti-aliasing settings improve image quality
by smoothing jagged edges, but reduce performance and may use more
memory. Consider turning anti-aliasing on before performing screen
captures or generating animations. Changing the anti-aliasing mode
forces a full resurfacing of all objects; for compact or presurfaced
SPIERSview files (.spv format) this may take a considerable amount of
time.