#include "svobject.h"
#include "globals.h"
#include "spv.h"
#include "compressedslice.h"
#include "mainwindow.h"

#include <QDebug>
#include <QApplication>
#include <QDir>
#include <QMessageBox>
#include <QList>
#include <QVector3D>
#include <QMatrix4x4>
#include <QtOpenGL/QtOpenGL>
#include <limits>

#include "ui_mainwindow.h"

QList <SVObject *> SVObjects;

/**
 * @brief SVObject::SVObject
 */
SVObject::SVObject(int index)
{
    Index = index;
    Position = index;
    InGroup = -1;
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
    ResampleType = 0;
    ResetMatrix();
    gotdefaultmatrix = false;
    polyDataCompressed = false;
    killme = false;
    AllSlicesCompressed = nullptr;
    scale = 1.0;
    Resample = 100;
    Name = "";
    buggedData = false;
    Key = QChar(0);
    object_ktr = 0;
    usesVBOs = false;
    Shininess = 2;
    donebox = false;
    voxels = 0;
    spv = nullptr;
    isSurfacing = false;
}

/**
 * @brief SVObject::~SVObject
 */
SVObject::~SVObject()
{
    mainWindow->gl3widget->makeCurrent();
    qDeleteAll(compressedslices.begin(), compressedslices.end());
    qDeleteAll(Isosurfaces.begin(), Isosurfaces.end());
    if (AllSlicesCompressed != nullptr)
        free(AllSlicesCompressed);
    qDeleteAll(VertexBuffers);
}

/**
 * @brief SVObject::Parent
 */
int SVObject::Parent()
{
    for (int i = 0; i < SVObjects.count(); i++)
        if (SVObjects[i]->Index == InGroup) return i;
    return -1;
}

/**
 * @brief SVObject::ResetMatrix
 */
void SVObject::ResetMatrix()
{
    matrix[0]  = 1; matrix[1]  = 0; matrix[2]  = 0; matrix[3]  = 0;
    matrix[4]  = 0; matrix[5]  = 1; matrix[6]  = 0; matrix[7]  = 0;
    matrix[8]  = 0; matrix[9]  = 0; matrix[10] = 1; matrix[11] = 0;
    matrix[12] = 0; matrix[13] = 0; matrix[14] = 0; matrix[15] = 1;
}

/**
 * @brief SVObject::DoUpdates
 */
void SVObject::DoUpdates()
{
    if (mainWindow->ui->actionAuto_Resurface->isChecked())
        MakeDlists();
    else
        Dirty = true;
}

/**
 * @brief SVObject::ForceUpdates
 */
void SVObject::ForceUpdates(int thisobj, int totalobj)
{
    if (thisobj >= 0)
    {
        QString status = QString("Reprocessing %1 of %2").arg(thisobj + 1).arg(totalobj);
        mainWindow->ui->OutputLabelOverall->setText(status);
        if (totalobj == 0)
            mainWindow->ui->ProgBarOverall->setValue(100);
        else
            mainWindow->ui->ProgBarOverall->setValue((thisobj * 100) / totalobj);
    }
    MakeDlists();
    CompressPolyData(false);
}

/**
 * @brief SVObject::GetFinalPolyData
 * Filters (decimation, smoothing, island removal) are stubbed as no-ops for now.
 * finalMesh is simply a copy of localMesh.
 * Normal vectors are computed from the mesh geometry.
 */
