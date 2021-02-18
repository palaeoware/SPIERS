.. _multipiecedatasets:

Multi-piece datasets
====================

SPIERSview can combine two or more ‘.spv’ format files (both compact and
presurfaces varieties) into a single multi-piece model; this facility is
useful for combining parts of an object initilly reconstructed as
multiple SPIERSedit datasets (e.g. part and counterpart of a fossil).
Each ‘.spv’ file incorporated is known as a *piece*. The *Pieces panel*
(see Fig. 2) shows all the pieces making up the currently open file – if
no extra pieces have been imported, there will be only one such piece.

Items in the pieces panel by default have the filename imported – they
can be renamed (by double clicking) to provide a more helpful name.

New pieces can be imported into SPIERSview using the *Import piece*
command on the *SPIERSview* menu. These must be ‘.spv’ files, and must
be files saved by SPIERSview itself – files exported by SPIERSedit
cannot be imported directly (to get around this restriction simply load
any such file into SPIERSview, then save it again). Pieces can be
removed by selecting them in the Pieces panel and using the *Remove
Piece* command on the *SPIERSview* menu. They can also be replaced with
new versions using the *Replace Piece* command on the *SPIERSview* menu;
this preserves any re-positioning, grouping of objects, etc performed on
the piece being replaced, so is useful to re-import updated files from a
SPIERSedit dataset.

Objects from *all* pieces are shown in the objects panel. On import the
objects from a new piece will appear at the bottom, but they can be
grouped with objects from other pieces, and/or moved up and down the
list at will.

When new pieces are imported they will not typically be correctly
positioned relative to one-another. To manually reposition, the user
should use the *Reposition Selected* mode described above. To assist
with this, selecting a piece in the pieces panel automatically selects
all the objects in the piece in the Object panel, ensuring that the
correct objects are moved.

Legacy ‘.sp2’ files (see above) are opened by SPIERSview as multi-piece
files.