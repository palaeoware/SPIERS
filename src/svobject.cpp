#include "svobject.h"
#include "globals.h"
#include "spv.h"
#include "vtkIdList.h"
#include "compressedslice.h"
#include "vtkProperty2D.h"
#include "mainwindow.h"
#include "vtkPolygon.h"
#include "vtkDataArray.h"
#include "svglwidget.h"

#include <QDebug>
#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QList>
#include <QVector3D>
#include <QMatrix4x4>

//#ifdef __APPLE__
//#include <gl.h>
//#include <glu.h>
//#endif

//#ifndef __APPLE__
//#include <GL/gl.h>
//#include <GL/glu.h>
//#endif

#include <QtOpenGL/QtOpenGL>
#include "ui_mainwindow.h"

QList <SVObject *> SVObjects;

/**
 * @brief SVObject::SVObject
 * @param index
 */
SVObject::SVObject(int index)
{
    colour = false;
    Index = index;
    Position = index;
    localPolyData = vtkPolyData::New();
    verts = vtkPoints::New();
    actualarray = vtkIdTypeArray::New();
    cellarray = vtkCellArray::New();
    InGroup = -1; //not in a group by default
    IsGroup = false;
    Transparency = 0;
    Visible = true;
    widgetitem = nullptr;
    Expanded = false;
    Dirty = true;
    displaylists.clear();
    IslandRemoval = 0;
    Smoothing = 0;
    Triangles = 0;
    ResampleType = 0; //default normally 0
    ResetMatrix();
    gotdefaultmatrix = false;
    polyDataCompressed = false;
    killme = false;
    AllSlicesCompressed = nullptr;
    scale = 1.0;
    Resample = 100;
    Name = "";
    buggedData = false;
    Key = 0;
    object_ktr = 0;
    usesVBOs = false;
    Shininess = 2;
    donebox = false;
    voxels = 0;
    spv = nullptr;
}

/**
 * @brief SVObject::~SVObject
 */
SVObject::~SVObject()
{
    MainWin->gl3widget->makeCurrent();
    localPolyData->Delete();
    verts ->Delete();
    actualarray->Delete();
    cellarray->Delete();
    qDeleteAll(compressedslices.begin(), compressedslices.end());
    qDeleteAll(Isosurfaces.begin(), Isosurfaces.end());
    if (AllSlicesCompressed != nullptr)
        free(AllSlicesCompressed);

    //qDeleteAll(VBOs);
    qDeleteAll(VertexBuffers);
    //qDeleteAll(NormalBuffers);
    qDeleteAll(ColourBuffers);
}

/**
 * @brief SVObject::Parent
 * @return
 */
int SVObject::Parent()
{
    //return pointer to parent
    for (int i = 0; i < SVObjects.count(); i++)
        if (SVObjects[i]->Index == InGroup) return i;

    return -1; //no group found
}

/**
 * @brief SVObject::ResetMatrix
 */
void SVObject::ResetMatrix()
{
    //intial identity matrix
    matrix[0] = 1;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 0;
    matrix[4] = 0;
    matrix[5] = 1;
    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = 0;
    matrix[9] = 0;
    matrix[10] = 1;
    matrix[11] = 0;
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 1;
}

/**
 * @brief SVObject::DoUpdates
 */
void SVObject::DoUpdates()
{
    if (MainWin->ui->actionAuto_Resurface->isChecked())
        MakeDlists();
    else
        Dirty = true;
}

/**
 * @brief SVObject::ForceUpdates
 * @param thisobj
 * @param totalobj
 */
void SVObject::ForceUpdates(int thisobj, int totalobj)
{
    qDebug() << "[Where I'm I?] In ForceUpdates";

    if (thisobj >= 0)
    {
        qDebug() << "[Where I'm I?] In ForceUpdates - thisobj >= 0";

        QString status;
        status.sprintf("Reprocessing %d of %d", thisobj + 1, totalobj);
        MainWin->ui->OutputLabelOverall->setText(status);
        if (totalobj == 0)
            MainWin->ui->ProgBarOverall->setValue(100);
        else
            MainWin->ui->ProgBarOverall->setValue((thisobj * 100) / totalobj);

    }

    qDebug() << "[Where I'm I?] In ForceUpdates calling MakeDlists();";
    MakeDlists();

    qDebug() << "[Where I'm I?] In ForceUpdates calling CompressPolyData(false);";
    CompressPolyData(false); //return it to compressed format
}

/**
 * @brief ProgressHandler
 * @param progress
 */
static void ProgressHandler(vtkObject *, unsigned long, void *, void *progress)
{
    double *amount = static_cast<double *>(progress);
    int iamount = static_cast<int>((*amount) * 100.0);
    qDebug() << "[ProgressHandler] " << iamount << "%";
    MainWin->setSpecificProgress(iamount);
    qApp->processEvents();
}

/**
 * @brief ErrorHandler
 * @param progress
 */
static void ErrorHandler(vtkObject *, unsigned long, void *, void *progress)
{
    Q_UNUSED(progress)

    QMessageBox::critical(MainWin, "Error", "Fatal VTK error: This is likely an 'out of memory' issue - if however you see this message when working with small objects please ttact the software author");
}

/**
 * @brief SVObject::DeleteVTKObjects
 */