void SVObject::GetFinalPolyData()
{
    if (IsGroup) return;


    if (polyDataCompressed) UnCompressPolyData();

    // --- Stub: copy localMesh to finalMesh (no decimation/smoothing/island removal) ---
    finalMesh = localMesh;

    if (IslandRemoval != 0)
    {
        MeshAdjacency adj;
        adj.build(finalMesh);
        finalMesh = MeshFilters::removeIslands(finalMesh, adj, IslandRemoval);
    }

    if (Smoothing != 0)
    {
        MeshAdjacency adj;
        adj.build(finalMesh);
        mainWindow->setSpecificLabel("Smoothing");
        mainWindow->setSpecificProgress(0);
        qApp->processEvents();
        finalMesh = MeshFilters::smooth(finalMesh, adj, Smoothing,
                                        [](int pass, int total) {
                                            mainWindow->setSpecificProgress((pass * 100) / total);
                                            qApp->processEvents();
                                        });
    }

    if (Resample != 100)
    {
        MeshAdjacency adj;
        adj.build(finalMesh);
        mainWindow->setSpecificLabel("Simplifying Object");
        mainWindow->setSpecificProgress(0);
        qApp->processEvents();
        int effectiveResampleType = mainWindow->ui->actionQuadric_Fidelity_Reduction->isChecked() ? 1 : ResampleType;
        finalMesh = MeshFilters::decimate(finalMesh, adj, Resample, effectiveResampleType,
                                          [](int current, int total) {
                                              mainWindow->setSpecificProgress((current * 100) / total);
                                              qApp->processEvents();
                                          });
    }

    if (finalMesh.triangleCount() == 0)
    {
        normalx.clear();
        normaly.clear();
        normalz.clear();
        return;
    }


    mainWindow->setSpecificLabel("Calculating Normals");
    qApp->processEvents();

    int tcount = finalMesh.triangleCount();
    int pcount = finalMesh.vertexCount();

    normalx.clear();
    normaly.clear();
    normalz.clear();
    normalx.resize(pcount, 0.0f);
    normaly.resize(pcount, 0.0f);
    normalz.resize(pcount, 0.0f);

    for (int i = 0; i < tcount; i++)
    {
        int t0 = finalMesh.triangles[i * 3];
        int t1 = finalMesh.triangles[i * 3 + 1];
        int t2 = finalMesh.triangles[i * 3 + 2];

        double x0 = finalMesh.vertices[t0 * 3],     y0 = finalMesh.vertices[t0 * 3 + 1], z0 = finalMesh.vertices[t0 * 3 + 2];
        double x1 = finalMesh.vertices[t1 * 3],     y1 = finalMesh.vertices[t1 * 3 + 1], z1 = finalMesh.vertices[t1 * 3 + 2];
        double x2 = finalMesh.vertices[t2 * 3],     y2 = finalMesh.vertices[t2 * 3 + 1], z2 = finalMesh.vertices[t2 * 3 + 2];

        // Cross product (p1-p0) x (p2-p0)
        double ax = x1 - x0, ay = y1 - y0, az = z1 - z0;
        double bx = x2 - x0, by = y2 - y0, bz = z2 - z0;
        float nx = static_cast<float>(ay * bz - az * by);
        float ny = static_cast<float>(az * bx - ax * bz);
        float nz = static_cast<float>(ax * by - ay * bx);

        normalx[t0] += nx; normaly[t0] += ny; normalz[t0] += nz;
        normalx[t1] += nx; normaly[t1] += ny; normalz[t1] += nz;
        normalx[t2] += nx; normaly[t2] += ny; normalz[t2] += nz;

        if (i % 10000 == 0)
        {
            mainWindow->setSpecificProgress((i * 100) / tcount);
            qApp->processEvents();
        }
    }

    // Normalise accumulated normals
    for (int i = 0; i < pcount; i++)
    {
        float len = sqrtf(normalx[i]*normalx[i] + normaly[i]*normaly[i] + normalz[i]*normalz[i]);
        if (len > 1e-6f)
        {
            normalx[i] /= len;
            normaly[i] /= len;
            normalz[i] /= len;
        }
    }
}

/**
 * @brief SVObject::MakeVBOs
 */
