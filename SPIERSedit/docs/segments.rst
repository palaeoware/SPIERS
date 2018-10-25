.. _segments:

Segments
========

Concepts
--------

*Multi-segment datasets*: In the Basic Concepts section we assumed that
data could be modelled simply by splitting into two ‘segments’ -
background ('off') and fossil ('on'); these are called single-segment
datasets in SPIERSedit (background is a special case, so not counted as
a segment). In some datasets however the situation is more complex; for
instance in Figure 11A two different types of material are present in
the fossil (light and dark grey). This is best modelled as a
multi-segment dataset *–* there are two types of material (recall that
background does not count), so it they requires *two* segments in
SPIERSedit, rather than the default one. In a multi-segment dataset each
source image has more than one corresponding working image, one for each
segment; each segment has independently specified generation rules. The
threshold image no longer consists of pixels simply ‘on’ or ‘off’;
instead each pixel is assigned to one of the segments (or none,
background). The threshold image is calculated from the working images
using the following simple rules: if a pixel is below threshold level
for all segments, it is off (black); If it is above threshold level in
only one segment it is assigned to that segment; if it is above
threshold level in more than one working image it is assigned to the
segment in with the lightest shade of grey. Figure 11B-D shows two
working images and the corresponding combined threshold image.

*Segments and Masks*: The segment and mask systems are independent and
complementary. Each pixel in each slice assigned to one segment (or
none, i.e. background, ‘off’) AND one mask. Segments are primarily used
to pick out different types of material, masks to pick out regions. The
way in which masks and segments interact to produce renderable objects
is discussed in the *Output* section below.

.. figure:: _static/figure_11.png
    :align: center
	
    Figure 11. Multi-segment dataset. A; source image (starfish arm section comprising two distinct materials). B; working image for first segment (lighter material round edge of fossil). C; working image for second segment (darker material in core of fossil). D; Threshold image showing pixels identified as light segment (white), dark segment (purple) or neither (black).

*Display colour*: Segment colour is visible in all modes except mask
mode, where mask colour is used instead, If however the *Always Show
Segments* option in the *Mode* menu is turned on, you will see segments
(in a washed-out form) in mask mode too. As with mask colour, segment
colour is simply the colour that segments appear in SPIERSedit, and has
no influence on output colour.

*Inactive and locked segments*: Like masks, segments can be locked or
hidden, though showing/hiding is termed activation/inactivation for
segments to emphasise the difference in function. Making segments
inactive essentially deletes them; SPIERSedit treats inactive segments
as though they do not exist. Unlike true segment deletion of course, a
hidden segment can be restored with a single operation. Locking a
segment stops all segment or brighten mode brush operations from
affecting any pixels currently assigned to that segment.

Working with segments
---------------------

*Segment selection:* The upper of the two drop-downs in the ‘segments’
section of the main toolbox is used to choose the segment you are
working with at the moment. This affects the operation of slice
generation, brighten mode and segmentation mode. The lower of the two
drop-downs only affects the right mouse button in segmentation mode. As
with masks, these choices are displayed as L and R (or B) in the
leftmost column of the *Segments* panel, and left/right clicking in here
will select them. Note that the R drop-down also has a ‘delete all’
option – if set to this, the right mouse will remove pixels from all
segments (i.e. turn them off).

*Brush in segment mode*: In segment mode the brush is used to assign
pixels directly to the selected segment (left mouse button) or segment
specified in the ‘R’ drop-down (right mouse button). Note that the R
drop-down also has a ‘delete all’ option – if set to this, the right
mouse will remove pixels from all segments (i.e. turn them off).

*Brush in brighten mode*: The brighten brush only affects the selected
segment. Note that brightening in a multi-segment dataset can not only
bring pixels over the threshold level so they turn on (as before), but
can also make them brighter than the corresponding pixels in other
segments so that the pixel changes to the selected segment.

*Brush in recalc mode:* This only affects the selected segment.

*Generation and segments*: Generation rules are stored on a
per-segment basis. If using linear generation for instance, one segment
can use a rule with *Invert* on to pick out dark pixels, while the other
uses a rule with *Invert* off to pick out light pixels. Selecting a
different segment (see above) will flick the settings of the
*Generation* toolbox to those setup for that segment for *Linear* and
*Polynomial* generation. *Range* generation works in a different way,
specifying a single set of rules applied to all segments (see below).

Advice on use of Segments
-------------------------

*Generation method:* For datasets based on colour source images,
carefully chosen *Linear* generation settings and/or *Polynomial*
generation are often the best way to generate working images for
multi-segment datasets. For datasets based on monochrome source images
however (e.g. CT data), multi-segment datasets are best handled with
*Range* generation (see below).

*Using segments to fill holes*: A common task with fossil datasets is to
generate a model of voids in a certain part of the model, for example
inside a fossil otherwise reconstructed with a single segment. The
segments system allows a simple trick to be used to handle this. First,
the normal segment is created and properly edited (if desired). Once
prepared, it is locked and a second ‘fill’ segment created. No
slice-based generation is performed on this second segment – slices are
left blank. To fill the holes, the first segment is locked, and the
‘fill’ segment then brushed roughly over the holes using the segment
brush.

*Number of segments:* There is no formal limit on the number of
segments, but the speed of many operations in SPIERSedit is inversely
proportional to the number of segments in use; furthermore the
disk-space used by the dataset is roughly proportional to the number of
segments. In most datasets only two or three segments will be required;
users are advised to avoid going beyond three unless they are sure they
know what they are doing.

Segment Manipulation
--------------------

Segment manipulation is primarily performed through the *Segments*
panel, which is very similar to the *Masks* panel both visually and in
operation. Some operations are performed instead through the *Segments*
Menu.

*Creating segments*: The *New* button in the *Segments* panel (or the
*New Segment* command in the *Segments* menu) creates a new segment;
this will take an appreciable amount of time as new working images will
need to be created for each source image. These are initially created
blank (as are new working images for a new dataset).

*Renaming segments*: Double click on the Segment Name in the *Segments*
panel to edit it.

*Changing colours:* Double-click the colour block to change it.

*Selecting segments in the panel*: One or more segments can be selected
by left clicking on any column of the *Segments* panel. To select
multiple segments use Ctrl-click or Shift-click. Selection is indicated
by an underlined segment name. Note that segment selection in the panel
and selection in the drop-down in the *Main Toolbox* panel are not the
same thing. Selection of segments in the panel is used for bulk locking
or activating/deactivation, bulk deleting, segment copying, and the
creation of output objects.

*Segment activation*: Double-clicking a mask’s ‘eye’ icon toggles its
activation status (visibility); this can be done to segments in bulk by
selecting them (see above) and using the *Activate Selected Segments* or
*Deactivate Selected Masks* commands on the *Segments* menu.

*Segment locking*: Double-clicking a segment’s ‘padlock’ icon toggles
its lock status; this can be done to segments in bulk by selecting them
(see above) and using the *Lock Selected Segments* or *Unlock Selected
Segments* commands on the *Segments* menu.

*Re-ordering segments list:* Segments can be moved up and down the list
by selecting a single mask and using the *Up* and *Down* buttons on the
*Segments* panel. This reordering only affects how the segments appear
in this list; it has no effect on output or images.

*Deleting segments:* It is not normal to delete a segment unless it has
been created in error; normally segments that are not required are
simply deactivated. Deleting a segment removes its associated files and
cannot be undone. To delete a segment or segments first select them (see
above), then click delete or use the *Delete Selected* *Segment(s)*
command from the *Segments* menu.