void SVObject::DeleteVTKObjects()
{
    qDebug() << "[Where I'm I?] In DeleteVTKObjects";

    int cells = static_cast<int>(polydata->GetNumberOfCells());

    if (IslandRemoval && polydata->GetNumberOfCells() != 0) islandfinder->Delete();
    if (Smoothing && cells != 0) smoother->Delete();
    //normals->Delete();
    normalx.clear();
    normaly.clear();
    normalz.clear();
    if (Resample != 100 && cells != 0)
    {
        if (ResampleType == 0) decimator->Delete();
        if (ResampleType == 1) qdecimator->Delete();
    }
}

/**
 * @brief SVObject::GetFinalPolyData
 */
void SVObject::GetFinalPolyData()
{
    qDebug() << "[Where I'm I?] In GetFinalPolyData";

    if (IsGroup) return;


    qDebug() << "[Where I'm I?] In GetFinalPolyData - this is not a group...";

    if (vaxml_mode == false)
    {
        if (polyDataCompressed) UnCompressPolyData();

        //First put it through the decimator if applicable
        vtkPolyDataAlgorithm *dout = nullptr;

        if (MainWin->ui->actionQuadric_Fidelity_Reduction->isChecked())
            ResampleType = 1;
        else
            ResampleType = 0;

        if (localPolyData->GetNumberOfCells() == 0)
        {
            polydata = localPolyData;
            normalx.clear();
            normaly.clear();
            normalz.clear();
            return;
        }

        if (Resample != 100)
        {
            if (ResampleType == 0)
            {
                MainWin->setSpecificLabel("Simplifying Object");
                qDebug() << "[Simplifying Object] Start - Type 0";
                qApp->processEvents();

                decimator = vtkDecimatePro::New();

                decimator->AccumulateErrorOff();
                decimator->SetTargetReduction(1.0 - static_cast<double>(Resample) / 100.0);
                decimator->PreserveTopologyOn();
                decimator->SetInputData(localPolyData);

                cb = vtkCallbackCommand::New();
                cb->SetCallback(ProgressHandler);
                cberror = vtkCallbackCommand::New();
                cberror->SetCallback(ErrorHandler);
                decimator->AddObserver(vtkCommand::ProgressEvent, cb, 1.0);
                decimator->AddObserver(vtkCommand::ErrorEvent, cberror, 1.0);
                decimator->Update();
                cb->Delete();
                cberror->Delete();
                dout = static_cast<vtkPolyDataAlgorithm *>(decimator);
                qDebug() << "[Simplifying Object] End - Type 0";
            }

            if (ResampleType == 1) //quadric
            {
                MainWin->setSpecificLabel("Simplifying Object with Quadric algoritim");
                qDebug() << "[Simplifying Object] Start - Type 1 (Quadric)";
                qApp->processEvents();

                qdecimator = vtkQuadricDecimation::New();

                //            qDebug()<<(1.0-(double)Resample/100.0);
                qdecimator->SetTargetReduction(1.0 - (double)Resample / 100.0);
                //            qdecimator->ScalarsAttributeOff();
                //            qdecimator->VectorsAttributeOff();
                //            qdecimator->NormalsAttributeOff();
                //            qdecimator->TCoordsAttributeOff();
                //            qdecimator->TensorsAttributeOff();

                qdecimator->SetInputData(localPolyData);

                cb = vtkCallbackCommand::New();
                cb->SetCallback(ProgressHandler);
                cberror = vtkCallbackCommand::New();
                cberror->SetCallback(ErrorHandler);
                qdecimator->AddObserver(vtkCommand::ProgressEvent, cb, 1.0);
                qdecimator->AddObserver(vtkCommand::ErrorEvent, cberror, 1.0);
                qdecimator->Update();
                //qDebug()<<qdecimator->GetActualReduction();
                cb->Delete();
                cberror->Delete();
                dout = static_cast<vtkPolyDataAlgorithm *>(qdecimator);
                qDebug() << "[Simplifying Object] End - Type 1";
            }
        }

        if (IslandRemoval != 0)
        {
            MainWin->setSpecificLabel("Finding Islands");
            qDebug() << "[Finding Islands] Start";
            qApp->processEvents();

            islandfinder = static_cast<DataConnectivityFilter *>(DataConnectivityFilter::New());

            if (Resample == 100)
                islandfinder->SetInputData(localPolyData);
            else
                islandfinder->SetInputConnection(dout->GetOutputPort());

            islandfinder->ScalarConnectivityOff();
            if (IslandRemoval == 5)
                islandfinder->SetExtractionModeToLargestRegion();
            else
                islandfinder->SetExtractionModeToAllRegions();

            islandfinder->ColorRegionsOff();

            cb = vtkCallbackCommand::New();
            cb->SetCallback(ProgressHandler);
            cberror = vtkCallbackCommand::New();
            cberror->SetCallback(ErrorHandler);
            islandfinder->AddObserver(vtkCommand::ProgressEvent, cb, 1.0);
            islandfinder->AddObserver(vtkCommand::ErrorEvent, cberror, 1.0);
            islandfinder->Update();

            // One big region - so just take output
            if (IslandRemoval == 5)
            {
                qDebug() << "[Finding Islands] Filter Limit = high Island Removal = " << IslandRemoval;
                pdislands = islandfinder->GetOutput();
            }
            else
            {
                int filterlimit = 0;
                if (IslandRemoval == 1) filterlimit = 20;
                if (IslandRemoval == 2) filterlimit = 100;
                if (IslandRemoval == 3) filterlimit = 600;
                if (IslandRemoval == 4) filterlimit = 4000;
                if (IslandRemoval < 0) filterlimit = 0 - IslandRemoval;
                qDebug() << "[Finding Islands] Filter Limit = " << filterlimit << " Island Removal = " << IslandRemoval;
                MainWin->setSpecificLabel("Filtering Islands");
                qApp->processEvents();

                int islandcount = islandfinder->GetNumberOfExtractedRegions();
                islandfinder->InitializeSpecifiedRegionList();
                islandfinder->SetExtractionModeToSpecifiedRegions();
                for (int i = 0; i < islandcount; i++)
                {
                    if (islandfinder->GetRegionCount(i) > filterlimit)
                        islandfinder->AddSpecifiedRegion(i);
                }

                islandfinder->Update();
                cb->Delete();
                cberror->Delete();
                pdislands = islandfinder->GetOutput();
            }
            qDebug() << "[Finding Islands] End";
        }

        // Do any required smoothing
        if (Smoothing != 0)
        {
            MainWin->setSpecificLabel("Performing Smoothing");
            qDebug() << "[Smoothing] Start";
            qApp->processEvents();
            smoother = vtkWindowedSincPolyDataFilter::New();
            if (Resample == 100 && IslandRemoval == 0)
                smoother->SetInputData(localPolyData);
            else
            {
                if (IslandRemoval != 0)
                    smoother->SetInputData(pdislands);
                else
                    smoother->SetInputConnection(dout->GetOutputPort());
            }

            smoother->FeatureEdgeSmoothingOff();
            smoother->BoundarySmoothingOff();
            smoother->GenerateErrorScalarsOff();
            smoother->GenerateErrorVectorsOff();
            if (Smoothing == 1) smoother->SetNumberOfIterations(5);
            if (Smoothing == 2) smoother->SetNumberOfIterations(10);
            if (Smoothing == 3) smoother->SetNumberOfIterations(20);
            if (Smoothing == 4) smoother->SetNumberOfIterations(40);
            if (Smoothing == 5) smoother->SetNumberOfIterations(60);
            if (Smoothing == 6) smoother->SetNumberOfIterations(100);
            if (Smoothing < 0) smoother->SetNumberOfIterations(0 - Smoothing);
            smoother->SetPassBand(.05);

            qDebug() << "[Smoothing] About to smooth " << Name << Key << " count is" << localPolyData->GetNumberOfCells();
            cb = vtkCallbackCommand::New();
            cb->SetCallback(ProgressHandler);
            cberror = vtkCallbackCommand::New();
            cberror->SetCallback(ErrorHandler);
            smoother->AddObserver(vtkCommand::ProgressEvent, cb, 1.0);
            smoother->AddObserver(vtkCommand::ErrorEvent, cberror, 1.0);
            smoother->Update();
            cb->Delete();
            cberror->Delete();
            qDebug() << "[Smoothing] Smoothed" << Name << Key;
            qDebug() << "[Smoothing] End";
        }


        qDebug() << "[Getting data] Getting data...";
        if ((Resample == 100 && IslandRemoval == 0 && Smoothing == 0))
        {
            qDebug() << "[Getting data] Getting data... using localPolyData";
            polydata = localPolyData;
        }
        else
        {
            if (Smoothing != 0)
            {
                qDebug() << "[Getting data] Getting data... using smoother->GetOutput()";
                polydata = smoother->GetOutput();
            }
            else
            {
                if (IslandRemoval != 0)
                {
                    qDebug() << "[Getting data] Getting data... using pdislands";
                    polydata = pdislands;
                }
                else
                {
                    qDebug() << "[Getting data] Getting data... using dout->GetOutput()";
                    polydata = dout->GetOutput();
                }
            }
        }
    }

    qDebug() << "[Calculating Normals] Calculating Normals";
    MainWin->setSpecificLabel("Calculating Normals");
    qApp->processEvents();

    // Now new simplified normal generating code
    int tcount = static_cast<int>(polydata->GetNumberOfCells());
    //int pcount = static_cast<int>(polydata->GetNumberOfPoints());

    normalx.clear();
    normaly.clear();
    normalz.clear();
    normalx.resize(tcount); //should auto zero them
    normaly.resize(tcount);
    normalz.resize(tcount);

    for (int i = 0; i < tcount; i++)
    {
        vtkTriangle *cell = static_cast<vtkTriangle *>(polydata->GetCell(i));
        vtkIdType tri[3]; //vertex indices
        for (vtkIdType j = 0; j < 3; j++) tri[j] = cell->GetPointId(static_cast<int>(j));

        //now find the actual vertex value
        double x[9]; //3 points - hopefully as nine consecutive values
        polydata->GetPoint(tri[0], &(x[0]));
        polydata->GetPoint(tri[1], &(x[3]));
        polydata->GetPoint(tri[2], &(x[6]));

        double n[3];
        vtkPolygon::ComputeNormal(3, x, n);

        //add to the normals
        normalx[static_cast<int>(tri[0])] += static_cast<float>(n[0]);
        normaly[static_cast<int>(tri[0])] += static_cast<float>(n[1]);
        normalz[static_cast<int>(tri[0])] += static_cast<float>(n[2]);
        normalx[static_cast<int>(tri[1])] += static_cast<float>(n[0]);
        normaly[static_cast<int>(tri[1])] += static_cast<float>(n[1]);
        normalz[static_cast<int>(tri[1])] += static_cast<float>(n[2]);
        normalx[static_cast<int>(tri[2])] += static_cast<float>(n[0]);
        normaly[static_cast<int>(tri[2])] += static_cast<float>(n[1]);
        normalz[static_cast<int>(tri[2])] += static_cast<float>(n[2]);

        if (i % 10000 == 0)
        {
            MainWin->setSpecificProgress((i * 100) / tcount);
            qApp->processEvents();
        }
    }

    qDebug() << "[Where I'm I?] End of GetFinalPolyData";

    return;
}