void SVObject::MakeVBOs()
{
    if (isSurfacing) return;
    isSurfacing = true;
    if (IsGroup) return;

    QVector<QVector3D> vertices;
    QVector<QVector3D> normals;

    vertices.resize(3 * MAXDLISTSIZE);
    normals.resize(3 * MAXDLISTSIZE);

    usesVBOs = true;

    qDeleteAll(VertexBuffers);
    VertexBuffers.clear();
    VBOVertexCounts.clear();

    GetFinalPolyData();

    // Compute bounding box from finalMesh vertices
    double d[6] = {
        std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(), -std::numeric_limits<double>::max(),
        std::numeric_limits<double>::max(), -std::numeric_limits<double>::max()
    };
    int pcount = finalMesh.vertexCount();
    for (int p = 0; p < pcount; p++)
    {
        double vx = finalMesh.vertices[p * 3];
        double vy = finalMesh.vertices[p * 3 + 1];
        double vz = finalMesh.vertices[p * 3 + 2];
        if (vx < d[0]) d[0] = vx;
        if (vx > d[1]) d[1] = vx;
        if (vy < d[2]) d[2] = vy;
        if (vy > d[3]) d[3] = vy;
        if (vz < d[4]) d[4] = vz;
        if (vz > d[5]) d[5] = vz;
    }

    if (donebox == false)
    {
        objectxmin = d[0]; objectxmax = d[1];
        objectymin = d[2]; objectymax = d[3];
        objectzmin = d[4]; objectzmax = d[5];
        donebox = true;
    }

    // Bounding box buffer
    if (boundingBoxBuffer.isCreated()) boundingBoxBuffer.destroy();
    boundingBoxBuffer.create();
    boundingBoxBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    boundingBoxBuffer.bind();

    QVector<QVector3D> lineVertices;
    lineVertices << QVector3D(static_cast<float>(objectxmin), static_cast<float>(objectymin), static_cast<float>(objectzmin))
                 << QVector3D(static_cast<float>(objectxmax), static_cast<float>(objectymin), static_cast<float>(objectzmin));
    lineVertices << QVector3D(static_cast<float>(objectxmin), static_cast<float>(objectymin), static_cast<float>(objectzmin))
                 << QVector3D(static_cast<float>(objectxmin), static_cast<float>(objectymax), static_cast<float>(objectzmin));
    lineVertices << QVector3D(static_cast<float>(objectxmin), static_cast<float>(objectymin), static_cast<float>(objectzmin))
                 << QVector3D(static_cast<float>(objectxmin), static_cast<float>(objectymin), static_cast<float>(objectzmax));
    lineVertices << QVector3D(static_cast<float>(objectxmax), static_cast<float>(objectymax), static_cast<float>(objectzmax))
                 << QVector3D(static_cast<float>(objectxmin), static_cast<float>(objectymax), static_cast<float>(objectzmax));
    lineVertices << QVector3D(static_cast<float>(objectxmax), static_cast<float>(objectymax), static_cast<float>(objectzmax))
                 << QVector3D(static_cast<float>(objectxmax), static_cast<float>(objectymin), static_cast<float>(objectzmax));
    lineVertices << QVector3D(static_cast<float>(objectxmax), static_cast<float>(objectymax), static_cast<float>(objectzmax))
                 << QVector3D(static_cast<float>(objectxmax), static_cast<float>(objectymax), static_cast<float>(objectzmin));
    lineVertices << QVector3D(static_cast<float>(objectxmax), static_cast<float>(objectymin), static_cast<float>(objectzmin))
                 << QVector3D(static_cast<float>(objectxmax), static_cast<float>(objectymax), static_cast<float>(objectzmin));
    lineVertices << QVector3D(static_cast<float>(objectxmax), static_cast<float>(objectymin), static_cast<float>(objectzmin))
                 << QVector3D(static_cast<float>(objectxmax), static_cast<float>(objectymin), static_cast<float>(objectzmax));
    lineVertices << QVector3D(static_cast<float>(objectxmin), static_cast<float>(objectymax), static_cast<float>(objectzmin))
                 << QVector3D(static_cast<float>(objectxmax), static_cast<float>(objectymax), static_cast<float>(objectzmin));
    lineVertices << QVector3D(static_cast<float>(objectxmin), static_cast<float>(objectymax), static_cast<float>(objectzmin))
                 << QVector3D(static_cast<float>(objectxmin), static_cast<float>(objectymax), static_cast<float>(objectzmax));
    lineVertices << QVector3D(static_cast<float>(objectxmin), static_cast<float>(objectymin), static_cast<float>(objectzmax))
                 << QVector3D(static_cast<float>(objectxmax), static_cast<float>(objectymin), static_cast<float>(objectzmax));
    lineVertices << QVector3D(static_cast<float>(objectxmin), static_cast<float>(objectymin), static_cast<float>(objectzmax))
                 << QVector3D(static_cast<float>(objectxmin), static_cast<float>(objectymax), static_cast<float>(objectzmax));

    boundingBoxBuffer.allocate(24 * 6 * sizeof(GLfloat));
    boundingBoxBuffer.write(0, lineVertices.constData(), 12 * 6 * sizeof(GLfloat));
    boundingBoxBuffer.write(12 * 6 * sizeof(GLfloat), lineVertices.constData(), 12 * 6 * sizeof(GLfloat));
    boundingBoxBuffer.release();

    // Apply matrix to bounding box for global min/max
    QVector3D v1(static_cast<float>(d[0]), static_cast<float>(d[2]), static_cast<float>(d[4]));
    QVector3D v2(static_cast<float>(d[1]), static_cast<float>(d[3]), static_cast<float>(d[5]));
    QMatrix4x4 mat(
        static_cast<float>(matrix[0]),  static_cast<float>(matrix[1]),  static_cast<float>(matrix[2]),  static_cast<float>(matrix[3]),
        static_cast<float>(matrix[4]),  static_cast<float>(matrix[5]),  static_cast<float>(matrix[6]),  static_cast<float>(matrix[7]),
        static_cast<float>(matrix[8]),  static_cast<float>(matrix[9]),  static_cast<float>(matrix[10]), static_cast<float>(matrix[11]),
        static_cast<float>(matrix[12]), static_cast<float>(matrix[13]), static_cast<float>(matrix[14]), static_cast<float>(matrix[15])
        );
    QVector3D v1t = mat.mapVector(v1);
    QVector3D v2t = mat.mapVector(v2);
    d[0] = static_cast<double>(v1t.x()); d[1] = static_cast<double>(v2t.x());
    d[2] = static_cast<double>(v1t.y()); d[3] = static_cast<double>(v2t.y());
    d[4] = static_cast<double>(v1t.z()); d[5] = static_cast<double>(v2t.z());

    if (d[0] < static_cast<double>(minX) || isFirstObject) minX = static_cast<float>(d[0]);
    if (d[1] > static_cast<double>(maxX) || isFirstObject) maxX = static_cast<float>(d[1]);
    if (d[2] < static_cast<double>(minY) || isFirstObject) minY = static_cast<float>(d[2]);
    if (d[3] > static_cast<double>(maxY) || isFirstObject) maxY = static_cast<float>(d[3]);
    if (d[4] < static_cast<double>(minZ) || isFirstObject) minZ = static_cast<float>(d[4]);
    if (d[5] > static_cast<double>(maxZ) || isFirstObject) maxZ = static_cast<float>(d[5]);

    int tcount = finalMesh.triangleCount();
    Triangles = tcount;

    mainWindow->setSpecificLabel("Creating VBO objects");
    qApp->processEvents();

    int i = 0, sizethisvao;
    while (i < tcount)
    {
        sizethisvao = (tcount - i > MAXDLISTSIZE) ? MAXDLISTSIZE : tcount - i;

        int index = 0;
        for (int k = 0; k < sizethisvao; k++)
        {
            int t0 = finalMesh.triangles[i * 3];
            int t1 = finalMesh.triangles[i * 3 + 1];
            int t2 = finalMesh.triangles[i * 3 + 2];
            int tri[3] = { t0, t1, t2 };

            for (int j = 0; j < 3; j++)
            {
                int v = tri[j];
                normals[index]  = QVector3D(normalx[v], normaly[v], normalz[v]);
                vertices[index] = QVector3D(
                    finalMesh.vertices[v * 3],
                    finalMesh.vertices[v * 3 + 1],
                    finalMesh.vertices[v * 3 + 2]
                    );
                index++;
            }
            i++;
        }

        VBOVertexCounts.append(sizethisvao * 3);

        QOpenGLBuffer *vbuffer = new QOpenGLBuffer;
        vbuffer->create();
        vbuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        vbuffer->bind();
        vbuffer->allocate(18 * sizethisvao * static_cast<int>(sizeof(GLfloat)));
        vbuffer->write(0, vertices.constData(), sizethisvao * 9 * static_cast<int>(sizeof(GLfloat)));
        vbuffer->write(sizethisvao * 9 * static_cast<int>(sizeof(GLfloat)), normals.constData(), sizethisvao * 9 * static_cast<int>(sizeof(GLfloat)));
        VertexBuffers.append(vbuffer);

        mainWindow->setSpecificProgress((i * 100) / tcount);
        if (i % 1000 == 0) qApp->processEvents();
    }

    mainWindow->setSpecificLabel("Completed");
    mainWindow->setSpecificProgress(100);
    qApp->processEvents();

    Dirty = false;
    modelKTr -= object_ktr;
    object_ktr = tcount;
    modelKTr += tcount;
    isSurfacing = false;
}


