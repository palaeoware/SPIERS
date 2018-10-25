.. _slicespacing:

Slice Spacing
=============

For most datasets slice-spacing is be consistent, and setting the
*Slices/mm* value in the *Settings* tab of the *Output* panel is
sufficient. In some datasets however slice-spacing is variable, either
as a result of errors in data collection, or a change in collection
parameters at some point in the sequence. While correct slice spacings
are not always known in these circumstances, educated guesses at them
can usually be made that will improve model results.

The reconstruction approach used by SPIERS theoretically assumes
consistent splice spacing, but is flexible enough to be able modify
models to be tweaked in SPIERSview after construction to allow for some
inconsistency in slice spacing. To facilitate this SPIERSedit provides a
mechanism for modifying slice spacing. This should not be used as a
replacement for a correct *Slices/mm* setting however; to ensure the
best reconstruction this value should be set to something close to the
average spacing for all slices before exact slice positions are
subsequently tweaked.

To alter slice spacing, the *Show position in slice selector* item in
the *Slice Spacing* menu should be selected. With this option turned on,
the filenames in the *Slice Selector* panel are replaced with positions
(in mm) of the slice, relative to a 0 position inferred for notional
slice 0 (so if Slices/mm is set to 10, slice 1 will be at 0.1mm, slice 2
at 0.2mm etc).

Individual slice positions can be set by selecting the slice and using
the *Set slice position* command on the *Slice Spacing* menu. SPIERSedit
will not allow a user to set the position in front of the next slice or
behind the preceding one; if this is attempted the program will warn the
user and move the positions of other slices to maintain slice order.

If several slices share a consistent spacing, this can be set using the
*Change slice spacing* command on the *Slice Spacing* menu. Select
slices to be affected then use the command – slice spacing can be set as
an absolute value or a percentage of standard slice spacing (the
*Slices/mm* setting).

We stress again that the mechanism to deal with inconsistent slice
spacing in SPIERS is something of a bodge, and while it *will* improve
the reconstruction of inconsistently spaced data, the process of
reconstruction (using SPIERS or any other software) is eased by the
production of consistently spaced data in the first place.