/**
 * @brief SVObject::MakeVBOs
 */
void SVObject::MakeVBOs()
{
    qDebug() << "[Where I'm I?] In MakeVBOs";

    //Copy of old MakeDlists, updated to work with VBOs for OpenGL 2+

    if (IsGroup) return;

    QVector<QVector3D> vertices;         // Vertices
    QVector<QVector3D> normals;         // Normals
    QVector<QVector3D> colours;         // Normals

    colours.resize(3 * MAXDLISTSIZE);
    vertices.resize(3 * MAXDLISTSIZE); //3 vertices, 3 values for each
    normals.resize(3 * MAXDLISTSIZE);

    usesVBOs = true;
    int dcount = 1; //old dlist count, will use for VBOs as well

    qDeleteAll(VertexBuffers);
    VertexBuffers.clear();
    qDeleteAll(ColourBuffers);
    ColourBuffers.clear();
    VBOVertexCounts.clear();

    GetFinalPolyData(); //do all the VTK stuff

    double *d = polydata->GetBounds();

    //store
    if (donebox == false) //anti-aliasing throws this - just keep values from first time round
    {
        objectxmin = d[0];
        objectxmax = d[1];
        objectymin = d[2];
        objectymax = d[3];
        objectzmin = d[4];
        objectzmax = d[5];
        donebox = true;
    }

    //create bounding box
    qDebug() << "[Where I'm I?] In MakeVBOs - creating bounding box";

    if (boundingBoxBuffer.isCreated()) boundingBoxBuffer.destroy();
    boundingBoxBuffer.create();
    boundingBoxBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw );
    boundingBoxBuffer.bind();

    QVector<QVector3D> lineVertices;

    lineVertices << QVector3D(objectxmin, objectymin,  objectzmin)
                 << QVector3D( objectxmax, objectymin,  objectzmin);
    lineVertices << QVector3D(objectxmin, objectymin,  objectzmin)
                 << QVector3D( objectxmin, objectymax,  objectzmin);
    lineVertices << QVector3D(objectxmin, objectymin,  objectzmin)
                 << QVector3D( objectxmin, objectymin,  objectzmax);

    lineVertices << QVector3D(objectxmax, objectymax,  objectzmax)
                 << QVector3D( objectxmin, objectymax,  objectzmax);
    lineVertices << QVector3D(objectxmax, objectymax,  objectzmax)
                 << QVector3D( objectxmax, objectymin,  objectzmax);
    lineVertices << QVector3D(objectxmax, objectymax,  objectzmax)
                 << QVector3D( objectxmax, objectymax,  objectzmin);

    //lineVertices << QVector3D(objectxmax, objectymin,  objectzmin) << QVector3D( objectxmin, objectymin,  objectzmin);
    lineVertices << QVector3D(objectxmax, objectymin,  objectzmin)
                 << QVector3D( objectxmax, objectymax,  objectzmin);
    lineVertices << QVector3D(objectxmax, objectymin,  objectzmin)
                 << QVector3D( objectxmax, objectymin,  objectzmax);

    lineVertices << QVector3D(objectxmin, objectymax,  objectzmin)
                 << QVector3D( objectxmax, objectymax,  objectzmin);
    //lineVertices << QVector3D(objectxmin, objectymax,  objectzmin) << QVector3D( objectxmin, objectymin,  objectzmin);
    lineVertices << QVector3D(objectxmin, objectymax,  objectzmin)
                 << QVector3D( objectxmin, objectymax,  objectzmax);

    lineVertices << QVector3D(objectxmin, objectymin,  objectzmax)
                 << QVector3D( objectxmax, objectymin,  objectzmax);
    lineVertices << QVector3D(objectxmin, objectymin,  objectzmax)
                 << QVector3D( objectxmin, objectymax,  objectzmax);
    //lineVertices << QVector3D(objectxmin, objectymin,  objectzmax) << QVector3D( objectxmin, objectymin,  objectzmin);

    QVector<QVector3D> lineNormals;
    for (int i = 0; i < 24; i++) lineNormals << QVector3D(0, 0, 1);

    boundingBoxBuffer.allocate(24 * 6 * sizeof(GLfloat));
    boundingBoxBuffer.write(0, lineVertices.constData(), 12 * 6 * sizeof(GLfloat));
    boundingBoxBuffer.write(12 * 6 * sizeof(GLfloat), lineVertices.constData(), 12 * 6 * sizeof(GLfloat));
    boundingBoxBuffer.release();

    //Do some clever stuff - apply matrix to bounding box basically
    QVector3D v1(d[0], d[2], d[4]), v2(d[1], d[3], d[5]);
    QMatrix4x4 mat(
        (qreal)matrix[0],
        (qreal)matrix[1],
        (qreal)matrix[2],
        (qreal)matrix[3],
        (qreal)matrix[4],
        (qreal)matrix[5],
        (qreal)matrix[6],
        (qreal)matrix[7],
        (qreal)matrix[8],
        (qreal)matrix[9],
        (qreal)matrix[10],
        (qreal)matrix[11],
        (qreal)matrix[12],
        (qreal)matrix[13],
        (qreal)matrix[14],
        (qreal)matrix[15]
    );

    QVector3D v1t = mat.mapVector(v1);
    QVector3D v2t = mat.mapVector(v2);

    d[0] = (float)v1t.x();
    d[1] = (float)v2t.x();
    d[2] = (float)v1t.y();
    d[3] = (float)v2t.y();
    d[4] = (float)v1t.z();
    d[5] = (float)v2t.z();

    //and now set min and max values properly

    //float minx, maxx, miny, maxy, minz, maxz;
    if (d[0] < minx || firstobject) minx = d[0];
    if (d[1] > maxx || firstobject) maxx = d[1];
    if (d[2] < miny || firstobject) miny = d[2];
    if (d[3] > maxy || firstobject) maxy = d[3];
    if (d[4] < minz || firstobject) minz = d[4];
    if (d[5] > maxz || firstobject) maxz = d[5];


    //count triangles for the record
    Triangles = polydata->GetNumberOfCells();

    MainWin->setSpecificLabel("Creating VBO objects");
    qApp->processEvents();

    //Create a VBO object and append to the list
    int tcount = polydata->GetNumberOfCells();
    int count = 0;

    vtkDataArray *scals = polydata->GetPointData()->GetScalars();

    //if this returns anything assume valid colour
    if (scals) colour = true;

    //i is index of vtk triangles, size is number to do each time
    int i = 0, sizethisvao;

    while (i < tcount)
    {
        //find number to do - capped as MAXDLISTSIZE
        if (tcount - i > MAXDLISTSIZE)
            sizethisvao = MAXDLISTSIZE;
        else
            sizethisvao = tcount - i;

        int index = 0; //index into vectors that will be passed to VAO

        for (int k = 0; k < sizethisvao; k++) // do determined number of triangles
        {
            double ctuple[3];
            double tuple[3];
            vtkTriangle *cell = (vtkTriangle *)polydata->GetCell(i);
            vtkIdType tri[3]; //vertex indices
            for (vtkIdType j = 0; j < 3; j++) //for each vertex
            {
                tri[j] = cell->GetPointId(j);

                if (colour) //if got per cell colour from a PLY
                {
                    scals->GetTuple(tri[j], ctuple);
                    colours[index] = QVector3D((GLfloat)(ctuple[0] / 255.0), (GLfloat)(ctuple[1] / 255.0), (GLfloat)(ctuple[2] / 255.0));
                }

                normals[index] = QVector3D(normalx[tri[j]], (GLfloat)normaly[tri[j]], (GLfloat)normalz[tri[j]]);

                polydata->GetPoint(tri[j], tuple); //get and stash vertices, casting to floats (eek!)
                vertices[index] = QVector3D((GLfloat)(tuple[0]), (GLfloat)(tuple[1]), (GLfloat)(tuple[2]));

                index += 1; //next index for vertices
            }
            i++; //next triangle
        }

        VBOVertexCounts.append(sizethisvao * 3);

        //create buffer objects
        QOpenGLBuffer *vbuffer = new QOpenGLBuffer;
        vbuffer->create();
        vbuffer->setUsagePattern(QOpenGLBuffer::StaticDraw );
        vbuffer->bind();
        vbuffer->allocate(18 * sizethisvao * sizeof(GLfloat));
        vbuffer->write(0, vertices.constData(), sizethisvao * 9 * sizeof(GLfloat));
        vbuffer->write(sizethisvao * 9 * sizeof(GLfloat), normals.constData(), sizethisvao * 9 * sizeof(GLfloat));
        VertexBuffers.append(vbuffer);

        if (colour)
        {
            QOpenGLBuffer *cbuffer = new QOpenGLBuffer;
            cbuffer->create();
            cbuffer->setUsagePattern(QOpenGLBuffer::StaticDraw );
            cbuffer->bind();
            cbuffer->allocate(9 * sizethisvao * sizeof(GLfloat));
            cbuffer->write(0, colours.constData(), sizethisvao * 9 * sizeof(GLfloat));
            ColourBuffers.append(cbuffer);
        }

        //progress bar
        MainWin->setSpecificProgress((i * 100) / tcount);
        if (i % 1000 == 0) qApp->processEvents();
    }

    MainWin->setSpecificLabel("Completed");
    MainWin->setSpecificProgress(100); //looks better!
    qApp->processEvents();

    //and delete any vtk objects used
    if (vaxml_mode == false) DeleteVTKObjects();

    Dirty = false;

    model_ktr -= object_ktr; //remove any old one
    object_ktr = tcount;
    model_ktr += tcount; //add in new count
}