/**
 * @brief SVObject::setMesh
 * Sets localMesh directly — used by VAXML import to inject STL/PLY geometry
 * without going through the isosurface pipeline.
 * @param mesh  The MeshData to adopt as localMesh.
 */
void SVObject::setMesh(const MeshData &mesh)
{
    localMesh = mesh;
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
 */
void SVObject::CompressPolyData(bool flag)
{
    if (IsGroup) return;
    if (isVaxmlMode) return;
    if (polyDataCompressed == true) return;
    if (flag == false && !(mainWindow->ui->actionSave_Memory->isChecked()))
        return;

    polyDataCompressed = true;

    if (compressedPolyData.size() > 0)
    {
        if (mainWindow->ui->actionSave_Memory->isChecked())
        {
            localMesh.clear();
            return;
        }
        else
        {
            polyDataCompressed = false;
            return;
        }
    }

    mainWindow->setSpecificLabel("Compressing...");
    mainWindow->setSpecificProgress(0);
    qApp->processEvents();

    QByteArray outdata;
    QDataStream out(&outdata, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);

    int pcount = localMesh.vertexCount();
    int tcount = localMesh.triangleCount();
    out << pcount << tcount;

    for (int i = 0; i < pcount; i++)
    {
        double x = static_cast<double>(localMesh.vertices[i * 3]);
        double y = static_cast<double>(localMesh.vertices[i * 3 + 1]);
        double z = static_cast<double>(localMesh.vertices[i * 3 + 2]);
        out << x << y << z;
    }

    for (int i = 0; i < tcount; i++)
    {
        int t0 = localMesh.triangles[i * 3];
        int t1 = localMesh.triangles[i * 3 + 1];
        int t2 = localMesh.triangles[i * 3 + 2];
        out << t0 << t1 << t2;
    }

    compressedPolyData = qCompress(outdata, 1);

    if (mainWindow->ui->actionSave_Memory->isChecked())
    {
        localMesh.clear();
        polyDataCompressed = true;
    }
    else
        polyDataCompressed = false;

    compressedPolyData.squeeze();

    mainWindow->setSpecificLabel("Completed");
    mainWindow->setSpecificProgress(100);
    qApp->processEvents();
}

/**
 * @brief SVObject::UnCompressPolyData
 */
void SVObject::UnCompressPolyData()
{
    if (polyDataCompressed == false) return;
    if (isVaxmlMode) return;

    mainWindow->setSpecificLabel("Decompressing...");
    mainWindow->setSpecificProgress(0);
    qApp->processEvents();

    localMesh.clear();

    QByteArray data = qUncompress(compressedPolyData);
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);

    int pcount, tcount;
    in >> pcount >> tcount;

    localMesh.vertices.resize(pcount * 3);
    for (int i = 0; i < pcount; i++)
    {
        double x, y, z;
        in >> x >> y >> z;
        localMesh.vertices[i * 3]     = static_cast<float>(x);
        localMesh.vertices[i * 3 + 1] = static_cast<float>(y);
        localMesh.vertices[i * 3 + 2] = static_cast<float>(z);
    }

    localMesh.triangles.resize(tcount * 3);
    for (int i = 0; i < tcount; i++)
    {
        int t0, t1, t2;
        in >> t0 >> t1 >> t2;
        localMesh.triangles[i * 3]     = t0;
        localMesh.triangles[i * 3 + 1] = t1;
        localMesh.triangles[i * 3 + 2] = t2;
    }

    polyDataCompressed = false;
    compressedPolyData.squeeze();

    mainWindow->setSpecificProgress(100);
    qApp->processEvents();
}

