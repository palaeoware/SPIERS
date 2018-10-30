#include "spvreader.h"
#include <stdio.h>
#include <string.h>
#include <QMessageBox>
#include <QFile>
#include <QDataStream>
#include <QTextStream>
#include <QTime>
#include <QDebug>

#include "globals.h"
#include "spv.h"
#include "marchingcubes.h"
#include "zlib.h"
#include "svobject.h"
#include "compressedslice.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vtkProperty2D.h"
#include "vaxml.h"
#include "staticfunctions.h"

/**
 * @brief SPVReader::SPVReader
 */
SPVReader::SPVReader()
{
    replaceIndex = -1;
}

/**
 * @brief SPVReader::processFileReplacement
 * @param filename
 * @param SPVindex
 * @return
 */
int SPVReader::processFileReplacement(QString filename, int spvIndex)
{
    currentFilename = filename;

    replaceIndex = spvIndex;
    internalProcessFile(currentFilename);
    mainWindow->EnableRenderCommands();
    return replaceIndex; //may well be an error code -2 = multipart, -3 = too old
}

/**
 * @brief SPVReader::processFile
 * @param filename
 */
void SPVReader::processFile(QString filename)
{
    //The old processfile stuff. This is the only external API function to the class
    //set the class-level globals
    currentFilename = filename;

    //call internal version with messier interface
    internalProcessFile(currentFilename);

    mainWindow->EnableRenderCommands();
}

/**
 * @brief SPVReader::fixKeyCodeData
 * Bodge to fix up any problems with key codes
 */
void SPVReader::fixKeyCodeData()
{
    //Fix any odd characters in keys
    for (int i = 0; i < SVObjects.count(); i++)
    {
        int key = static_cast<int>(SVObjects[i]->Key.toLatin1());
        if (key >= 97 && key <= 122) SVObjects[i]->Key = (QChar(key - 64)); //to Upper
        if (key < 48 || key > 90) SVObjects[i]->Key = 0;
        if (key >= 58 && key <= 64) SVObjects[i]->Key = 0;
    }
}



/**
 * @brief SPVReader::fileReadFailed
 * @param fname
 * @param write
 * @param n
 */
void SPVReader::fileReadFailed(QString fname, bool write, int n)
{
    //qDebug() << "[Where I'm I?] In fileReadFailed";

    QString message;
    if (write)
        message = QString("Error code %1 - could not open file %2 for writing - it may be write-protected").arg(n).arg(fname);
    else
        message = QString("Error code %1 - could not open file %2 - does it exist?").arg(n).arg(fname);


    //qDebug() << "[Where I'm I?] In fileReadFailed - something has gone wrong, exiting! | meesage = " << message;
    QMessageBox::warning(static_cast<QWidget *>(mainWindow), "File Error", message);

    QCoreApplication::quit();
}

/**
 * @brief SPVReader::internalProcessFile
 * This function checks the file exension to see if it is an SP2 or SPV file. If SP2
 * the file is processed to extract the SPV file names. Each SPV is then processed in turn.
 * If SPV the single file is processed.
 * @param filename
 */
void SPVReader::internalProcessFile(QString filename)
{
    //qDebug() << "[Where I'm I?] In internalProcessFile";

    QFileInfo fi(filename);

    QString path = fi.absolutePath();
    QString fname = fi.fileName();

    isSP2 = (fi.suffix() == "sp2");
    if (!isSP2)
    {
        //qDebug() << "[Where I'm I?] In internalProcessFile - about to call processSPV with NON .sp2 file | filecount = 0 | matrix = nullptr";
        processSPV(filename, nullptr);
        return;
    }
    else
    {
        char buffer[1024];
        sp2Lock = true; //lock all interactions
        QFile in(filename);
        if (!in.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            fileReadFailed(filename, false, 13);
            return;
        }

        int filecount = 0;
        //Read main array of SPVs, brights and matrices
        do
        {
            do
            {
                if (in.readLine(buffer, 200) <= 0) return; //get line - error if nothing returned
            }
            while (buffer[0] == '\n' || buffer[0] == ' ' || buffer[0] == '\r'); //if it's just a return try again

            while (buffer[strlen(buffer) - 1] == '\n' || buffer[strlen(buffer) - 1] == '\r')
                buffer[strlen(buffer) - 1] = 0; //lose the newline character

            char namebuff[200];
            strcpy(namebuff, buffer);

            if (strcmp("END", buffer) == 0 && filecount == 0) return; //Error - sp2 file does not refer to any spv files
            if (strcmp("END", buffer) != 0)
            {
                //float bright;
                float matrix[16];
                QString d(in.readLine());
                //bright = d.toFloat();
                for (int m = 0; m < 16; m++)
                {
                    QString d(in.readLine());
                    matrix[m] = d.toFloat();
                }

                QString spvname = path + "/" + QString(namebuff);

                //qDebug() << "[Where I'm I?] In internalProcessFile - about to call processSPV with .sp2 file | filecount = " << filecount << " | matrix = " << matrix;
                processSPV(spvname, matrix);
            }
            filecount++;
        }
        while (strcmp(buffer, "END"));

        //work out proper scale - from FIRST Spv (maybe this one, maybe not)
        mmPerUnit = (static_cast<float>(SPVs[0]->iDim) / static_cast<float>(SCALE)) / static_cast<float>(SPVs[0]->PixPerMM);

        sp2Lock = false;
    }
}