/**
 * @brief SVObject::MakeDlists
 */
void SVObject::MakeDlists()
{
    return MakeVBOs();
}

/**
 * @brief SVObject::CompressPolyData
 * @param flag
 */
void SVObject::CompressPolyData(bool flag)
{
    //MainWin->ui->actionSave_Memory->setChecked(true);
    if (IsGroup) return;
    if (vaxml_mode) return;
    if (polyDataCompressed == true) return;
    if (flag == false && !(MainWin->ui->actionSave_Memory->isChecked())) //if not a saving to ps call and if the save memory flag is unticked
        //we don't compress at all
        return;
    polyDataCompressed = true;

    if (compressedPolyData.size() > 0) //there is already compressed data
    {
        if (MainWin->ui->actionSave_Memory->isChecked())
        {
            localPolyData->Delete();
            localPolyData = vtkPolyData::New();
            verts->Delete();
            verts = vtkPoints::New();
            cellarray->Delete();
            actualarray->Delete();
            actualarray = vtkIdTypeArray::New();
            cellarray = vtkCellArray::New();

            return;
        }
        else
        {
            polyDataCompressed = false;
            return;
        }

    }

    MainWin->setSpecificLabel("Compressing...");
    MainWin->setSpecificProgress(0);
    qApp->processEvents();

    QByteArray outdata;

    QDataStream out(&outdata, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);
    //Convert the polydata to an output format, compress, and write
    if (localPolyData == nullptr) return;

    int tcount = localPolyData->GetNumberOfCells();
    int pcount = localPolyData->GetNumberOfPoints();

    out << pcount;
    out << tcount;

    //first output the points
    for (int i = 0; i < pcount; i++)
    {
        double x[3];
        localPolyData->GetPoint(i, x);
        out << x[0] << x[1] << x[2];
    }


    for (int i = 0; i < tcount; i++)
    {
        vtkTriangle *cell = (vtkTriangle *) localPolyData->GetCell(i);
        vtkIdType tri[3]; //vertex indices
        for (vtkIdType j = 0; j < 3; j++) tri[j] = cell->GetPointId(j);
        out << (int)tri[0] << (int)tri[1] << (int)tri[2];
    }

    compressedPolyData = qCompress(outdata, 1); //apparently no gain from higher compression levels (and big speed loss)

    if (MainWin->ui->actionSave_Memory->isChecked())
    {
        localPolyData->Delete();
        localPolyData = vtkPolyData::New();
        verts->Delete();
        verts = vtkPoints::New();
        actualarray->Delete();
        cellarray->Delete();
        actualarray = vtkIdTypeArray::New();
        cellarray = vtkCellArray::New();
        polyDataCompressed = true;
    }
    else  polyDataCompressed = false;
    compressedPolyData.squeeze();

    MainWin->setSpecificLabel("Completed");
    MainWin->setSpecificProgress(100);
    qApp->processEvents();
}

