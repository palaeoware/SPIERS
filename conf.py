# -*- coding: utf-8 -*-
#
# Configuration file for the Sphinx documentation builder.
#
# This file is set up to allow the sphinx-multiproject to build the docs for align, edit and view from a single repository

extensions = [
   "multiproject",
]

# Define the projects that will share this configuration file.
# These have to match the project environmental variable defined in the readthedocs admin menu (readthedocs project --> admin --> environmental variables; i.e.SPIERSalign/docs,SPIERSedit/docs or SPIERSview/docs)
multiproject_projects = {
    "SPIERSalign/docs": {
         "config": {
            "project": "SPIERS align",
            "html_title": "SPIERS align",
            "path":  "SPIERSalign/docs",
         },
    },
    "SPIERSedit/docs": {
         "config": {
            "project": "SPIERS edit",
            "html_title": "SPIERS edit",
            "path":  "SPIERSedit/docs",
         },
    },
    "SPIERSview/docs": {
         "config": {
            "project": "SPIERS view",
            "html_title": "SPIERS view",
            "path":  "SPIERSview/docs",
         },
    },
}

# Common options.
html_theme = "sphinx_rtd_theme"