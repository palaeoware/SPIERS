.. _shiftingrotatingscaling:

Shifting, Rotating, And Scaling Images
======================================

Registration work consists of moving images so they line up properly; this is done by Shifting, Rotating and Scaling. Commands to do this exist on the *Transform* menu, though normally the keyboard shortcuts (given below and on the menu) should be used rather than the menu commands.

Shifting
--------

Each individual image can be shifted (moved) left, right, up or down by using the arrow keys. By default a single key press will move the image by one pixel. Pressing *Ctrl* and the arrow key will move the image by half a pixel. *Ctrl + Shift* an arrow key will move it ten pixels.

Rotating
--------

Each individual image can also be rotated about its centre either clockwise or anticlockwise. To rotate use either '0' (1° clockwise) or '1' (1° anticlockwise). Using *Ctrl* and these keys will rotate by 0.1°, and *Crtl + shift* will make the rotation 10°.

Scaling
-------

Each individual image can be rescaled (made larger or smaller). Ideal datasets should not need scale correction, but in the experience of the authors small scale errors of the order of 1% often occur in some forms of data. Normally the image should be rotated and shifted first, and only then rescaled if it is clear that this is necessary. To increase the size of the image use ']'. To decrease it, use '['. Again, *Ctrl* will rescale by a smaller factor, and *Ctrl-Shift* by a larger one.

Reset
-----

The Reset Image command on the Transform menu (Ctrl-Shift-Z) resets all rotate, scale and shift operations on the current image. A Reset Image button can also be found on the Auto Align panel (see below). A Reset Scroll Bars around Current Image command is also provided, which may be useful in datasets comprising numerous resolutions; if used frequently this may impede alignment by hampering comparison between slices.

*Note: The resampling process involved in rotating/scaling/sub-pixel shifting images will produce slight blurring. This is unavoidable, and is very unlikely to cause a problem. However, repeated rotating, scaling etc. of a single image can in theory cause a build-up of this blur effect. SPIERSalign normally avoids this problem; every time a rotate, scale or shift is performed the program goes back to the original image and does a single operation combining all rotates/scales/shifts specified so far. However, once a cropped dataset has been created (see below) the changes are permanently applied to these images. For this reason, it is NOT good practice to realign images in a dataset which has already been cropped – this may result in some images being rotated/scaled/shifted twice, introducing more substantial blurring. If a user feels that they may want to go back to a dataset to re-align it, compress it and then if necessary reload the settings file. Note that this does not apply to whole-pixel shifting (using the arrow keys), which does not introduce any blur and can be done as often as required.*