/**
 * @brief SPVReader::processSPV
 * @param filename
 * @param index
 * @param PassedMatrix
 * @return
 */
int SPVReader::processSPV(QString filename, float *passedMatrix = nullptr)
{
    //qDebug() << "[Where I'm I?] In processSPV | filename = " << filename << "; index = " << index << "; passedMatrix = " << PassedMatrix;

    int version = 1;
    double p1;
    FILE *file;
    int errnum = 0;

    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    //qDebug() << "File Handle = " << f.handle();
    file = fdopen(f.handle(), "rb");

    if (file == nullptr)
    {
        fileReadFailed(filename, false, errnum);
        return 1;
    }

    //read all the parameters in
    fread(&p1, 8, 1, file);
    StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&p1), 8);

    // v1 SPV files lacked a versioning system...
    // ... so work around is for first param to be negative in v2 and up. For v2
    // and up, next param is int version followed by real p1.
    // Note 1: version numbers >= 1000 are reserved for SPIERSedit generated files
    // that have not yet been resaved by SPIERSview
    // Note 2: v5 was first QT one; v4 was last one from VB; v3 is last Mac one;
    // v6 includes grid/flag support. (this note is out fo date)
    if (p1 < 0)
    {
        fread(&version, sizeof(int), 1, file);
        StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&version), 4);
        fread(&p1, 8, 1, file);
        StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&p1), 8);
    }
    //qDebug() << "[Version] " << version;
    fclose(file);

    // We have two main types of SPV file, <= v5 uses the older style, while
    // >= v6 uses the newer style. Here we use the worked out version to select
    // which function to call.
    if (version > 5 && version < 1000)
    {
        //qDebug() << "[Version] Reading >= v6 SPV file";
        version6Plus(filename);
        return 0;
    }
    else
    {
        //qDebug() << "[Version] Reading <= v5 SPV file";
        version5Below(filename, passedMatrix);
        return 1;
    }
}

/**
 * @brief SPVReader::version6Plus
 * This function reads and processes all <= v5 SPV files.
 * @param Filename
 */
