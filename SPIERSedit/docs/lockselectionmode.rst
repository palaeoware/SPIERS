.. _lockselectionmode:

Lock/Selection Mode
===================

This is by far the least used of SPIERSedit’s six brush modes. It is
used to brush a blue ‘selection’ colour onto images; this is similar to
a mask, but stored and handled separately. Each image in the dataset has
a separate lock/selection mode ‘mask’. The blue colour is applied with
the brush and the left mouse button, and removed with the right mouse
button. It is only visible in Lock/Selection mode.

Selecting pixels in this way has two applications. Firstly, it allows
selection of pixels for sample generation for *Polynomial* Generation
(see above). Secondly, selected pixels are locked for generation
operations (the *Generate* button on the *Generation* panel, or the use
of the *Auto* tick box); it is thus possible to control regions to be
affected by generation using this mode; while the *Hidden Masks Locked
for Generation* option

on the *Masks* menu can provide the same functionality through the masks
system, in some cases this is not practical without destroying a complex
existing mask structure.