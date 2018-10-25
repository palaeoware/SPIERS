.. _advancedprefs:

Advanced Prefs
==============

The *Advanced Prefs* option on the *File* menu give access to a dialog
with some of SPIERS' more esoteric settings. These are global (rather
than per-dataset) settings; some only take effect when datasets are
opened (e.g. maximum memory for undo and cache).

*Maximum memory for undo*: Controls the maximum size of the undo stack.
As the stack is wiped every time a multi-slice operation is executed,
this is normally limited to the changes to a single slice anyway, and
thus the default of 512 Mb is ample. For computers with limited RAM this
can safely be reduced, although of course this means that fewer Undo’s
will be possible.

*Undo Timer Interval*: Determines how many seconds' worth of actions are
clumped into a single undo event. If the amount of memory for the undo
stack is reduced, it may be worth increasing this value to allow bigger
undo steps and hence to allow stepping further back in time.

*Maximum memory for cache*: SPIERSedit keeps copies of all files it
loads (source images, working images etc) in a ‘cache’. Use of this
cache greatly speeds subsequent access to files. The maximum size for
the cache is set here; it is recommended that it should be set as high
as the system reasonably allows; approximately one half of total system
RAM might be a good guideline. Be aware that this may make the opening
of multiple copies of SPIERSedit problematic.

*Tune Cache for Rendering*: If ticked, the cache preferentially stores
working images, mask data etc rather than source images. If the latter
are relatively large (e.g. colour images at twice resolution of working
images) this can give substantial performance increases while rendering,
as it may enable the entire dataset to fit within the cache. The
downside is that it will reduce performance for ‘leafing through’ the
dataset.

*File compression*: Controls how working images, mask data and
lock/selection data are written to disk. Setting to ‘off’ produces large
files, but normally results in faster disk operations. If set to ‘low’
or ‘high’ these files are compressed when written back to disk (i.e. the
compression only happens if/when they change). ‘low’ compression offers
substantial space savings over ‘none’ (typically a factor of 5-10),
‘high’ may increase compression factor a little over ‘low’. High
settings produce extra processor load (compression/decompressing data),
so will normally slow many operations down. The default setting of ‘low’
will be best in most cases.

*(Re)compress source files now:* Allows the user to recompress the
source images; this is the only exception to SPIERSedit’s rule that it
never changes source images, so should be used with caution. Behaviour
is controlled by the file compression slider - if this is ‘off’ then
this button converts all source images to ‘.bmp’ format. If slider is
set to ‘low’ images are converted to ‘.png’ format (normally around one
half of the size of ‘.bmp’ files. If the slider is set to ‘high’ images
are converted to ‘.jpg’ format, which will be substantially smaller
again. ‘.jpg’ images are saved at maximum fidelity, so artefacts should
be negligible; repeated conversion back and forward between high and
low/off is not recommended though, as this could build up artefacts with
each conversion. Compression of source images to ‘jpg’ format is not
normally recommended, but may be an acceptable means of saving disk
space once the user is confident that all generation operations have
been completed.

*(Re)compress working files now:* Applies the current file compression
level (see above) to all working files.

*Cache compression*: SPIERSedit can compress files when storing them in
its internal cache (see above). This trades off processing speed against
memory usage, but it may enable the entire dataset to fit in the cache,
which can in itself increase performance substantially. If cache
compression is set to low around 4-5 times more files will fit in the
cache – it set to high this figure will reach perhaps 7-8 times. Note
that if you set cache compression to 'high' then colour images are
cached using the lossy JPEG algorithm - there may be slight artefacts
which can in theory (though probably not in practice) make their way
into your datasets via slice-generation. Cache Compression should
normally be turned off, but may be worth trying on machines with limited
memory. Note that changing this setting empties the cache, so will not
provide an instant performance increase!

*Autosave frequency*: Controls how often the SPIERSedit settings file
(.spe) is written out.

*Clear Recent Files List*: This button removes everything from the *Open
Recent* list on the *File* menu.