void SPVReader::version5Below(QString filename, float *passedMatrix = nullptr)
{
    double p1;
    double p2;
    double p3;
    double p4;
    int n;
    int version = 1;
    int dummyint;
    int fwidth; //these need to be 32 bit ints
    int fheight; //these need to be 32 bit ints
    int filesused; //these need to be 32 bit ints
    int items; //these need to be 32 bit ints
    int bsize; //these need to be 32 bit ints
    unsigned char *tarray;
    unsigned char *fullarray; //this is legacy
    uLongf size;
    double temp;
    char dummy[4096];
    int slen;
    FILE *file;
    short OutKeys[201]; // these must be 16 bit ints
    short OutResamples[201]; // these must be 16 bit ints
    short OutColours[201 * 3]; // these must be 16 bit ints
    int BaseIndex;
    int firstgroup;
    int errnum = 0;

    QFile f(filename);
    f.open(QIODevice::ReadOnly);
    file = fdopen(f.handle(), "rb");

    if (file == nullptr)
    {
        fileReadFailed(filename, false, errnum);
        return;
    }

    // Read all the parameters in
    fread(&p1, 8, 1, file);
    StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&p1), 8);

    // v1 SPV files lacked a versioning system...
    // ... so work around is for first param to be negative in v2 and up. For v2
    // and up, next param is int version followed by real p1.
    // Note 1: version numbers >= 1000 are reserved for SPIERSedit generated files
    // that have not yet been resaved by SPIERSview
    // Note 2: v5 was first QT one; v4 was last one from VB; v3 is last Mac one;
    // v6 includes grid/flag support. (this note is out fo date)
    if (p1 < 0)
    {
        fread(&version, sizeof(int), 1, file);
        StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&version), 4);
        fread(&p1, 8, 1, file);
        StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&p1), 8);
    }

    if (replaceIndex >= 0)
    {
        // Can't replace with a pre-v6 spv return -3 error code
        replaceIndex = -3;
        return;
    }

    fread(&p2, 8, 1, file);
    StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&p2), 8);
    fread(&p3, 8, 1, file);
    StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&p3), 8);
    fread(&p4, 8, 1, file);
    StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&p4), 8);
    fread(&fwidth, 4, 1, file);
    StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&fwidth), 4);
    fread(&fheight, 4, 1, file);
    StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&fheight), 4);
    fread(&filesused, 4, 1, file);
    StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&filesused), 4);
    fread(&items, 4, 1, file);
    StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&items), 4);

    SPV *thisspv = new SPV(SPVs.count(), version, fwidth, fheight, filesused);
    SPVs.append(thisspv);
    thisspv->filename = filename;

    // Get filename without path
    QString fn = filename;
    fn = fn.mid(qMax(fn.lastIndexOf("\\"), fn.lastIndexOf("/")) + 1);
    thisspv->filenamenopath = fn;

    // Store these in the SPV object
    thisspv->PixPerMM = p1;
    thisspv->SlicePerMM = p2;
    thisspv->SkewDown = -p3 * p1;
    thisspv->SkewLeft = -p4 * p1;

    mmPerUnit = (static_cast<float>(fwidth) / static_cast<float>(SCALE)) / static_cast<float>(thisspv->PixPerMM);

    // Create and append all the SVObjects
    for (int i = 0; i < items; i++)
    {
        SVObject *newobj = new SVObject(SVObjects.count());
        newobj->spv = thisspv; //pointer in object to SPV
        thisspv->ComponentObjects.append(newobj); //pointer in SPV to object
        SVObjects.append(newobj); //put it in my general list
        newobj->Index = SVObjects.count() - 1;
    }

    // Keys array - currently there are 201
    fread(OutKeys, sizeof(OutKeys), 1, file);
    for (n = 0; n < 201; n++) StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&OutKeys[n]), 2);
    fread(OutColours, sizeof(OutColours), 1, file);
    for (n = 0; n < 201 * 3; n++) StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&OutColours[n]), 2);
    fread(OutResamples, sizeof(OutResamples), 1, file);
    for (n = 0; n < 201; n++) StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&OutResamples[n]), 2);

    // Put these in the new structures
    for (int i = 0; i < items; i++)
    {
        thisspv->ComponentObjects[i]->Key = static_cast<QChar>(OutKeys[i]);
        thisspv->ComponentObjects[i]->Colour[0] = static_cast<uchar>(OutColours[i]);
        thisspv->ComponentObjects[i]->Colour[1] = static_cast<uchar>(OutColours[i + 201]);
        thisspv->ComponentObjects[i]->Colour[2] = static_cast<uchar>(OutColours[i + 201 * 2]);
        thisspv->ComponentObjects[i]->Resample = static_cast<int>(OutResamples[i]);
        if (passedMatrix)
            for (int j = 0; j < 16; j++)
            {
                thisspv->ComponentObjects[i]->matrix[j] = passedMatrix[j];
            }
        StaticFunctions::transposeMatrix(thisspv->ComponentObjects[i]->matrix);

        for (int j = 0; j < 16; j++)
            thisspv->ComponentObjects[i]->defaultmatrix[j] = thisspv->ComponentObjects[i]->matrix[j];

        thisspv->ComponentObjects[i]->gotdefaultmatrix = true;
    }

    // Handle stretch array (v2+ only)
    double *stretches = reinterpret_cast<double *>(malloc((static_cast<unsigned long long>(filesused) + 1) * sizeof(double)));
    thisspv->stretches = stretches;
    if (version > 1)
    {
        //Read stretches array
        fread(stretches, static_cast<unsigned long long>(filesused) * sizeof(double), 1, file);
        for (n = 0; n < filesused; n++)
            StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&stretches[n]), sizeof(double));
        temp = stretches[n];
    }
    else // V1.0 - must construct a stretcharray with no changes
    {
        temp = 0; //construct a no-stretch array
        for (n = 0; n < filesused; n++) stretches[n] = temp++;
    }

    // Extra stretches item
    stretches[filesused] = stretches[filesused - 1] + 1;

    // v3 and up: model mirrored in y to correct for BMP inversion.
    if (version > 2) thisspv->MirrorFlag = true;
    else thisspv->MirrorFlag = false;

    if (version < 5) // old file, need old style large array
    {
        //alloc the main array
        if ((fullarray = reinterpret_cast<unsigned char *>(malloc(static_cast<unsigned long long>(filesused) * static_cast<unsigned long long>(fwidth) * static_cast<unsigned long long>(fheight)))) == nullptr)
        {
            QMessageBox::warning(static_cast<QWidget *>(mainWindow), "Memory Error", "Fatal Error - could not obtain enough memory to reconstruct volume.\nTry exporting from a newer version of SPIERSview");
            QCoreApplication::quit();
        }

        //ensure top and bottom are blank
        for (int i = 0; i < fwidth * fheight; i++)
            fullarray[i] = 0;

        unsigned char *endfullarray = fullarray + (fwidth * fheight) * (filesused - 1);

        for (int i = 0; i < fwidth * fheight; i++)
            endfullarray[i] = 0;
    }
    else fullarray = nullptr; //no fullarrray

    // Previously made blank slices here - now we just have a blank flag in the compressedslices object
    for (int m = 0; m < items; m++) //for each item in the file
    {
        QString status;
        status.sprintf("Processing object %d of %d", m + 1, items);
        mainWindow->ui->OutputLabelOverall->setText(status);
        mainWindow->ui->ProgBarOverall->setValue((m * 100) / items);

        mainWindow->setSpecificLabel("Preprocessing Data");
        qApp->processEvents();

        SVObject *thisobj = thisspv->ComponentObjects[m];

        if (version < 4) thisobj->buggedData = true;
        if (version > 4)
        {
            //Chunked compression - build into fullarray
            int pieces;

            //read the number of pieces of the file - ought to be same as file count, but no chances!
            fread(&pieces, 4, 1, file);
            StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&pieces), 4);

            CompressedSlice *blankslice = new CompressedSlice(thisobj, true);
            (thisobj->compressedslices).append(blankslice);
            int SlicePointer = 1;

            for (int p = 0; p < pieces; p++)
            {
                mainWindow->setSpecificProgress((p * 100) / pieces);
                if (p % 10 == 0) qApp->processEvents();

                if (p % (filesused - 2) == 0) SlicePointer = 1; // reached a restart from merge

                //read the size
                fread(&bsize, 4, 1, file);
                StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&bsize), 4);

                CompressedSlice *s = new CompressedSlice(thisobj, false);

                if (bsize == -1) //No data - flag as empty
                    s->empty = true;
                else
                {
                    tarray = reinterpret_cast<unsigned char *>(malloc(static_cast<size_t>(bsize)));
                    fread(tarray, static_cast<size_t>(bsize), 1, file);
                    s->datasize = bsize;
                    s->data = tarray;

                    s->grid = reinterpret_cast<unsigned char *>(malloc(static_cast<size_t>(thisspv->GridSize)));
                    if (version >= 1000) //read the grid
                        fread(s->grid, static_cast<size_t>(thisspv->GridSize), 1, file);
                    //if no grid all grid squares are on
                    else for (int i = 0; i < thisspv->GridSize; i++) s->grid[i] = static_cast<uchar>(255);
                }

                if (p < (filesused - 2)) //no merge complications
                {

                    (thisobj->compressedslices).append(s);
                }
                else
                {
                    QString fname;
                    fname = QString(QString(TESTDUMPLOCATION) + "cdump_%1_%2_a")
                            .arg(p, 3, 10, QChar('0')).arg(SlicePointer, 3, 10, QChar('0'));
                    thisobj->compressedslices[SlicePointer]->dump(fname);
                    fname = QString(QString(TESTDUMPLOCATION) + "cdump_%1_%2_b")
                            .arg(p, 3, 10, QChar('0')).arg(SlicePointer, 3, 10, QChar('0'));
                    s->dump(fname);
                    fname = QString(QString(TESTDUMPLOCATION) + "cdump_%1_%2_d")
                            .arg(p, 3, 10, QChar('0')).arg(SlicePointer, 3, 10, QChar('0'));
                    thisobj->compressedslices[SlicePointer]->merge(s, fname);
                    fname = QString(QString(TESTDUMPLOCATION) + "cdump_%1_%2_c")
                            .arg(p, 3, 10, QChar('0')).arg(SlicePointer, 3, 10, QChar('0'));
                    thisobj->compressedslices[SlicePointer]->dump(fname);
                    delete s;
                }
                SlicePointer++;
            }

            blankslice = new CompressedSlice(thisobj, true);
            (thisobj->compressedslices).append(blankslice);
        }
        else
        {
            //old style - one chunk
            //get the compressed file size

            fread(&bsize, 4, 1, file);
            StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&bsize), 4);

            //alloc and read in
            tarray = reinterpret_cast<unsigned char *>(malloc(static_cast<size_t>(bsize)));
            fread(tarray, static_cast<size_t>(bsize), 1, file);

            //uncompress it
            size = static_cast<unsigned long>(filesused * fwidth * fheight);
            uncompress(fullarray, &size, tarray, static_cast<unsigned long>(bsize));
            thisobj->AllSlicesCompressed = tarray;
            thisobj->AllSlicesSize = bsize;
            thisspv->fullarray = fullarray;
        }

        int TrigCount;
        double *TrigArray = nullptr;
        if (version > 3) //contains spline info
        {
            fread(&TrigCount, 4, 1, file);
            StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&TrigCount), 4);
            if (TrigCount > 0)
            {
                TrigArray = reinterpret_cast<double *>(malloc(6 * static_cast<unsigned long long>(TrigCount) * sizeof(double)));
                fread(TrigArray, 6 * static_cast<unsigned long long>(TrigCount) * sizeof(double), 1, file);

                //Now worry about endianism - note loop is skipped on Win32 for speed
                //#ifdef _BIG_ENDIAN
                //for (ii=0; ii<TrigCount*6; ii++)
                //StaticFunctions::invertEndian((unsigned char *)(&(TrigArray[TrigCount])),sizeof(double));
                //#endif
            }


        }
        else TrigCount = 0;

        //Make isosurface
        MarchingCubes surfacer(thisobj); //create surfacer object
        surfacer.surfaceObject();

        //Next job - do isosurface stretching and convert into VTK format
        thisobj->MakePolyData();
        thisobj->ForceUpdates(-1, -1);
        mainWindow->UpdateGL();
        fixKeyCodeData();
        mainWindow->RefreshObjects();
        if (TrigCount > 0) free(TrigArray);
    }

    // Reset in the object
    if (fullarray) free(fullarray);
    thisspv->fullarray = nullptr;

    // Now ready to read panel arrays
    int ttrig = 0;
    for (int i = 0; i < SVObjects.count(); i++) ttrig += SVObjects[i]->Triangles;
    QString status;
    status.sprintf("Completed");
    mainWindow->ui->OutputLabelOverall->setText(status);
    mainWindow->ui->ProgBarOverall->setValue(100);
    qApp->processEvents();

    // Now back at end with all data stored for later writing.
    // Attempt to read arrays - expect errors - these will indicate that they weren't there!

    // Set a default ListCount to flag failure
    int ListCount = -1;

    if (fread(&dummyint, sizeof(int), 1, file) != 1) goto out;
    StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&dummyint), sizeof(int));

    if (dummyint != 10000) goto out; //10000 is code for listcount stuff follows...

    if (fread(&ListCount, sizeof(ListCount), 1, file) != 1) goto out;
    StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&ListCount), sizeof(ListCount));


    // If Listcount > count of objects in the SPV create new blank group objects
    // Convert so reads these into the objects in SPV (thisspv->Objects[i] etc.)
    firstgroup = thisspv->ComponentObjects.count();
    if (ListCount >= (thisspv->ComponentObjects.count()))
    {
        //Extra objects (must be groups)
        for (int i = thisspv->ComponentObjects.count(); i < ListCount; i++)
        {
            SVObject *group = new SVObject(SVObjects.count());
            group->IsGroup = true;
            SVObjects.append(group);
            thisspv->ComponentObjects.append(group);
        }

    }

    int ObjNumbers[201];
    bool IsGroup[201];
    int InGroup[201];
    bool ShowGroups[201];
    unsigned char ListKeys[201];

    //For now just read these as fixed size arrays. Need an alternative to read arbitrary numbers of them in
    if (fread(&(ObjNumbers[0]), sizeof(int)*static_cast<unsigned long long>(ListCount), 1, file) != 1) goto out;
    for (n = 0; n < ListCount; n++) StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&ObjNumbers[n]), sizeof(int));
    if (fread(&(InGroup[0]), sizeof(int)*static_cast<unsigned long long>(ListCount), 1, file) != 1) goto out;
    for (n = 0; n < ListCount; n++) StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&InGroup[n]), sizeof(int));
    if (fread(&(IsGroup[0]), sizeof(bool)*static_cast<unsigned long long>(ListCount), 1, file) != 1) goto out;
    for (n = 0; n < ListCount; n++) StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&IsGroup[n]), sizeof(bool));
    if (fread(&(ShowGroups[0]), sizeof(bool)*static_cast<unsigned long long>(ListCount), 1, file) != 1) goto out;
    for (n = 0; n < ListCount; n++) StaticFunctions::invertEndian(reinterpret_cast<unsigned char *>(&ShowGroups[n]), sizeof(bool));
    if (fread(&(ListKeys[0]), static_cast<size_t>(ListCount), 1, file) != 1) goto out;

    // OK, old style had funny order - groups may not be at end - I think it's actually display order
    // ObjNumbers gives the read-in number for these objects... for non-groups anyway

    BaseIndex = thisspv->ComponentObjects[0]->Index; //first item - might not be 0 if this is not first spv
    for (int i = 0; i < ListCount; i++)
    {
        // Loop round all of these old nasty things
        // is it a group? If so fix up it's ObjNumber, currently 0
        if (IsGroup[i]) ObjNumbers[i] = BaseIndex + firstgroup++;
        else ObjNumbers[i] += BaseIndex;
    }

    for (int i = 0; i < ListCount; i++)
    {
        // OK, this time round can assign all the ingroups etc properly
        int realindex = ObjNumbers[i] - BaseIndex;
        if ((InGroup[i]) == -1)
            thisspv->ComponentObjects[realindex]->InGroup = -1;
        else
            thisspv->ComponentObjects[realindex]->InGroup = ObjNumbers[InGroup[i]];

        thisspv->ComponentObjects[realindex]->Key = static_cast<QChar>(ListKeys[i]);
    }

    // Now read strings
    for (n = 0; n < ListCount; n++)
    {
        if (fread(&slen, sizeof(int), 1, file) != 1) goto out;
        //read this many characters
        if (fread(dummy, static_cast<size_t>(slen), 1, file) != 1) goto out;
        dummy[slen] = '\0'; //terminate it
        QString readinname = dummy;

        //strip the key off the front if there
        if (readinname.mid(1, 3) == " - ")
            readinname = readinname.mid(4);

        thisspv->ComponentObjects[ObjNumbers[n] - BaseIndex]->Name = readinname;
    }

    // Finally, there may be some re-ordering to do. Write positions in.
    for (int i = 0; i < ListCount; i++)
    {
        thisspv->ComponentObjects[ObjNumbers[i] - BaseIndex]->Position = i + BaseIndex;
    }