/**
 * @brief SVObject::WritePD
 */
void SVObject::WritePD(QFile *outfile)
{
    if (polyDataCompressed == false) CompressPolyData(true);

    QDataStream out(outfile);
    out.setByteOrder(QDataStream::LittleEndian);
    out << compressedPolyData.size();
    outfile->write(compressedPolyData);
}

/**
 * @brief SVObject::ReadPD
 */
void SVObject::ReadPD(QFile *infile)
{
    QDataStream in(infile);
    in.setByteOrder(QDataStream::LittleEndian);
    if (spv->FileVersion == 6)
    {
        QMessageBox::critical(mainWindow, "Error", "Can't read V6 presurfaced files, sorry");
        QCoreApplication::quit();
    }
    else
    {
        int size;
        in >> size;
        compressedPolyData = infile->read(size);
        polyDataCompressed = true;
    }
}

/**
 * @brief SVObject::DoMatrixDXFoutput
 */
QString SVObject::DoMatrixDXFoutput(int v, float x, float y, float z)
{
    float *M = matrix;
    float x1 = x * M[0] + y * M[4] + z * M[8]  + M[12];
    float y1 = x * M[1] + y * M[5] + z * M[9]  + M[13];
    float z1 = x * M[2] + y * M[6] + z * M[10] + M[14];

    QString string = QString("1%1\n%2\n2%3\n%4\n3%5\n%6\n")
                         .arg(v).arg(static_cast<double>(x1))
                         .arg(v).arg(static_cast<double>(y1))
                         .arg(v).arg(static_cast<double>(z1));

    if (v < 2) return string;

    QString string2 = QString("1%1\n%2\n2%3\n%4\n3%5\n%6\n")
                          .arg(v + 1).arg(static_cast<double>(x1))
                          .arg(v + 1).arg(static_cast<double>(y1))
                          .arg(v + 1).arg(static_cast<double>(z1));

    string.append(string2);
    return string;
}

