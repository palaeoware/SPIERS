.. _savingexporting:

Saving and exporting
====================

SPIERSview compact and presurfaced files (.spv)
-----------------------------------------------

When viewing an ‘spv’ format file, the *Save Changes* command (Ctrl-V)
on the *SPIERSview* menu will save the file in whatever format (compact
or presurfaced) it was opened in. SPIERSview will also ask if you want
to save changes before exiting; this also will use the format the file
was opened in. The *Save Compact As...* or *Save Presurfaced As...*
commands on the *SPIERSview* menu are also provided – these force the
file to save in whichever of these two formats you specify, and can
hence be used to convert between the two

SPIERSview finalised files (.spvf)
----------------------------------

The *Export Finalised...* command on the *SPIERSview* menu saves a copy
of the current ‘.spv’ file in Finalised format (see above), for rapid
view-only loading. As finalised format is internally implemented using
VAXML, a few restrictions apply and are checked at save-time. The most
significant of these is that all groups must have unique names; if this
is not true you will receive a warning, and the file will not be
written.

If the *Export Hidden Objects* item on the *SPIERSview* menu is ticked,
all objects are exported. If it is not ticked, the visibility of each
object is used to determine whether or not it is exported. Note that
this is simply the presence of absence of the ‘eye’ icon next to the
object in the Objects panel; an object might be set to visible (and
hence be exported) but not appear onscreen in SPIERSview because the
group it belongs to is hidden.

Groups are always exported, whether hidden or not.

VAXML/STL datasets
------------------

The *Export VAXML/STL...* command on the *SPIERSview* menu exports the
current ‘.spv’ file as a VAXML file, plus associated STL files
(SPIERSview cannot export PLY files). One STL file is created per
object, and these are placed in a subdirectory called <filename>\_stl
where <filename> is the supplied name for the VAXML file. There is no
option to export STL files without also creating a VAXML file (although
the latter can of course be manually deleted if it is not required).

The *Export Hidden Objects* item on the *SPIERSview* menu works in the
same way as for SPIERSview finalised files.

DXF files
---------

The *Export DXF...* command on the *SPIERSview* menu exports the current
‘.spv’ file as a DXF triangle mesh. Objects are exported with their
correct names, but neither groups nor colours are preserved.

The *Export Hidden Objects* item on the *SPIERSview* menu works in the
same way as for SPIERSview finalised files.

Screen Capture
--------------

The *Screen Capture...* command on the *SPIERSview* menu exports the
current Main view windows as either a JPEG (.jpg), Windows Bitmap
(.bmp), Portable Network Graphic (.png) or Tagged Image File Format
(.tiff) file. The exact image visible (at the resolution visible) is
used – for best quality remove all panels and maximise the SPIERSview
window to obtain a large view window, and use Anti-aliasing (see previous section).