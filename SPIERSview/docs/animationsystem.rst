.. _animationsystem:

Animation System
================

SPIERSview incorporates a simple system for generating sequences of
images that can be converted (via third-party software) into video-file
animations. The commands for this system are found on the *Animation
menu*, and settings are modified in the Animation Panel (Fig. 5).

.. figure:: _static/figure_5.png
    :align: center
	
    Figure 5. Default anatomy of the SPIERSview animation pannel

SPIERS view animations are generated step-by-step by the user – each
step rotations and/or zooms to the model are performed, and optionally
an image file is saved to disk. The user may choose to modify the view
in any other way at any stage (e.g. changing transparency, performing a
dissection, etc etc) to produce more complex animations.

Understanding Image Output 
---------------------------

Note that until an output folder is specified using the *Set Image
Output Folder* menu command, all commands and settings concerned with
creating output images are disabled. Files, when created, are given a
filename consisting of *stub#####.EEE*, where *stub* is the text entered
in the ‘Filename Stub’ box of the Animation Panel, ##### is a five-digit
version of the Next File # of the Animation Panel (with leading 0’s),
and EEE is the file extension. File type and extension are chosen from
the *Set Image Output Format* submenu of the Animation Menu; note that
there are three different compression levels available for JPEG.

Images are created by direct screen capture of the Main View window –
they are thus created at whatever resolution this is currently displayed
at. It may however be convenient instead to create images at a different
resolution. To facilitate this SPIERSview incorporates a rescaling
facility. If the *Rescale Output* menu item is ticked, the output image
is rescaled to the width entered in the ‘Rescale to width’ box of the
Animation Panel (which defaults to 640 pixels). The height of the output
image will depend on the aspect ratio of thw Main View window – the
actual height which will be output if Rescale Output is turned on is
given next the to width in the Animation Panel. If the user requires
both the height *and* width to be particular values, they should set the
width and then resize the window until the height is correct.

Users should consider using Anti-aliasing (see above) when generating
animations, to maximise the quality of exported images.

The conversion of a sequence of image files into a video file is beyond
the scope of SPIERS; many third party software solutions exist for this
task. For users on a Windows platform, the authors recommend the free
version of *VideoMach* (http://gromada.com/videomach/).

Rotation and Zoom Settings
--------------------------

The rotations and zoom to be perfored in each step are specified in the
Animation Panel. The rotations are in degrees, and are around the axis
specified (the Y axis is vertical onscreen, the X axis horizontal, and
the Z axis comes directly out of the screen). The zoom is in arbitrary
units – a zoom of 10 is equivalent to zooming in one press of *Page-Up*,
and a zoom of -10 equivalent to zooming out by one press of *Page-Down.*

Commands Applying Animation Steps
---------------------------------

The *Apply Single Step* command (Ctrl-N) is the simplest animation
command – it simply applies the roatations and zoom specified in the
Animation panel. It does not generate any output files. *Apply Multiple
Steps* does the same, but asks the user for a number of steps, and
applies the animation that number of times.

The *Apply Single Step Saving Image* command (Ctrl-Shift-N) saves the
currently visible image using the rules described above, then applies
the rotations and zoom specified in the Animation panel. It also
increments the ‘Next File #’ given in the Animation Panel. Multiple
animation frames can thus be produced simply by repeated uses of this
command, although the *Apply Multiple Steps Saving Images* provides a
more convenient means of achieving the same effect. As an alternative to
the use of menus, two buttons are provided in the animation panel to
perform either single or multiple steps; a checkbox next to them
controls whether images are to be saved or not.