/**
 * @brief SVObject::WriteDXFfaces
 */
int SVObject::WriteDXFfaces(QFile *outfile)
{
    QTextStream dxf(outfile);
    QString header, name;

    int ocount = 1;
    for (int i = 0; i < SVObjects.count(); i++)
        if (!(SVObjects[i]->IsGroup) && i < Index) ocount++;

    QTextStream ts(&header);
    name = Name.isEmpty() ? QString("%1").arg(ocount) : Name;
    ts << "0\n3DFACE\n8\n" << name.toLatin1() << "\n62\n" << ocount << "\n";

    GetFinalPolyData();

    int tcount = finalMesh.triangleCount();
    Triangles = tcount;

    mainWindow->setSpecificLabel("Creating DXF object");
    qApp->processEvents();

    int count = 0;
    for (int i = 0; i < tcount; i++)
    {
        dxf << header.toLatin1();
        int tri[3] = {
            finalMesh.triangles[i * 3],
            finalMesh.triangles[i * 3 + 1],
            finalMesh.triangles[i * 3 + 2]
        };
        for (int j = 0; j < 3; j++)
        {
            int v = tri[j];
            dxf << DoMatrixDXFoutput(j,
                                     finalMesh.vertices[v * 3],
                                     finalMesh.vertices[v * 3 + 1],
                                     finalMesh.vertices[v * 3 + 2]).toLatin1();
        }
        if (++count > 1000)
        {
            mainWindow->setSpecificProgress((i * 100) / tcount);
            qApp->processEvents();
            count = 0;
        }
    }

    mainWindow->setSpecificLabel("Completed");
    mainWindow->setSpecificProgress(100);
    qApp->processEvents();
    return tcount;
}