out:
    fixKeyCodeData();
    mainWindow->RefreshObjects();
    fclose(file);
}

/**
 * @brief SPVReader::version6Plus
 * This function reads and processes all v6+ SPV files. Once read this function calls
 * update on the GlWidget to show the model.
 * @param Filename
 */
void SPVReader::version6Plus(QString filename)
{
    // qDebug() << "[Where I'm I?] In version6Plus | filename = " << Filename;

    int BaseIndex;
    double dummy;
    int version;
    int objectcount;
    double PixPerMM, SlicePerMM, SkewDown, SkewLeft;
    int iDim, jDim, kDim;
    QString filenamenopath;

    //Read in new version of SPV out
    QFile InputFile(filename);
    if (InputFile.open(QIODevice::ReadOnly) == false)
    {
        // qDebug() << "[Where I'm I?] In version6Plus | File Error = Fatal - Can't open SPV file for reading";
        QMessageBox::warning(mainWindow, "File Error", "Fatal - Can't open SPV file for reading");
        QCoreApplication::quit();
    }

    BaseIndex = -1;
    for (int i = 0; i < SVObjects.count(); i++) //non 0 for imports
        if (SVObjects[i]->Index > BaseIndex) BaseIndex = SVObjects[i]->Index;

    // qDebug() << "[Where I'm I?] In version6Plus | BaseIndex = " << BaseIndex;

    BaseIndex++; //will now be first free index number

    QDataStream in(&InputFile);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setVersion(QDataStream::Qt_4_4);

    in >> dummy; //will be -1
    in >> version;

    // qDebug() << "[Where I'm I?] In version6Plus | Version = " << version;

    int spvcount = 1;
    int totalobjcount;
    if (version > 6) //single SPV version
    {
        in >> spvcount;
        if (spvcount > 1 && replaceIndex >= 0)
        {
            replaceIndex = -2;    //error - can't replace one piece with a multipiece!
            return;
        }
        in >> totalobjcount;
        bool t;
        in >> t;
        mainWindow->ui->actionSave_Memory->setChecked(t);
    }

    // qDebug() << "[Where I'm I?] In version6Plus | Reading file... | spvcount = " << spvcount;

    mainWindow->setSpecificLabel("Reading file...");
    mainWindow->setSpecificProgress(0);
    qApp->processEvents();

    // For loop - for each
    for (int spv_i = 0; spv_i < spvcount; spv_i++) //for each spv
    {
        if (version > 6)
            in >> filenamenopath; //NEW

        in >> objectcount;

        if (version == 6)
            totalobjcount = objectcount;

        in >> PixPerMM;
        in >> SlicePerMM;
        in >> SkewDown;
        in >> SkewLeft;
        in >> iDim;
        in >> jDim;
        in >> kDim;

        //Now make the SPV
        SPV *thisspv = new SPV(SPVs.count(), version, iDim, jDim, kDim);
        SPVs.append(thisspv);
        thisspv->filename = filename;
        thisspv->filenamenopath = filenamenopath;

        //work out proper scale - from FIRST Spv (maybe this one, maybe not)

        //store these in the SPV object
        thisspv->PixPerMM = PixPerMM;
        thisspv->SlicePerMM = SlicePerMM;
        thisspv->SkewDown = -SkewDown * PixPerMM;
        thisspv->SkewLeft = -SkewLeft * PixPerMM;
        mmPerUnit = ((static_cast<float>(SPVs[0]->iDim) / static_cast<float>(SCALE)) / static_cast<float>(SPVs[0]->PixPerMM));

        // Create and append all the SVObjects
        for (int i = 0; i < objectcount; i++)
        {
            SVObject *newobj = new SVObject(SVObjects.count());
            newobj->spv = thisspv; //pointer in object to SPV
            thisspv->ComponentObjects.append(newobj); //pointer in SPV to object
            SVObjects.append(newobj); //put it in my general list
            newobj->Index = SVObjects.count() - 1;
        }

        // Mirrow flag
        in >> thisspv->MirrorFlag;
        // qDebug() << "Just mirror " << thisspv->MirrorFlag;

        float Mat[16];
        if (version == 6) //matrix now
        {
            for (int i = 0; i < 16; i++) in >> Mat[i];
            for (int i = 0; i < objectcount; i++)
            {
                for (int j = 0; j < 16; j++)
                    thisspv->ComponentObjects[i]->matrix[j] = Mat[j];

                // Transpose this matrix
                StaticFunctions::transposeMatrix(thisspv->ComponentObjects[i]->matrix);

                for (int j = 0; j < 16; j++)
                    thisspv->ComponentObjects[i]->defaultmatrix[j] = thisspv->ComponentObjects[i]->matrix[j];

                thisspv->ComponentObjects[i]->gotdefaultmatrix = true;
            }
        }

        thisspv->stretches = static_cast<double *>(malloc((static_cast<unsigned long long>(kDim) + 1) * sizeof(double)));
        for (int i = 0; i < thisspv->kDim + 1; i++)
            in >> thisspv->stretches[i];

        //Now the component object details
        for (int i = 0; i < objectcount; i++)
        {
            SVObject *o = thisspv->ComponentObjects[i];
            in >> o->Name;
            in >> o->Key;
            in >> o->Colour[0];
            in >> o->Colour[1];
            in >> o->Colour[2];
            in >> o->Transparency;
            in >> o->IsGroup;
            in >> o->InGroup; //this may cause trouble...
            if (o->InGroup >= 0) o->InGroup += BaseIndex; //offset for import
            in >> o->Position;
            o->Position += BaseIndex;
            in >> o->Visible;
            in >> o->Resample;
            in >> o->IslandRemoval;
            in >> o->Smoothing;
            in >> o->ResampleType;

            //And a loveley bodge - if this is negative it means data is old (pre v4).
            //Merged in here to avoid a new file version
            if (o->ResampleType < 0)
            {
                o->ResampleType = 10000 + o->ResampleType;
                o->buggedData = true;
            }

            if (version >= 7) for (int i = 0; i < 16; i++) in >> o->matrix[i];

            StaticFunctions::transposeMatrix(o->matrix);

            for (int j = 0; j < 16; j++)
                thisspv->ComponentObjects[i]->defaultmatrix[j] = thisspv->ComponentObjects[i]->matrix[j];

            thisspv->ComponentObjects[i]->gotdefaultmatrix = true;

            if (version >= 7)
            {
                in >> o->Index;
                o->Index += BaseIndex;
            }

            if (version >= 7)
            {
                in >> o->scale;
            }
        }

        if (replaceIndex >= 0) //we are replacing something
        {
            for (int i = 0; i < objectcount; i++)
            {
                SVObject *o = thisspv->ComponentObjects[i];
                //Find matching part in old SPV
                int match = -1;
                for (int j = 0; j < SPVs[replaceIndex]->ComponentObjects.count(); j++)
                {
                    if (SPVs[replaceIndex]->ComponentObjects[j]->Name == o->Name) match = j;
                }
                if (match >= 0)
                {
                    //put it into the correct group
                    o->InGroup = SPVs[replaceIndex]->ComponentObjects[match]->InGroup;
                    o->Transparency = SPVs[replaceIndex]->ComponentObjects[match]->Transparency;
                    o->Position = SPVs[replaceIndex]->ComponentObjects[match]->Position;
                    o->Visible = SPVs[replaceIndex]->ComponentObjects[match]->Visible;
                    o->Resample = SPVs[replaceIndex]->ComponentObjects[match]->Resample;
                    o->Smoothing = SPVs[replaceIndex]->ComponentObjects[match]->Smoothing;
                    o->IslandRemoval = SPVs[replaceIndex]->ComponentObjects[match]->IslandRemoval;

                    for (int j = 0; j < 16; j++) o->matrix[j] = SPVs[replaceIndex]->ComponentObjects[match]->matrix[j];
                }
                else //take matrix from first object and cross fingers
                {
                    for (int j = 0; j < 16; j++) o->matrix[j] = SPVs[replaceIndex]->ComponentObjects[0]->matrix[j];
                }
            }
        }

        //Now the data
        QTime t;
        t.start();
        for (int i = 0; i < objectcount; i++)
        {
            SVObject *o = thisspv->ComponentObjects[i];

            mainWindow->setSpecificLabel("Preprocessing Data");
            qApp->processEvents();
            if (!(o->IsGroup))
            {

                int pieces;

                in >> pieces; //this is the number of pieces thing - always kdim -2 on a writeback
                //but for a merged object it could be more on first read from edit-generated SPV

                if (pieces == -3) //Code for 'single compressed thing
                {
                    in >> o->AllSlicesSize;
                    o->AllSlicesCompressed = static_cast<unsigned char *>(malloc(static_cast<size_t>(o->AllSlicesSize)));
                    InputFile.read(reinterpret_cast<char *>(o->AllSlicesCompressed), o->AllSlicesSize);
                }
                else
                {
                    CompressedSlice *BlankSlice = new CompressedSlice(o, true);
                    (o->compressedslices).append(BlankSlice);
                    int ThisSlice = 1;
                    //qDebug() << "pieces" << pieces;
                    for (int j = 0; j < pieces; j++)
                    {

                        mainWindow->setSpecificProgress((j * 100) / pieces);
                        if (j % 10 == 0) qApp->processEvents();

                        if (j % (thisspv->kDim - 2) == 0) ThisSlice = 1; // reached a restart from merge

                        int EmptyFlag;
                        in >> EmptyFlag;
                        if (EmptyFlag == -1) //empty
                        {
                            if (j < (thisspv->kDim - 2)) //no merge complications
                            {
                                CompressedSlice *blankslice = new CompressedSlice(o, true);
                                o->compressedslices.append(blankslice);
                            }
                            //do nothing if it's a merge job
                            ThisSlice++;

                        }
                        else //not empty
                        {
                            //read the grid
                            t.start();
                            CompressedSlice *newslice = new CompressedSlice(o, false);
                            newslice->grid = static_cast<unsigned char *>(malloc(static_cast<size_t>(thisspv->GridSize)));

                            int temp;
                            temp = in.readRawData(reinterpret_cast<char *>(newslice->grid), thisspv->GridSize);
                            Q_UNUSED(temp)

                            in >> newslice->datasize;

                            newslice->data = static_cast<unsigned char *>(malloc(static_cast<size_t>(newslice->datasize)));

                            temp = in.readRawData(reinterpret_cast<char *>(newslice->data), newslice->datasize);
                            //qDebug() << "1." << t.elapsed();
                            if (j < (thisspv->kDim - 2)) //no merge complications
                            {
                                (o->compressedslices).append(newslice);
                                //qDebug() << "h1";
                            }
                            else
                            {
                                o->compressedslices[ThisSlice]->merge(newslice, QString(TESTDUMPLOCATION) + "test.bmp");
                                delete newslice;
                                //qDebug() << "h2";
                            }
                            ThisSlice++;
                            //qDebug() << "2." << t.elapsed();
                        }
                    }

                    BlankSlice = new CompressedSlice(o, true);
                    (o->compressedslices).append(BlankSlice);
                }
                bool withpd;
                in >> withpd;
                if (withpd)
                {
                    containsPresurfaced = true;
                    o->ReadPD(&InputFile);
                    o->Dirty = true;
                    o->SurfaceMe = false;

                }
                else
                {
                    containsNonPresurfaced = true;
                    o->SurfaceMe = true;

                }
            }
            //qDebug() << "3." << t.elapsed();
        }
    }

    //qDebug() << "[Where I'm I?] In version6Plus | Now the orphan objects - which can only be groups";

    // Now the orphan objects - which can only be groups
    if (version >= 8)
    {
        int newobs;
        in >> newobs;
        for (int i = 0; i < newobs; i++)
        {
            SVObject *o = new SVObject(0);
            in >> o->Name;
            in >> o->Key;
            in >> o->IsGroup;
            in >> o->InGroup;
            in >> o->Position;
            in >> o->Visible;
            in >> o->Index;
            SVObjects.append(o);
        }
    }

    if (!(in.atEnd()))
    {
        //qDebug() << "[Where I'm I?] In version6Plus | checking for InfoLists";

        QString check;
        in >> check;
        if (check == "InfoLists")
        {
            in >> infoComments;
            in >> infoReference;
            in >> infoAuthor;
            in >> infoSpecimen;
            in >> infoProvenance;
            in >> infoClassificationName;
            in >> infoClassificationRank;
            in >> infoTitle;
        }
    }

    //Now scale stuff and a few interface things
    if (!(in.atEnd()))
    {
        // qDebug() << "[Where I'm I?] In version6Plus | checking for scaleball data";

        in >> scaleBallColour[0];
        in >> scaleBallColour[1];
        in >> scaleBallColour[2];
        in >> scaleBallScale; //resize applied to ball - for scale calc
        for (int i = 0; i < 16; i++) in >> scaleMatrix[i];

        bool tempbool;
        in >> tempbool;
        mainWindow->ui->actionQuadric_Fidelity_Reduction->setChecked(tempbool);
        in >> tempbool;
        mainWindow->ui->actionShow_Ball_2->setChecked(tempbool);
    }


    if (!(in.atEnd()))
    {
        in >> colorBackgroundRed;
        in >> colorBackgroundGreen;
        in >> colorBackgroundBlue;
    }

    if (!(in.atEnd()))
    {
        in >> colorGridRed;
        in >> colorGridGreen;
        in >> colorGridBlue;
        in >> colorGridMinorRed;
        in >> colorGridMinorGreen;
        in >> colorGridMinorBlue;
    }

    if (!(in.atEnd()))
        for (int i = 0; i < SPVs.count(); i++) //NEW - do each SPV
        {
            SPV *s = SPVs[i];
            for (int i = 0; i < s->ComponentObjects.count(); i++)
            {
                SVObject *o = s->ComponentObjects[i];
                in >> o->Shininess;
                in >> o->Transparency; //these might now be <0
                in >> o->IslandRemoval; //these might now be <0
                in >> o->Smoothing; //these might now be <0
            }
        }

    //qDebug() << "[Where I'm I?] In version6Plus calling fixKeyCodeData();";
    fixKeyCodeData();

    //qDebug() << "[Where I'm I?] In version6Plus calling RefreshObjects();";
    mainWindow->RefreshObjects();

    //qDebug() << "[Where I'm I?] In version6Plus calling RefreshInfo()";
    mainWindow->RefreshInfo();

    int items = 0;
    for (int z = BaseIndex; z < SVObjects.count(); z++)
        if (!(SVObjects[z]->IsGroup))
            items++;

    int icount = 1;

    //Now do all the processing
    //qDebug() << "[Where I'm I?] In version6Plus starting processing...";
    for (int i = BaseIndex; i < SVObjects.count(); i++)
    {
        if (!(SVObjects[i]->IsGroup))
        {
            QString status;
            QTextStream ts(&status);

            //qDebug() << "Processing...'" << SVObjects[i]->Name << "', " << icount << " of " << items;
            ts << "Processing '" << SVObjects[i]->Name << "', " << icount << " of " << items;

            mainWindow->ui->OutputLabelOverall->setText(status);
            mainWindow->ui->ProgBarOverall->setValue((icount++ * 100) / items);
            if (SVObjects[i]->SurfaceMe)
            {
                MarchingCubes surfacer(SVObjects[i]); //create surfacer object

                unsigned char *fullarray = nullptr;
                if (SVObjects[i]->AllSlicesCompressed)
                {

                    int size = SVObjects[i]->spv->size;
                    if ((fullarray = static_cast<unsigned char *>(malloc(static_cast<size_t>(size * SVObjects[i]->spv->kDim)))) == nullptr)
                    {
                        QMessageBox::warning(static_cast<QWidget *>(mainWindow), "Memory Error", "Fatal Error - could not obtain enough memory to reconstruct volume.\nTry exporting from a newer version of SPIERSview");
                        QCoreApplication::quit();
                    }

                    //ensure top and bottom are blank
                    for (int z = 0; z < size; z++)
                        fullarray[z] = 0;

                    unsigned char *endfullarray = fullarray + (SVObjects[i]->spv->size) * (SVObjects[i]->spv->kDim - 1);

                    for (int z = 0; z < size; z++)
                        endfullarray[z] = 0;

                    SVObjects[i]->spv->fullarray = fullarray;
                    uLongf s = static_cast<uLongf>(size * SVObjects[i]->spv->kDim);
                    uncompress(fullarray, &s, SVObjects[i]->AllSlicesCompressed, static_cast<uLong>(SVObjects[i]->AllSlicesSize));

                    int count = 0;
                    for (int iii = 0; iii < size * SVObjects[i]->spv->kDim; iii++)
                        if (fullarray[iii]) count++;

                }
                surfacer.surfaceObject();
                if (fullarray)
                {
                    free(fullarray);
                    SVObjects[i]->spv->fullarray = nullptr;
                }

                //qDebug() << "[Where I'm I?] In version6Plus - calling MakePolyData()";
                SVObjects[i]->MakePolyData();
            }

            //Next job - do isosurface stretching and convert into VTK format
            SVObjects[i]->ForceUpdates(-1, -1);
            mainWindow->UpdateGL();
        }
    }

    int ttrig = 0;
    for (int i = 0; i < SVObjects.count(); i++)
        ttrig += SVObjects[i]->Triangles;

    //qDebug() << "[Where I'm I?] In version6Plus - Completed " << (ttrig / 1000);
    QString status = QString("Completed");
    mainWindow->ui->OutputLabelOverall->setText(status);
    mainWindow->ui->ProgBarOverall->setValue(100);

    //qDebug() << "[Where I'm I?] In version6Plus calling UpdateGL() at function end.";
    mainWindow->UpdateGL();
}
