.. _updatesystem:

Update System
=============

SPIERSedit incorporates an update-checking system (as do the other two
SPIERS applications). If connected to the Internet, it will check for
updates every time it is run, by comparing the current version against
the latest release published on the SPIERS GitHub page. This check
happens quietly in the background and does not delay or interrupt
startup; if no Internet connection is available the check is simply
skipped.

A check can also be triggered manually at any time using the *Check for
Updates...* command on the *Help* menu. Unlike the automatic startup
check, a manual check will always report its result, showing a
confirmation dialog even when no update is available.

If a newer version is found, an *Update Available* dialog is shown,
listing the files attached to that release (installers, and source code
archives) along with their sizes. Selecting *Download* for one of these
prompts for a save location, then downloads the file with a progress
dialog, showing a confirmation once complete. Downloading does **not**
automatically install the update or close SPIERSedit - the downloaded
installer must be run manually afterwards, in the same way as the
original installation. A link to the full release notes on GitHub is
also provided in this dialog.

When an update is found by the automatic startup check, the dialog
additionally offers a *Skip This Version* button; this silences the
startup prompt for that specific version only (checked again, and
reported, once a newer version is released). This option is not offered
on a manual check, since in that case the user has explicitly asked to
be informed.