/**
 * @brief SVObject::AppendCompressedFaces
 */
int SVObject::AppendCompressedFaces(QString mainfile, QString internalfile, QDataStream *main)
{
    Q_UNUSED(mainfile)
    Q_UNUSED(internalfile)

    QByteArray b;
    QDataStream stl(&b, QIODevice::WriteOnly);

    GetFinalPolyData();

    float *M = matrix;
    int tcount = finalMesh.triangleCount();
    int vcount = finalMesh.vertexCount();
    Triangles = tcount;

    mainWindow->setSpecificLabel("Creating object");
    qApp->processEvents();

    stl << vcount;
    for (int i = 0; i < vcount; i++)
    {
        double px = finalMesh.vertices[i * 3];
        double py = finalMesh.vertices[i * 3 + 1];
        double pz = finalMesh.vertices[i * 3 + 2];
        double x1 = static_cast<double>(static_cast<float>(px * M[0] + py * M[4] + pz * M[8]  + M[12]));
        double y1 = static_cast<double>(static_cast<float>(px * M[1] + py * M[5] + pz * M[9]  + M[13]));
        double z1 = static_cast<double>(static_cast<float>(px * M[2] + py * M[6] + pz * M[10] + M[14]));
        stl << x1 << y1 << z1;
    }

    stl << tcount;
    for (int i = 0; i < tcount; i++)
    {
        stl << finalMesh.triangles[i * 3];
        stl << finalMesh.triangles[i * 3 + 1];
        stl << finalMesh.triangles[i * 3 + 2];
    }

    QByteArray b2 = qCompress(b);
    (*main) << b2;
    return tcount;
}

/**
 * @brief SVObject::WriteSTLfaces
 */
int SVObject::WriteSTLfaces(QDir stldir, QString fname)
{
    Q_UNUSED(stldir)

    QFile stlfile(fname);
    stlfile.open(QIODevice::WriteOnly);
    QDataStream stl(&stlfile);
    stl.setByteOrder(QDataStream::LittleEndian);
    stl.setVersion(QDataStream::Qt_4_5);

    for (int i = 0; i < 80; i++) stl << static_cast<unsigned char>(0);

    GetFinalPolyData();

    QMatrix4x4 thematrix(matrix);
    float *M = thematrix.data();

    int tcount = finalMesh.triangleCount();
    Triangles = tcount;

    mainWindow->setSpecificLabel("Creating STL object");
    qApp->processEvents();

    stl << tcount;
    float szero = 0;
    int count = 0;

    for (int i = 0; i < tcount; i++)
    {
        stl << szero << szero << szero; // fake normal

        int tri[3] = {
            finalMesh.triangles[i * 3],
            finalMesh.triangles[i * 3 + 1],
            finalMesh.triangles[i * 3 + 2]
        };
        for (int j = 0; j < 3; j++)
        {
            int v = tri[j];
            double px = finalMesh.vertices[v * 3];
            double py = finalMesh.vertices[v * 3 + 1];
            double pz = finalMesh.vertices[v * 3 + 2];
            float x1 = static_cast<float>(px * M[0] + py * M[4] + pz * M[8]  + M[12]);
            float y1 = static_cast<float>(px * M[1] + py * M[5] + pz * M[9]  + M[13]);
            float z1 = static_cast<float>(px * M[2] + py * M[6] + pz * M[10] + M[14]);
            stl << x1 << y1 << z1;
        }
        if (++count > 1000)
        {
            mainWindow->setSpecificProgress((i * 100) / tcount);
            qApp->processEvents();
            count = 0;
        }
        stl << static_cast<unsigned char>(0) << static_cast<unsigned char>(0);
    }

    mainWindow->setSpecificLabel("Complete");
    mainWindow->setSpecificProgress(100);
    qApp->processEvents();
    return tcount;
}

