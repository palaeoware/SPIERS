.. _savingexporting:

Saving and exporting
====================

SPIERSview compact and presurfaced files (.spv)
-----------------------------------------------

When viewing an 'spv' format file, the *Save Changes* command (Ctrl-V)
on the *SPIERSview* menu will save the file in whatever format (compact
or presurfaced) it was opened in. SPIERSview will also ask if you want
to save changes before exiting; this also will use the format the file
was opened in. The *Save Compact As...* or *Save Presurfaced As...*
commands on the *SPIERSview* menu are also provided - these force the
file to save in whichever of these two formats you specify, and can
hence be used to convert between the two

SPIERSview finalised files (.spvf)
----------------------------------

The *Export Finalised...* command on the *SPIERSview* menu saves a copy
of the current '.spv' file in Finalised format (see above), for rapid
view-only loading. As finalised format is internally implemented using
VAXML, a few restrictions apply and are checked at save-time. The most
significant of these is that all groups must have unique names; if this
is not true you will receive a warning, and the file will not be
written.

If the *Export Hidden Objects* item on the *SPIERSview* menu is ticked,
all objects are exported. If it is not ticked, the visibility of each
object is used to determine whether or not it is exported. Note that
this is simply the presence of absence of the 'eye' icon next to the
object in the Objects panel; an object might be set to visible (and
hence be exported) but not appear onscreen in SPIERSview because the
group it belongs to is hidden.

Groups are always exported, whether hidden or not.

VAXML/STL datasets
------------------

The *Export VAXML/STL...* command on the *SPIERSview* menu exports the
current '.spv' file as a VAXML file, plus associated STL files
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
'.spv' file as a DXF triangle mesh. Objects are exported with their
correct names, but neither groups nor colours are preserved.

The *Export Hidden Objects* item on the *SPIERSview* menu works in the
same way as for SPIERSview finalised files.

Blender / FBX export (experimental)
------------------------------------

SPIERSview also provides two newer, experimental export paths aimed at
bringing models into Blender (or other 3D applications that support FBX)
for rendering, animation or further editing outside SPIERSview.

*Export FBX...*: The *Export FBX...* command on the *SPIERSview* menu
exports all objects with geometry to a single '.fbx' file, automatically
surfacing any objects that have not yet been surfaced first if required.
Object colour, transparency and shininess are converted into FBX
material properties (colour, alpha and an approximate roughness value),
and each object's current transform (position, rotation, scale) is baked
directly into its exported vertex positions. The file is written
directly by SPIERSview - no external software is required - and can be
imported into Blender, Maya, Cinema 4D and most other applications that
support the FBX format.

*Export as Blend...*: This command goes a step further, producing a
native Blender '.blend' file directly, with materials already assigned,
avoiding the need to manually import an FBX file into Blender and
re-create its materials by hand. Internally this works by exporting a
temporary FBX file (as above), then running a local installation of
Blender in the background to convert it to '.blend' format, before
deleting the temporary FBX file. Because this relies on Blender itself,
it requires a working Blender installation on the same computer as
SPIERSview.

SPIERSview attempts to automatically detect a Blender installation (in
the system PATH, or common install locations) on startup. If none is
found, *Export as Blend...* is disabled, with its menu entry labelled
"(Blender not found)" - in this case, use *Export FBX...* instead,
followed by a manual import into Blender. If Blender is installed
somewhere SPIERSview does not detect automatically, its location can be
set manually via *Blender Path* in the *Global Settings* dialog (*File*
menu, *Advanced Prefs...*), using *Browse...* to locate the Blender
executable directly, or *Auto-Detect* to retry automatic detection.

*A note on maturity*: These two export paths are newer than SPIERSview's
other export formats and should currently be considered experimental.
The FBX writer is a bespoke implementation of the binary FBX format
(rather than using Autodesk's official FBX SDK), and the Blender
conversion step drives an external Blender installation via an embedded
Python script run in Blender's background mode; both approaches are more
sensitive to version differences and edge cases than SPIERSview's
longer-established export paths. Models with complex grouping or
material setups should be checked carefully in Blender after export, and
VAXML/STL (or OBJ) export used as a fallback if problems are
encountered.

Also note that, unlike VAXML/STL, finalised, and DXF export, *Export
FBX...*, *Export as Blend...* and *Export as OBJ...* currently export
all objects that have geometry regardless of their visibility (the 'eye'
icon in the Objects panel); the *Export Hidden Objects* setting has no
effect on these three export commands.

Screen Capture
--------------

The *Screen Capture...* command on the *SPIERSview* menu exports the
current Main view windows as either a JPEG (.jpg), Windows Bitmap
(.bmp), Portable Network Graphic (.png) or Tagged Image File Format
(.tiff) file. The exact image visible (at the resolution visible) is
used - for best quality remove all panels and maximise the SPIERSview
window to obtain a large view window, and use Anti-aliasing (see previous section).