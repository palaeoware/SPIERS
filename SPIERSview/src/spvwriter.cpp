#include <QFile>
#include <QMessageBox>

#include "spvwriter.h"
#include "globals.h"
#include "vaxml.h"
#include "svobject.h"
#include "staticfunctions.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief SPVWriter::SPVWriter
 */
SPVWriter::SPVWriter()
{

}


/**
 * @brief SPVWriter::writeFile
 * @param withpd
 */
void SPVWriter::writeFile(bool withPolydata)
{
    writeSPV(withPolydata);
}

/**
 * @brief writeFinalised
 */
void SPVWriter::writeFinalised()
{
    QFile OutputFile(fname);
    if (OutputFile.open(QIODevice::WriteOnly) == false)
    {
        QMessageBox::warning(mainWindow, "File Error", "Can't open SPV file for writing - is it set to read-only?");
        return;
    }

    VAXML v;
    if (v.writeVAXML(fname, true) == false) return;

    //Add the STLs
    //2. Write VAXML in text mode
    //3. Write compressed STLs
}

/**
 * @brief SPVWriter::writeSPV
 * Write new version of SPV out
 * @param withPolydata
 */
void SPVWriter::writeSPV(bool withPolydata)
{
    bool tempbool;

    QFile OutputFile(fname);
    if (OutputFile.open(QIODevice::WriteOnly) == false)
    {
        QMessageBox::warning(mainWindow, "File Error", "Can't open SPV file for writing - is it set to read-only?");
        return;
    }

    QDataStream out(&OutputFile);
    out.setByteOrder(QDataStream::LittleEndian);
    out.setVersion(QDataStream::Qt_4_4);

    // SPV v1 files do not have a versioning system.
    // All versions >= 2 start with a -1 followed by the real
    // version number.
    out << static_cast<double>(-1);
    out << static_cast<int>(SPVFILEVERSION);

    // Total count of objects (for progress bar)
    int c = 0;
    out << SPVs.count();
    for (int i = 0; i < SPVs.count(); i++)
        c += SPVs[i]->ComponentObjects.count();
    out << c;

    // Save memory is checked
    out << mainWindow->ui->actionSave_Memory->isChecked();

    // Some global settings... maybe just matrix for now
    for (int i = 0; i < SPVs.count(); i++) //NEW - do each SPV
    {
        SPV *s = SPVs[i];
        out << s->filenamenopath; //NEW
        out << s->ComponentObjects.count();
        out << s->PixPerMM;
        out << s->SlicePerMM;
        out << s->SkewDown / s->PixPerMM;;
        out << s->SkewLeft / s->SlicePerMM;
        out << s->iDim;
        out << s->jDim;
        out << s->kDim;
        out << s->MirrorFlag;

        for (int i = 0; i < s->kDim + 1; i++)
            out << s->stretches[i];

        //NEW - removed matrix for SPV
        //Now the component object details
        for (int i = 0; i < s->ComponentObjects.count(); i++)
        {
            SVObject *o = s->ComponentObjects[i];
            out << o->Name;
            out << o->Key;
            out << o->Colour[0];
            out << o->Colour[1];
            out << o->Colour[2];
            if (o->Transparency < 0)
                out << static_cast<int>(0);
            else
                out << o->Transparency; //bodge for backward compatibility
            out << o->IsGroup;
            out << o->InGroup;
            out << o->Position;
            out << o->Visible;
            out << o->Resample;
            if (o->IslandRemoval < 0)
                out << static_cast<int>(0);
            else
                out << o->IslandRemoval; //bodge for backward compatibility
            if (o->Smoothing < 0)
                out << static_cast<int>(0);
            else
                out << o->Smoothing; //bodge for backward compatibility

            if (o->buggedData) out << o->ResampleType - 10000;
            else out << o->ResampleType;

            // Write martix - transpose it to write
            StaticFunctions::transposeMatrix(o->matrix);
            for (int i = 0; i < 16; i++)
                out << o->matrix[i];
            // ... and Transpose it back so we can keep using it
            StaticFunctions::transposeMatrix(o->matrix);

            out << o->Index;
            out << o->scale;
        }

        //Now the data
        for (int i = 0; i < s->ComponentObjects.count(); i++)
        {
            SVObject *o = s->ComponentObjects[i];
            if (!(o->IsGroup))
            {
                if (o->AllSlicesCompressed) //Not done by slice
                {
                    out << -3;
                    out << o->AllSlicesSize;
                    OutputFile.write(reinterpret_cast<char *>(o->AllSlicesCompressed), o->AllSlicesSize);
                }
                else
                {
                    out << (s->kDim - 2); //this is the number of pieces thing - always kdim -2 on a writeback
                    //we know how many slice there will be - it's Kdim

                    for (int j = 0; j < s->kDim - 2; j++) //allow for two end caps
                    {
                        if (o->compressedslices.count() == 0) //<v5, no comp slices
                            out << static_cast<int>(-1);
                        else
                        {
                            CompressedSlice *sl = o->compressedslices[j + 1];

                            if (sl->empty)
                            {
                                //no data, just write a -1
                                out << static_cast<int>(-1);
                            }
                            else
                            {
                                out << static_cast<int>(0); //code for there is a grid and then data
                                //write the grid
                                out.writeRawData(reinterpret_cast<char *>(sl->grid), s->GridSize);
                                //now write the compressed data
                                out << sl->datasize;
                                out.writeRawData(reinterpret_cast<char *>(sl->data), sl->datasize);
                            }
                        }

                    }
                }
                //and now optionally write the polydata object too
                out << withPolydata;
                if (withPolydata) o->WritePD(&OutputFile);
            }
        }

    }

    //Now the orphan objects - which can only be groups
    QList <SVObject *> AllObs;
    AllObs = SVObjects;
    for (int i = 0; i < SPVs.count(); i++)
        for (int j = 0; j < SPVs[i]->ComponentObjects.count(); j++)
        {
            int p = AllObs.indexOf(SPVs[i]->ComponentObjects[j]);
            if (p == -1) exit(20);
            AllObs.removeAt(p);
        }

    // AllObs should now just list orphan groups
    out << AllObs.count();
    for (int i = 0; i < AllObs.count(); i++)
    {
        SVObject *o = AllObs[i];
        out << o->Name;
        out << o->Key;
        out << o->IsGroup;
        out << o->InGroup;
        out << o->Position;
        out << o->Visible;
        out << o->Index;
    }

    // Info lists
    out << QString("InfoLists");
    out << infoComments;
    out << infoReference;
    out << infoAuthor;
    out << infoSpecimen;
    out << infoProvenance;
    out << infoClassificationName;
    out << infoClassificationRank;
    out << infoTitle;

    // Scale ball color and matrix - this is no longer needed as scale ball has been removed
    out << scaleBallColour[0];
    out << scaleBallColour[1];
    out << scaleBallColour[2];
    out << scaleBallScale; //resize applied to ball - for scale calc
    for (int i = 0; i < 16; i++) out << scaleMatrix[i];

    // Is Quadric fedelity checked?
    tempbool = mainWindow->ui->actionQuadric_Fidelity_Reduction->isChecked();
    out << tempbool;

    // Is Scale ball shown? - this is no longer needed as scale ball has been removed
    tempbool = mainWindow->ui->actionShow_Ball_2->isChecked();
    out << tempbool;

    // Background colour
    out << colorBackgroundRed << colorBackgroundGreen << colorBackgroundBlue;

    // Scale Grid - colour, options, fonts
    out << colorGridRed << colorGridGreen << colorGridBlue;
    out << colorGridMinorRed << colorGridMinorGreen << colorGridMinorBlue;
    out << fontSizeGrid << showMinorGridLines << showMinorGridValues << showScaleGrid;

    // This tagged on the end to keep some sort of file compatibility
    for (int i = 0; i < SPVs.count(); i++) //NEW - do each SPV
    {
        SPV *s = SPVs[i];
        for (int i = 0; i < s->ComponentObjects.count(); i++)
        {
            SVObject *o = s->ComponentObjects[i];
            out << o->Shininess;
            out << o->Transparency; //these might now be <0
            out << o->IslandRemoval; //these might now be <0
            out << o->Smoothing; //these might now be <0
        }
    }

    // Show saved message
    mainWindow->ui->statusBar->showMessage("Save Complete");
}