/**
 * @brief SVObject::MakePolyData
 */
void SVObject::MakePolyData()
{
    int vertexCount = 0;
    int trigCount = 0;
    for (int i = 0; i < Isosurfaces.count(); i++)
    {
        trigCount  += Isosurfaces[i]->nTriangles;
        vertexCount += Isosurfaces[i]->nVertices;
    }

    mainWindow->setSpecificLabel("Converting Surface");
    qApp->processEvents();

    localMesh.vertices.resize(vertexCount * 3);
    localMesh.triangles.resize(trigCount * 3);

    int vertexBase = 0;
    int trigBase = 0;
    for (int i = 0; i < Isosurfaces.count(); i++)
    {
        MakePolyVerts(i, vertexBase);

        int c = Isosurfaces[i]->nTriangles;
        for (int t = 0; t < c; t++)
        {
            localMesh.triangles[trigBase * 3]     = Isosurfaces[i]->triangles[t * 3] ;
            localMesh.triangles[trigBase * 3 + 1] = Isosurfaces[i]->triangles[t * 3 + 1];
            localMesh.triangles[trigBase * 3 + 2] = Isosurfaces[i]->triangles[t * 3 + 2];
            trigBase++;
        }

        vertexBase += Isosurfaces[i]->nVertices;
        mainWindow->setSpecificProgress((i * 100) / Isosurfaces.count());
        qApp->processEvents();
    }

    qDeleteAll(Isosurfaces.begin(), Isosurfaces.end());
    Isosurfaces.clear();
}

/**
 * @brief SVObject::MakePolyVerts
 */
void SVObject::MakePolyVerts(int slice, int vertexBase)
{
    int z1;
    float x, y, z, scale;
    float xpos, ypos, zpos;
    float k;
    int zadd;
    int vertex;
    Isosurface *iso = Isosurfaces[slice];

    zadd = 1;
    if (buggedData) zadd = 0;
    scale = static_cast<float>(spv->iDim) / static_cast<float>(SCALE);
    xpos  = static_cast<float>(spv->iDim) / (static_cast<float>(2 * scale));
    ypos  = static_cast<float>(spv->jDim) / (2 * scale);
    zpos  = static_cast<float>(spv->kDim) / (2 * scale);
    k     = static_cast<float>(spv->PixPerMM) / static_cast<float>(spv->SlicePerMM);

    float SkewLeft    = static_cast<float>(spv->SkewLeft);
    float SkewDown    = static_cast<float>(spv->SkewDown);
    double *stretches = spv->stretches;
    bool MirrorFlag   = spv->MirrorFlag;

    for (vertex = 0; vertex < iso->nVertices; vertex++)
    {
        x  = static_cast<float>(iso->vertices[vertex * 3])     / 2;
        y  = static_cast<float>(iso->vertices[vertex * 3 + 1]) / 2;
        z1 = iso->vertices[vertex * 3 + 2];

        if (z1 % 2 == 0)
            z = static_cast<float>(stretches[(z1 / 2) + zadd]);
        else
            z = (static_cast<float>(stretches[(z1 / 2) + zadd]) + static_cast<float>(stretches[(z1 / 2) + 1 + zadd])) / 2.0f;

        z *= k;
        x += (z * SkewLeft);
        x /= scale;
        x -= xpos;
        y += (z * SkewDown);
        y /= scale;
        if (buggedData)
            z -= ypos;
        else
            y -= ypos;
        z /= scale;
        z -= zpos;

        int idx = (vertex + vertexBase) * 3;
        if (MirrorFlag)
        {
            localMesh.vertices[idx]     = x;
            localMesh.vertices[idx + 1] = -y;
            localMesh.vertices[idx + 2] = z;
        }
        else
        {
            localMesh.vertices[idx]     = x;
            localMesh.vertices[idx + 1] = y;
            localMesh.vertices[idx + 2] = z;
        }
    }
}