/**
 * @brief SVObject::UnCompressPolyData
 */
void SVObject::UnCompressPolyData()
{
    if (polyDataCompressed == false) return;
    if (vaxml_mode) return;

    MainWin->setSpecificLabel("Decompressing...");
    MainWin->setSpecificProgress(0);
    qApp->processEvents();

    localPolyData->Initialize();
    verts->Initialize();
    cellarray->Initialize();
    actualarray->Initialize();

    QByteArray data = qUncompress(compressedPolyData);

    QDataStream in(&data, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    //convert data from file into polydata structure

    int pcount, tcount;
    in >> pcount;
    in >> tcount;

    verts->SetNumberOfPoints(pcount);
    //localPolyData->Allocate(tcount);

    for (int i = 0; i < pcount; i++)
    {
        double temp0, temp1, temp2;
        in >> temp0 >> temp1 >> temp2;
        verts->InsertPoint(i, temp0, temp1, temp2);
    }

    actualarray->SetNumberOfValues(tcount * 4);


    int pos = 0;
    for (int i = 0; i < tcount; i++)
    {
        int temp0, temp1, temp2;
        in >> temp0 >> temp1 >> temp2;
        actualarray->SetValue(pos++, 3);
        actualarray->SetValue(pos++, temp0);
        actualarray->SetValue(pos++, temp1);
        actualarray->SetValue(pos++, temp2);
    }

    cellarray->SetCells(tcount, actualarray);
    localPolyData->SetPolys(cellarray);
    localPolyData->SetPoints(verts);

    polyDataCompressed = false;
    //compressedPolyData.resize(0);
    compressedPolyData.squeeze(); //free compressed data

    MainWin->setSpecificProgress(100);
    qApp->processEvents();

}

/**
 * @brief SVObject::WritePD
 * @param outfile
 */
void SVObject::WritePD(QFile *outfile)
{
    if (polyDataCompressed == false) CompressPolyData(true);

    QDataStream out(outfile);
    out.setByteOrder(QDataStream::LittleEndian);
    out << compressedPolyData.size();
    outfile->write(compressedPolyData);   //Convert the polydata to an output format, compress, and write
}

/**
 * @brief SVObject::ReadPD
 * @param infile
 */
void SVObject::ReadPD(QFile *infile)
{

    QDataStream in(infile);
    in.setByteOrder(QDataStream::LittleEndian);
    if (spv->FileVersion == 6)
    {
        QMessageBox::critical(MainWin, "Error", "Can't read V6 presurfaced files, sorry");
        QCoreApplication::quit();
    }
    else
    {
        //convert data from file into polydata structure
        int size;
        in >> size;
        compressedPolyData = infile->read(size);
        polyDataCompressed = true;
    }
}

/**
 * @brief SVObject::DoMatrixDXFoutput
 * @param v
 * @param x
 * @param y
 * @param z
 * @return
 */
QString SVObject::DoMatrixDXFoutput(int v, float x, float y, float z)
{

    float *M = matrix;
    float x1, y1, z1;
    QString S;

    x1 = x * M[0] + y * M[4] + z * M[8] + M[12];
    y1 = x * M[1] + y * M[5] + z * M[9] + M[13];
    z1 = x * M[2] + y * M[6] + z * M[10] + M[14];

    S.sprintf("1%d\n%f\n2%d\n%f\n3%d\n%f\n", v, x1, v, y1, v, z1);
    if (v < 2) return S;

    QString T;
    T.sprintf("1%d\n%f\n2%d\n%f\n3%d\n%f\n", v + 1, x1, v + 1, y1, v + 1, z1);

    S += T;
    return S;
}

/**
 * @brief SVObject::WriteDXFfaces
 * @param outfile
 * @return
 */
int SVObject::WriteDXFfaces(QFile *outfile)
{
    QTextStream dxf(outfile);

    QString header, name;

    //work out object number
    int ocount = 1;
    for (int i = 0; i < SVObjects.count(); i++) if (!(SVObjects[i]->IsGroup) && i < Index) ocount++;
    QTextStream ts(&header);
    if (Name.isEmpty()) name.sprintf("%d", ocount);
    else name = Name;

    ts << "0\n3DFACE\n8\n" << name.toLatin1() << "\n62\n" << ocount << "\n";

    GetFinalPolyData(); //do all the VTK stuff

    //count triangles for the record
    Triangles = polydata->GetNumberOfCells();

    MainWin->setSpecificLabel("Creating DXF object");
    qApp->processEvents();

    int tcount = polydata->GetNumberOfCells();
    int count = 0;
    for (int i = 0; i < tcount; i++)
    {
        double tuple[3];
        vtkTriangle *cell = (vtkTriangle *)polydata->GetCell(i);
        vtkIdType tri[3]; //vertex indices
        dxf << header.toLatin1();
        for (vtkIdType j = 0; j < 3; j++)
        {
            tri[j] = cell->GetPointId(j);
            polydata->GetPoint(tri[j], tuple);
            dxf << DoMatrixDXFoutput(j, tuple[0], tuple[1], tuple[2]).toLatin1();
        }
        count++;
        if (count > 1000)
        {
            MainWin->setSpecificProgress((i * 100) / tcount);
            qApp->processEvents();


            count = 0;
        }
    }

    MainWin->setSpecificLabel("Completed");
    MainWin->setSpecificProgress(100); //looks better!
    qApp->processEvents();

    //and delete any vtk objects used
    DeleteVTKObjects();

    return tcount;
}

/**
 * @brief SVObject::AppendCompressedFaces
 * @param mainfile
 * @param internalfile
 * @param qMain
 * @return
 */
int SVObject::AppendCompressedFaces(QString mainfile, QString internalfile, QDataStream *main)
{
    QByteArray b;
    QDataStream stl(&b, QIODevice::WriteOnly);

    GetFinalPolyData(); //do all the VTK stuff

    float *M = matrix;

    //count triangles for the record
    Triangles = polydata->GetNumberOfCells();

    MainWin->setSpecificLabel("Creating object");
    qApp->processEvents();

    //

    vtkPoints *verts = polydata->GetPoints();
    vtkCellArray *cellarray = polydata->GetPolys();
    vtkIdTypeArray *actualarray = cellarray->GetData();

    int vcount = verts->GetNumberOfPoints();
//    qDebug()<<"Writing "<<vcount;
    stl << vcount;

    for (int i = 0; i < vcount; i++)
    {
        double tuple[3];
        verts->GetPoint(i, tuple);
        double x1 = (float)(tuple[0] * M[0] + tuple[1] * M[4] + tuple[2] * M[8] + M[12]);
        double y1 = (float)(tuple[0] * M[1] + tuple[1] * M[5] + tuple[2] * M[9] + M[13]);
        double z1 = (float)(tuple[0] * M[2] + tuple[1] * M[6] + tuple[2] * M[10] + M[14]);
        stl << x1 << y1 << z1;
    }

    int tcount = polydata->GetNumberOfCells();
//    qDebug()<<"Writing "<<tcount;

    stl << tcount;

    for (int i = 0; i < tcount; i++)
    {
        int d;
        //has 4 values per cell, but 1st is always 3 - skip it
        d = actualarray->GetValue(i * 4 + 1);
        stl << d;
        d = actualarray->GetValue(i * 4 + 2);
        stl << d;
        d = actualarray->GetValue(i * 4 + 3);
        stl << d;
    }

    QByteArray b2 = qCompress(b);
//     qDebug()<<"Compressing from"<<b.count()<<"to "<<b2.count();

    (*main) << b2;

    return tcount;
}

/**
 * @brief SVObject::WriteSTLfaces
 * @param stldir
 * @param fname
 * @return
 */
int SVObject::WriteSTLfaces(QDir stldir, QString fname)
{

    QFile stlfile(fname);

    stlfile.open(QIODevice::WriteOnly);

    QDataStream stl(&stlfile);
    stl.setByteOrder(QDataStream::LittleEndian);
    stl.setVersion(QDataStream::Qt_4_5);
    //header
    for (int i = 0; i < 80; i++) stl << (unsigned char)0;

    GetFinalPolyData(); //do all the VTK stuff

    //get a transposed copy of matrix for old calculation
    QMatrix4x4 thematrix(matrix);
    //qDebug()<< "Norm"<<thematrix;
    //thematrix.scale(2.0*((float)qrand())/((float)RAND_MAX));
    //thematrix=thematrix.transposed();
    float *M = thematrix.data();
    //qDebug()<< "Trans"<<thematrix;

    //count triangles for the record
    Triangles = polydata->GetNumberOfCells();

    MainWin->setSpecificLabel("Creating STL object");
    qApp->processEvents();

    int tcount = polydata->GetNumberOfCells();
    stl << tcount;
    float szero = 0;

    int count = 0;
    for (int i = 0; i < tcount; i++)
    {
        //fake triangle normal
        stl << szero;
        stl << szero;
        stl << szero;

        double tuple[3];
        vtkTriangle *cell = (vtkTriangle *)polydata->GetCell(i);
        vtkIdType tri[3]; //vertex indices

        for (vtkIdType j = 0; j < 3; j++)
        {
            tri[j] = cell->GetPointId(j);
            polydata->GetPoint(tri[j], tuple);

            float x1 = (float)(tuple[0] * M[0] + tuple[1] * M[4] + tuple[2] * M[8] + M[12]);
            float y1 = (float)(tuple[0] * M[1] + tuple[1] * M[5] + tuple[2] * M[9] + M[13]);
            float z1 = (float)(tuple[0] * M[2] + tuple[1] * M[6] + tuple[2] * M[10] + M[14]);

            stl << x1 << y1 << z1;
        }
        count++;
        if (count > 1000)
        {
            MainWin->setSpecificProgress((i * 100) / tcount);
            qApp->processEvents();

            count = 0;
        }
        //two bytes of zero
        stl << (unsigned char)0;
        stl << (unsigned char)0;
    }

    MainWin->setSpecificLabel("Complete");
    MainWin->setSpecificProgress(100); //looks better!
    qApp->processEvents();

    //and delete any vtk objects used
    DeleteVTKObjects();

    return tcount;
}

/**
 * @brief SVObject::MakePolyData
 */
void SVObject::MakePolyData()
{
    int VertexCount = 0;
    int TrigCount = 0;
    for (int i = 0; i < Isosurfaces.count(); i++)
    {
        TrigCount += Isosurfaces[i]->nTriangles;
        VertexCount += Isosurfaces[i]->nVertices;
    }

    MainWin->setSpecificLabel("Converting Surface");
    qApp->processEvents();
    verts->SetNumberOfPoints(VertexCount);

    int VertexBase = 0;
    for (int i = 0; i < Isosurfaces.count(); i++)
    {
        MakePolyVerts(i, VertexBase);
        VertexBase += Isosurfaces[i]->nVertices;
        MainWin->setSpecificProgress((i * 50) / Isosurfaces.count());
        qApp->processEvents();
    }

    actualarray->SetNumberOfValues(TrigCount * 4);

    int pos = 0;
    for (int i = 0; i < Isosurfaces.count(); i++)
    {
        int c = Isosurfaces[i]->nTriangles;
        int t = 0;

        for (int trigs = 0; trigs < c; trigs++)
        {
            actualarray->SetValue(pos++, 3);
            actualarray->SetValue(pos++, Isosurfaces[i]->triangles[t++]);
            actualarray->SetValue(pos++, Isosurfaces[i]->triangles[t++]);
            actualarray->SetValue(pos++, Isosurfaces[i]->triangles[t++]);
        }
        MainWin->setSpecificProgress(50 + (i * 50) / Isosurfaces.count());
        qApp->processEvents();
    }

    cellarray->SetCells(TrigCount, actualarray);
    localPolyData->SetPolys(cellarray);
    localPolyData->SetPoints(verts);

    qDeleteAll(Isosurfaces.begin(), Isosurfaces.end());
    Isosurfaces.clear();
    return;
}

/**
 * @brief SVObject::MakePolyVerts
 * @param slice
 * @param VertexBase
 */
void SVObject::MakePolyVerts(int slice, int VertexBase)
//make a polydata object for this slice
//Modified from old MakeDlist. Takes and applies my distortions to mesh, writes into localPolyData structure for VTK normalisation and filtering
{
    int z1;
    float x, y, z, scale;
    float xpos, ypos, zpos;
    float k;
    int ii;
    int zadd;
    int vertex;
    Isosurface *iso;

    iso = Isosurfaces[slice];

    //Now into stripped down version of old MakeDlist

    zadd = 1;
    if (buggedData) zadd = 0; //a version 4 and up bugfix
    scale = (float)(spv->iDim) / (float)SCALE;
    xpos = (float)(spv->iDim) / ((float)(2 * scale));
    ypos = (float)(spv->jDim) / (2 * scale);
    zpos = (float)(spv->kDim) / (2 * scale);
    k = (float)((spv->PixPerMM) / (float)(spv->SlicePerMM));
    /*Initialise index pointers*/

    //First - loop round vertices stretching as appropriate. THEN do triangles.

    //set up some variablees from SPV, as locals for speed (cut out a pointer ref)
    float SkewLeft = spv->SkewLeft;
    float SkewDown = spv->SkewDown;
    double *stretches = spv->stretches;
    bool MirrorFlag = spv->MirrorFlag;
    for (vertex = 0; vertex < (iso->nVertices); vertex++)
    {
        x = ((float) iso->vertices[vertex * 3]) / 2;
        y = ((float) iso->vertices[vertex * 3 + 1]) / 2;
        z1 = iso->vertices[vertex * 3 + 2];
        //     qDebug()<<"Raw " <<x<<y<<z;


        if (z1 % 2 == 0)
            z = (float)stretches[(z1 / 2) + zadd];
        else
            z = ((float)stretches[(z1 / 2) + zadd] + (float)stretches[(z1 / 2) + 1 + zadd]) / 2.0;
        z *= k;
        x += (z * SkewLeft);
        x /= scale;
        x -= xpos;
        y += (z * SkewDown);
        y /= scale;
        //handle pre v4 bug
        if (buggedData) z -= ypos;
        else y -= ypos;

        z /= scale;
        z -= zpos;
        //      qDebug()<<"Inserting " <<x<<y<<z;
        if (MirrorFlag)
            verts->InsertPoint(vertex + VertexBase, x, 0 - y, z);
        else
            verts->InsertPoint(vertex + VertexBase, x, y, z);
    }
}
