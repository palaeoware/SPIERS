/**
 * @file
 * Source: FBX Exporter
 *
 * All SPIERS code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERS code is Copyright 2008-2026 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "fbxexporter.h"
#include "svobject.h"

#include <QFile>
#include <QDataStream>
#include <QVector>
#include <QBuffer>
#include <cmath>

bool FBXExporter::exportToFBX(
    const QString &fbxFilepath,
    const QList<SVObject*> &visibleObjects)
{
    QFile fbxFile(fbxFilepath);
    if (!fbxFile.open(QIODevice::WriteOnly))
    {
        return false;
    }

    QDataStream out(&fbxFile);
    out.setByteOrder(QDataStream::LittleEndian);

    if (!writeFBXHeader(out))
    {
        fbxFile.close();
        return false;
    }

    if (!writeFBXDocument(out, visibleObjects))
    {
        fbxFile.close();
        return false;
    }

    if (!writeFBXFooter(out))
    {
        fbxFile.close();
        return false;
    }

    fbxFile.close();
    return true;
}

QList<float> FBXExporter::applyMatrixToVertices(
    const QList<float> &vertices,
    const float matrix[16])
{
    QList<float> result;
    result.reserve(vertices.size());

    for (int i = 0; i < vertices.size(); i += 3)
    {
        float x = vertices[i];
        float y = vertices[i + 1];
        float z = vertices[i + 2];

        float xNew = matrix[0] * x + matrix[4] * y + matrix[8] * z + matrix[12];
        float yNew = matrix[1] * x + matrix[5] * y + matrix[9] * z + matrix[13];
        float zNew = matrix[2] * x + matrix[6] * y + matrix[10] * z + matrix[14];

        result.append(xNew);
        result.append(yNew);
        result.append(zNew);
    }

    return result;
}

QList<float> FBXExporter::computeVertexNormals(
    const QList<float> &vertices,
    const QList<int> &triangles)
{
    int vertexCount = vertices.size() / 3;
    QList<float> normals;
    normals.reserve(vertexCount * 3);

    for (int i = 0; i < vertexCount * 3; i++)
    {
        normals.append(0.0f);
    }

    for (int i = 0; i < triangles.size(); i += 3)
    {
        int idx0 = triangles[i];
        int idx1 = triangles[i + 1];
        int idx2 = triangles[i + 2];

        float x0 = vertices[idx0 * 3];
        float y0 = vertices[idx0 * 3 + 1];
        float z0 = vertices[idx0 * 3 + 2];

        float x1 = vertices[idx1 * 3];
        float y1 = vertices[idx1 * 3 + 1];
        float z1 = vertices[idx1 * 3 + 2];

        float x2 = vertices[idx2 * 3];
        float y2 = vertices[idx2 * 3 + 1];
        float z2 = vertices[idx2 * 3 + 2];

        float edge1x = x1 - x0;
        float edge1y = y1 - y0;
        float edge1z = z1 - z0;

        float edge2x = x2 - x0;
        float edge2y = y2 - y0;
        float edge2z = z2 - z0;

        float nx = edge1y * edge2z - edge1z * edge2y;
        float ny = edge1z * edge2x - edge1x * edge2z;
        float nz = edge1x * edge2y - edge1y * edge2x;

        normals[idx0 * 3] += nx;
        normals[idx0 * 3 + 1] += ny;
        normals[idx0 * 3 + 2] += nz;

        normals[idx1 * 3] += nx;
        normals[idx1 * 3 + 1] += ny;
        normals[idx1 * 3 + 2] += nz;

        normals[idx2 * 3] += nx;
        normals[idx2 * 3 + 1] += ny;
        normals[idx2 * 3 + 2] += nz;
    }

    for (int i = 0; i < normals.size(); i += 3)
    {
        float nx = normals[i];
        float ny = normals[i + 1];
        float nz = normals[i + 2];

        float length = std::sqrt(nx * nx + ny * ny + nz * nz);
        if (length > 1e-6f)
        {
            normals[i] = -nx / length;
            normals[i + 1] = -ny / length;
            normals[i + 2] = -nz / length;
        }
        else
        {
            normals[i] = 0.0f;
            normals[i + 1] = 0.0f;
            normals[i + 2] = 1.0f;
        }
    }

    return normals;
}

bool FBXExporter::writeFBXHeader(QDataStream &outFile)
{
    static const char magic[] = "Kaydara FBX Binary  \x00\x1A\x00";
    outFile.writeRawData(magic, 23);
    quint32 version = 7300;
    outFile << version;
    return outFile.status() == QDataStream::Ok;
}

/**
 *
 * Helper: write a 13-byte null record that terminates a node's child list.
 *
 **/
static void writeNullRecord(QDataStream &out)
{
    for (int i = 0; i < 13; i++)
        out << static_cast<quint8>(0);
}

/**
 *
 * Helper: encode an int32 property (1 byte type code 'I' + 4 bytes value).
 *
 **/
static void encodeInt32Property(QDataStream &out, qint32 value)
{
    out << static_cast<quint8>('I');
    out << value;
}

/**
 *
 * Helper: encode an int64 property (1 byte type code 'L' + 8 bytes value).
 *
 **/
static void encodeInt64Property(QDataStream &out, qint64 value)
{
    out << static_cast<quint8>('L');
    out << value;
}

/**
 *
 * Helper: encode an int32 array property (type 'i').
 *
 **/
static void encodeInt32ArrayProperty(QDataStream &out, const QList<qint32> &data)
{
    out << static_cast<quint8>('i');
    out << static_cast<quint32>(data.size());
    out << static_cast<quint32>(0);  // Encoding: 0 = uncompressed
    out << static_cast<quint32>(data.size() * 4);
    for (qint32 i : data)
        out << i;
}

/**
 *
 * Helper: encode a string property (type 'S').
 * Format: 'S' + length(u32) + string data
 *
 **/
static void encodeStringProperty(QDataStream &out, const QByteArray &str)
{
    out << static_cast<quint8>('S');
    out << static_cast<quint32>(str.size());
    out.writeRawData(str.constData(), str.size());
}

/**
 *
 * Helper: encode a double array property (type 'd').
 * Format: 'd' + count(u32) + encoding(u32, 0=uncompressed) + compressedLen(u32) + data
 *
 **/
static void encodeDoubleArrayProperty(QDataStream &out, const QList<double> &data)
{
    out << static_cast<quint8>('d');
    out << static_cast<quint32>(data.size());
    out << static_cast<quint32>(0);  // Encoding: 0 = uncompressed
    out << static_cast<quint32>(data.size() * 8);
    for (double d : data)
        out << d;
}

/**
 * Helper: helper for a double property.
 */
static void encodeDoubleProperty(QDataStream &out, double value)
{
    out << static_cast<quint8>('D');
    out << value;
}

struct FBXEncodedProperty70
{
    QByteArray data;
    quint32 propertyCount = 0;
};


static FBXEncodedProperty70 encodeIntProperty70(const QByteArray &name, qint32 value)
{
    FBXEncodedProperty70 result;

    QDataStream out(&result.data, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);

    // P: "Name", "int", "Integer", "", value
    encodeStringProperty(out, name);
    encodeStringProperty(out, QByteArray("int"));
    encodeStringProperty(out, QByteArray("Integer"));
    encodeStringProperty(out, QByteArray(""));
    encodeInt32Property(out, value);

    result.propertyCount = 5;
    return result;
}

static FBXEncodedProperty70 encodeDoubleProperty70(const QByteArray &name, double value)
{
    FBXEncodedProperty70 result;

    QDataStream out(&result.data, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::LittleEndian);

    // P: "Name", "double", "Number", "", value
    encodeStringProperty(out, name);
    encodeStringProperty(out, QByteArray("double"));
    encodeStringProperty(out, QByteArray("Number"));
    encodeStringProperty(out, QByteArray(""));
    encodeDoubleProperty(out, value);

    result.propertyCount = 5;
    return result;
}

/**
 *
 * Helper: write a child node containing a single array property.
 * Used for Vertices, PolygonVertexIndex, etc.
 * Returns the buffer containing the complete node (header + property + null terminator).
 * Note: endOffset values in the returned buffer are RELATIVE to buffer start (0).
 * Caller must adjust by adding the absolute parent start position when needed.
 *
 **/
bool FBXExporter::writeFBXDocument(QDataStream &outFile, const QList<SVObject*> &visibleObjects)
{
    // Helper lambda: get absolute position in output file
    auto getAbsolutePos = [&outFile]() {
        return static_cast<quint32>(outFile.device()->pos());
    };

    // === FBXHeaderExtension node (TOP-LEVEL SIBLING 1) ===
    // endOffset must be ABSOLUTE file position, not relative sum
    {
        quint32 headerNodeStart = getAbsolutePos();
        quint32 headerNodeHeaderSize = 4 + 4 + 4 + 1 + 18; // "FBXHeaderExtension"

        QByteArray headerChildren;
        {
            QDataStream headerStream(&headerChildren, QIODevice::WriteOnly);
            headerStream.setByteOrder(QDataStream::LittleEndian);

            quint32 childBufPos = 0;

            // FBXHeaderVersion
            {
                QByteArray prop;
                QDataStream propStream(&prop, QIODevice::WriteOnly);
                propStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(propStream, 1003);

                quint32 childHeaderSize = 4 + 4 + 4 + 1 + 16; // "FBXHeaderVersion"
                quint32 childStart = headerNodeStart + headerNodeHeaderSize + childBufPos;
                quint32 childEnd = childStart + childHeaderSize + prop.size() + 13;

                headerStream << childEnd;
                headerStream << static_cast<quint32>(1);
                headerStream << static_cast<quint32>(prop.size());
                headerStream << static_cast<quint8>(16);
                headerStream.writeRawData("FBXHeaderVersion", 16);
                headerStream.writeRawData(prop.constData(), prop.size());
                writeNullRecord(headerStream);

                childBufPos += childHeaderSize + prop.size() + 13;
            }

            // FBXVersion
            {
                QByteArray prop;
                QDataStream propStream(&prop, QIODevice::WriteOnly);
                propStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(propStream, 7300);

                quint32 childHeaderSize = 4 + 4 + 4 + 1 + 10; // "FBXVersion"
                quint32 childStart = headerNodeStart + headerNodeHeaderSize + childBufPos;
                quint32 childEnd = childStart + childHeaderSize + prop.size() + 13;

                headerStream << childEnd;
                headerStream << static_cast<quint32>(1);
                headerStream << static_cast<quint32>(prop.size());
                headerStream << static_cast<quint8>(10);
                headerStream.writeRawData("FBXVersion", 10);
                headerStream.writeRawData(prop.constData(), prop.size());
                writeNullRecord(headerStream);

                childBufPos += childHeaderSize + prop.size() + 13;
            }

            // Creator
            {
                QByteArray prop;
                QDataStream propStream(&prop, QIODevice::WriteOnly);
                propStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(propStream, QByteArray("SPIERS FBX Exporter"));

                quint32 childHeaderSize = 4 + 4 + 4 + 1 + 7; // "Creator"
                quint32 childStart = headerNodeStart + headerNodeHeaderSize + childBufPos;
                quint32 childEnd = childStart + childHeaderSize + prop.size() + 13;

                headerStream << childEnd;
                headerStream << static_cast<quint32>(1);
                headerStream << static_cast<quint32>(prop.size());
                headerStream << static_cast<quint8>(7);
                headerStream.writeRawData("Creator", 7);
                headerStream.writeRawData(prop.constData(), prop.size());
                writeNullRecord(headerStream);

                childBufPos += childHeaderSize + prop.size() + 13;
            }

            // Terminate FBXHeaderExtension children
            writeNullRecord(headerStream);
        }

        quint32 headerNodeEnd = headerNodeStart + headerNodeHeaderSize + headerChildren.size();

        outFile << headerNodeEnd;
        outFile << static_cast<quint32>(0);
        outFile << static_cast<quint32>(0);
        outFile << static_cast<quint8>(18);
        outFile.writeRawData("FBXHeaderExtension", 18);
        outFile.writeRawData(headerChildren.constData(), headerChildren.size());
    }

    // === GlobalSettings node (TOP-LEVEL SIBLING 1.5) ===
    {
        quint32 globalNodeStart = getAbsolutePos();
        quint32 globalNodeHeaderSize = 4 + 4 + 4 + 1 + 14; // "GlobalSettings"

        QByteArray globalChildren;
        {
            QDataStream globalStream(&globalChildren, QIODevice::WriteOnly);
            globalStream.setByteOrder(QDataStream::LittleEndian);

            quint32 childBufPos = 0;

            // ----------------------------
            // Version child
            // ----------------------------
            QByteArray versionProp;
            {
                QDataStream propStream(&versionProp, QIODevice::WriteOnly);
                propStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(propStream, 1000);
            }

            quint32 versionHeaderSize = 4 + 4 + 4 + 1 + 7; // "Version"
            quint32 versionNodeStart = globalNodeStart + globalNodeHeaderSize + childBufPos;
            quint32 versionNodeEnd = versionNodeStart + versionHeaderSize + versionProp.size() + 13;

            globalStream << versionNodeEnd;
            globalStream << static_cast<quint32>(1);
            globalStream << static_cast<quint32>(versionProp.size());
            globalStream << static_cast<quint8>(7);
            globalStream.writeRawData("Version", 7);
            globalStream.writeRawData(versionProp.constData(), versionProp.size());
            writeNullRecord(globalStream);

            childBufPos += versionHeaderSize + versionProp.size() + 13;

            // ----------------------------
            // Build Properties70 payload
            // ----------------------------
            FBXEncodedProperty70 pUpAxisProp          = encodeIntProperty70(QByteArray("UpAxis"), 1);
            FBXEncodedProperty70 pUpAxisSignProp      = encodeIntProperty70(QByteArray("UpAxisSign"), 1);
            FBXEncodedProperty70 pFrontAxisProp       = encodeIntProperty70(QByteArray("FrontAxis"), 2);
            FBXEncodedProperty70 pFrontAxisSignProp   = encodeIntProperty70(QByteArray("FrontAxisSign"), 1);
            FBXEncodedProperty70 pCoordAxisProp       = encodeIntProperty70(QByteArray("CoordAxis"), 0);
            FBXEncodedProperty70 pCoordAxisSignProp   = encodeIntProperty70(QByteArray("CoordAxisSign"), 1);
            FBXEncodedProperty70 pUnitScaleFactorProp = encodeDoubleProperty70(QByteArray("UnitScaleFactor"), 1.0);

            quint32 props70HeaderSize = 4 + 4 + 4 + 1 + 12; // "Properties70"
            quint32 pHeaderSize = 4 + 4 + 4 + 1 + 1;        // "P"

            quint32 pUpAxisNodeSize          = pHeaderSize + pUpAxisProp.data.size() + 13;
            quint32 pUpAxisSignNodeSize      = pHeaderSize + pUpAxisSignProp.data.size() + 13;
            quint32 pFrontAxisNodeSize       = pHeaderSize + pFrontAxisProp.data.size() + 13;
            quint32 pFrontAxisSignNodeSize   = pHeaderSize + pFrontAxisSignProp.data.size() + 13;
            quint32 pCoordAxisNodeSize       = pHeaderSize + pCoordAxisProp.data.size() + 13;
            quint32 pCoordAxisSignNodeSize   = pHeaderSize + pCoordAxisSignProp.data.size() + 13;
            quint32 pUnitScaleFactorNodeSize = pHeaderSize + pUnitScaleFactorProp.data.size() + 13;

            quint32 props70NodeStart = globalNodeStart + globalNodeHeaderSize + childBufPos;

            quint32 pUpAxisStart = props70NodeStart + props70HeaderSize;
            quint32 pUpAxisEnd = pUpAxisStart + pUpAxisNodeSize;

            quint32 pUpAxisSignStart = pUpAxisEnd;
            quint32 pUpAxisSignEnd = pUpAxisSignStart + pUpAxisSignNodeSize;

            quint32 pFrontAxisStart = pUpAxisSignEnd;
            quint32 pFrontAxisEnd = pFrontAxisStart + pFrontAxisNodeSize;

            quint32 pFrontAxisSignStart = pFrontAxisEnd;
            quint32 pFrontAxisSignEnd = pFrontAxisSignStart + pFrontAxisSignNodeSize;

            quint32 pCoordAxisStart = pFrontAxisSignEnd;
            quint32 pCoordAxisEnd = pCoordAxisStart + pCoordAxisNodeSize;

            quint32 pCoordAxisSignStart = pCoordAxisEnd;
            quint32 pCoordAxisSignEnd = pCoordAxisSignStart + pCoordAxisSignNodeSize;

            quint32 pUnitScaleFactorStart = pCoordAxisSignEnd;
            quint32 pUnitScaleFactorEnd = pUnitScaleFactorStart + pUnitScaleFactorNodeSize;

            quint32 props70NodeEnd = pUnitScaleFactorEnd + 13; // child-list terminator

            // Write Properties70 node
            globalStream << props70NodeEnd;
            globalStream << static_cast<quint32>(0);
            globalStream << static_cast<quint32>(0);
            globalStream << static_cast<quint8>(12);
            globalStream.writeRawData("Properties70", 12);

            // P: UpAxis
            globalStream << pUpAxisEnd;
            globalStream << pUpAxisProp.propertyCount;
            globalStream << static_cast<quint32>(pUpAxisProp.data.size());
            globalStream << static_cast<quint8>(1);
            globalStream.writeRawData("P", 1);
            globalStream.writeRawData(pUpAxisProp.data.constData(), pUpAxisProp.data.size());
            writeNullRecord(globalStream);

            // P: UpAxisSign
            globalStream << pUpAxisSignEnd;
            globalStream << pUpAxisSignProp.propertyCount;
            globalStream << static_cast<quint32>(pUpAxisSignProp.data.size());
            globalStream << static_cast<quint8>(1);
            globalStream.writeRawData("P", 1);
            globalStream.writeRawData(pUpAxisSignProp.data.constData(), pUpAxisSignProp.data.size());
            writeNullRecord(globalStream);

            // P: FrontAxis
            globalStream << pFrontAxisEnd;
            globalStream << pFrontAxisProp.propertyCount;
            globalStream << static_cast<quint32>(pFrontAxisProp.data.size());
            globalStream << static_cast<quint8>(1);
            globalStream.writeRawData("P", 1);
            globalStream.writeRawData(pFrontAxisProp.data.constData(), pFrontAxisProp.data.size());
            writeNullRecord(globalStream);

            // P: FrontAxisSign
            globalStream << pFrontAxisSignEnd;
            globalStream << pFrontAxisSignProp.propertyCount;
            globalStream << static_cast<quint32>(pFrontAxisSignProp.data.size());
            globalStream << static_cast<quint8>(1);
            globalStream.writeRawData("P", 1);
            globalStream.writeRawData(pFrontAxisSignProp.data.constData(), pFrontAxisSignProp.data.size());
            writeNullRecord(globalStream);

            // P: CoordAxis
            globalStream << pCoordAxisEnd;
            globalStream << pCoordAxisProp.propertyCount;
            globalStream << static_cast<quint32>(pCoordAxisProp.data.size());
            globalStream << static_cast<quint8>(1);
            globalStream.writeRawData("P", 1);
            globalStream.writeRawData(pCoordAxisProp.data.constData(), pCoordAxisProp.data.size());
            writeNullRecord(globalStream);

            // P: CoordAxisSign
            globalStream << pCoordAxisSignEnd;
            globalStream << pCoordAxisSignProp.propertyCount;
            globalStream << static_cast<quint32>(pCoordAxisSignProp.data.size());
            globalStream << static_cast<quint8>(1);
            globalStream.writeRawData("P", 1);
            globalStream.writeRawData(pCoordAxisSignProp.data.constData(), pCoordAxisSignProp.data.size());
            writeNullRecord(globalStream);

            // P: UnitScaleFactor
            globalStream << pUnitScaleFactorEnd;
            globalStream << pUnitScaleFactorProp.propertyCount;
            globalStream << static_cast<quint32>(pUnitScaleFactorProp.data.size());
            globalStream << static_cast<quint8>(1);
            globalStream.writeRawData("P", 1);
            globalStream.writeRawData(pUnitScaleFactorProp.data.constData(), pUnitScaleFactorProp.data.size());
            writeNullRecord(globalStream);

            // Terminate Properties70 children
            writeNullRecord(globalStream);

            childBufPos += props70HeaderSize
                           + pUpAxisNodeSize
                           + pUpAxisSignNodeSize
                           + pFrontAxisNodeSize
                           + pFrontAxisSignNodeSize
                           + pCoordAxisNodeSize
                           + pCoordAxisSignNodeSize
                           + pUnitScaleFactorNodeSize
                           + 13;

            // Terminate GlobalSettings children
            writeNullRecord(globalStream);
        }

        quint32 globalNodeEnd = globalNodeStart + globalNodeHeaderSize + globalChildren.size();

        outFile << globalNodeEnd;
        outFile << static_cast<quint32>(0);
        outFile << static_cast<quint32>(0);
        outFile << static_cast<quint8>(14);
        outFile.writeRawData("GlobalSettings", 14);
        outFile.writeRawData(globalChildren.constData(), globalChildren.size());
    }

    // === Documents node (TOP-LEVEL SIBLING 2) ===
    {
        quint32 documentsNodeStart = getAbsolutePos();
        quint32 documentsHeaderSize = 4 + 4 + 4 + 1 + 9;

        // Build Document child node
        QByteArray documentsBuffer;
        {
            QDataStream docStream(&documentsBuffer, QIODevice::WriteOnly);
            docStream.setByteOrder(QDataStream::LittleEndian);

            // Build Document properties: id, name, SourceObject, ActiveAnimStackName
            QByteArray docProps;
            QDataStream docPropStream(&docProps, QIODevice::WriteOnly);
            docPropStream.setByteOrder(QDataStream::LittleEndian);
            encodeInt64Property(docPropStream, 1);  // Document ID
            encodeStringProperty(docPropStream, QByteArray("Scene"));  // Name
            encodeInt64Property(docPropStream, 0);  // SourceObject (root)
            encodeStringProperty(docPropStream, QByteArray(""));  // ActiveAnimStackName

            quint32 docHeaderSize = 4 + 4 + 4 + 1 + 8;  // "Document"
            quint32 docPropsSize = docProps.size();
            quint32 docAbsStart = documentsNodeStart + documentsHeaderSize;
            quint32 docAbsEnd = docAbsStart + docHeaderSize + docPropsSize + 13;

            docStream << docAbsEnd;
            docStream << static_cast<quint32>(4);  // 4 properties
            docStream << docPropsSize;
            docStream << static_cast<quint8>(8);
            docStream.writeRawData("Document", 8);
            docStream.writeRawData(docProps.constData(), docProps.size());
            writeNullRecord(docStream); // terminate Document children
            writeNullRecord(docStream); // terminate Documents children
        }

        quint32 documentsEndOffset = documentsNodeStart + documentsHeaderSize + documentsBuffer.size();
        outFile << documentsEndOffset;
        outFile << static_cast<quint32>(0);
        outFile << static_cast<quint32>(0);
        outFile << static_cast<quint8>(9);
        outFile.writeRawData("Documents", 9);
        outFile.writeRawData(documentsBuffer.constData(), documentsBuffer.size());
    }

    // Count valid objects before writing Definitions
    quint32 validObjectCount = 0;
    for (const SVObject *obj : visibleObjects)
    {
        if (obj && obj->localMesh.vertexCount() > 0)
        {
            const QList<float> &vertices = obj->localMesh.vertices;
            const QList<int> &triangles = obj->localMesh.triangles;

            if (!vertices.isEmpty() && !triangles.isEmpty())
                validObjectCount++;
        }
    }

    // === Definitions node (TOP-LEVEL SIBLING 3) ===
    {
        quint32 definitionsNodeStart = getAbsolutePos();
        quint32 definitionsHeaderSize = 4 + 4 + 4 + 1 + 11;

        // Build ObjectType child nodes for Geometry and Model
        QByteArray definitionsBuffer;
        {
            QDataStream defStream(&definitionsBuffer, QIODevice::WriteOnly);
            defStream.setByteOrder(QDataStream::LittleEndian);

            quint32 defBufPos = 0;

            // === ObjectType: Geometry ===
            {
                QByteArray geomTypeProps;
                QDataStream geomTypePropStream(&geomTypeProps, QIODevice::WriteOnly);
                geomTypePropStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(geomTypePropStream, QByteArray("Geometry"));  // Class name

                QByteArray geomTypeCountProp;
                QDataStream geomTypeCountStream(&geomTypeCountProp, QIODevice::WriteOnly);
                geomTypeCountStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(geomTypeCountStream, static_cast<qint32>(validObjectCount));

                quint32 geomTypeHeaderSize = 4 + 4 + 4 + 1 + 10;  // "ObjectType"
                quint32 geomTypePropsSize = geomTypeProps.size();
                quint32 geomTypeCountHeaderSize = 4 + 4 + 4 + 1 + 5;  // "Count"
                quint32 geomTypeCountNodeSize = geomTypeCountHeaderSize + geomTypeCountProp.size() + 13;

                quint32 geomTypeAbsStart = definitionsNodeStart + definitionsHeaderSize + defBufPos;
                quint32 geomTypeCountAbsStart = geomTypeAbsStart + geomTypeHeaderSize + geomTypePropsSize;
                quint32 geomTypeCountAbsEnd = geomTypeCountAbsStart + geomTypeCountHeaderSize + geomTypeCountProp.size() + 13;
                quint32 geomTypeAbsEnd = geomTypeCountAbsStart + geomTypeCountNodeSize + 13;

                defStream << geomTypeAbsEnd;
                defStream << static_cast<quint32>(1);  // 1 property (class name)
                defStream << geomTypePropsSize;
                defStream << static_cast<quint8>(10);
                defStream.writeRawData("ObjectType", 10);
                defStream.writeRawData(geomTypeProps.constData(), geomTypeProps.size());

                // Write Count child node
                defStream << geomTypeCountAbsEnd;
                defStream << static_cast<quint32>(1);  // 1 property
                defStream << static_cast<quint32>(geomTypeCountProp.size());
                defStream << static_cast<quint8>(5);
                defStream.writeRawData("Count", 5);
                defStream.writeRawData(geomTypeCountProp.constData(), geomTypeCountProp.size());
                writeNullRecord(defStream);

                writeNullRecord(defStream);  // Terminate ObjectType children

                defBufPos += geomTypeHeaderSize + geomTypePropsSize + geomTypeCountNodeSize + 13;
            }

            // === ObjectType: Model ===
            {
                QByteArray modelTypeProps;
                QDataStream modelTypePropStream(&modelTypeProps, QIODevice::WriteOnly);
                modelTypePropStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(modelTypePropStream, QByteArray("Model"));  // Class name

                QByteArray modelTypeCountProp;
                QDataStream modelTypeCountStream(&modelTypeCountProp, QIODevice::WriteOnly);
                modelTypeCountStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(modelTypeCountStream, static_cast<qint32>(validObjectCount));

                quint32 modelTypeHeaderSize = 4 + 4 + 4 + 1 + 10;  // "ObjectType"
                quint32 modelTypePropsSize = modelTypeProps.size();
                quint32 modelTypeCountHeaderSize = 4 + 4 + 4 + 1 + 5;  // "Count"
                quint32 modelTypeCountNodeSize = modelTypeCountHeaderSize + modelTypeCountProp.size() + 13;

                quint32 modelTypeAbsStart = definitionsNodeStart + definitionsHeaderSize + defBufPos;
                quint32 modelTypeCountAbsStart = modelTypeAbsStart + modelTypeHeaderSize + modelTypePropsSize;
                quint32 modelTypeCountAbsEnd = modelTypeCountAbsStart + modelTypeCountHeaderSize + modelTypeCountProp.size() + 13;
                quint32 modelTypeAbsEnd = modelTypeCountAbsStart + modelTypeCountNodeSize + 13;

                defStream << modelTypeAbsEnd;
                defStream << static_cast<quint32>(1);  // 1 property (class name)
                defStream << modelTypePropsSize;
                defStream << static_cast<quint8>(10);
                defStream.writeRawData("ObjectType", 10);
                defStream.writeRawData(modelTypeProps.constData(), modelTypeProps.size());

                // Write Count child node
                defStream << modelTypeCountAbsEnd;
                defStream << static_cast<quint32>(1);  // 1 property
                defStream << static_cast<quint32>(modelTypeCountProp.size());
                defStream << static_cast<quint8>(5);
                defStream.writeRawData("Count", 5);
                defStream.writeRawData(modelTypeCountProp.constData(), modelTypeCountProp.size());
                writeNullRecord(defStream);

                writeNullRecord(defStream);  // Terminate ObjectType children

                defBufPos += modelTypeHeaderSize + modelTypePropsSize + modelTypeCountNodeSize + 13;
            }

            // Terminate Definitions children
            writeNullRecord(defStream);
        }

        quint32 definitionsEndOffset = definitionsNodeStart + definitionsHeaderSize + definitionsBuffer.size();
        outFile << definitionsEndOffset;
        outFile << static_cast<quint32>(0);
        outFile << static_cast<quint32>(0);
        outFile << static_cast<quint8>(11);
        outFile.writeRawData("Definitions", 11);
        outFile.writeRawData(definitionsBuffer.constData(), definitionsBuffer.size());
    }

    // === Objects node (TOP-LEVEL SIBLING 4) - contains geometry and models as children ===
    QList<qint64> modelIds; // Track model object IDs for connections (needed for Connections section later)
    QList<qint64> geometryIds;  // Track geometry object IDs for connections (needed for Connections section later)
    QList<QByteArray> exportedObjectNames;
    QList<FBXColor> materialColors;
    QList<qint64> materialIds;

    {
        quint32 objectsNodeStart = getAbsolutePos();
        quint32 objectsHeaderSize = 4 + 4 + 4 + 1 + 7;

        // Build all geometry and model child nodes in a buffer first
        // This lets us know the total child size before writing Objects node
        QByteArray childrenBuffer;
        {
            QDataStream childStream(&childrenBuffer, QIODevice::WriteOnly);
            childStream.setByteOrder(QDataStream::LittleEndian);

            quint32 bufferPos = 0;  // Track position within child buffer
            qint64 objectId = 10000;

            for (const SVObject *obj : visibleObjects)
            {
                if (!obj || obj->localMesh.vertexCount() == 0)
                    continue;

                QList<float> vertices = obj->localMesh.vertices;
                QList<int> triangles = obj->localMesh.triangles;
                if (vertices.isEmpty() || triangles.isEmpty())
                    continue;

                qint64 geometryId = objectId;  // Save for later connection
                geometryIds.append(geometryId);

                QList<float> transformedVertices = applyMatrixToVertices(vertices, obj->matrix);
                QList<float> normals = computeVertexNormals(transformedVertices, triangles);

                // Normals as doubles
                QList<double> normalsDouble;
                for (float n : std::as_const(normals))
                    normalsDouble.append(static_cast<double>(n));

                // Name
                QString baseName = obj->Name;
                if (baseName.isEmpty())
                    baseName = QStringLiteral("Object_%1").arg(geometryId);

                // Colour
                FBXColor col;
                col.r = static_cast<double>(obj->Colour[0]) / 255.0;
                col.g = static_cast<double>(obj->Colour[1]) / 255.0;
                col.b = static_cast<double>(obj->Colour[2]) / 255.0;
                materialColors.append(col);

                QByteArray safeName = baseName.toUtf8();
                exportedObjectNames.append(safeName);

                // Track position within buffer being written
                quint32 positionInBuffer = 0;
                quint32 geomHeaderSize = 4 + 4 + 4 + 1 + 8;  // endOffset + numProps + propListLen + nameLen + "Geometry"

                // Build Geometry properties: id, name (Geometry::ObjectN), subtype (Mesh)
                QByteArray geomProps;
                QDataStream geomPropStream(&geomProps, QIODevice::WriteOnly);
                geomPropStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt64Property(geomPropStream, objectId);  // Object ID

                // Blender expects FBX object-style nodes like:
                // Geometry: [id, "Name\0\1Geometry", "Mesh"]
                QByteArray geomNameClass = safeName + QByteArray("\x00\x01", 2) + QByteArray("Geometry");
                encodeStringProperty(geomPropStream, geomNameClass);

                encodeStringProperty(geomPropStream, QByteArray("Mesh"));  // Subtype
                objectId++;

                quint32 geomPropsSize = geomProps.size();
                positionInBuffer += geomHeaderSize + geomPropsSize;

                // Build GeometryVersion child node
                QByteArray geomVersionProp;
                QDataStream geomVersionStream(&geomVersionProp, QIODevice::WriteOnly);
                geomVersionStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(geomVersionStream, 124);

                quint32 geomVersionHeaderSize = 4 + 4 + 4 + 1 + 15;  // "GeometryVersion"
                quint32 geomVersionNodeSize = geomVersionHeaderSize + geomVersionProp.size() + 13;
                //quint32 geomVersionPositionInBuffer = positionInBuffer;
                positionInBuffer += geomVersionNodeSize;

                // Build Vertices child node
                QList<double> verticesDouble;
                for (float v : std::as_const(transformedVertices))
                    verticesDouble.append(static_cast<double>(v));

                QByteArray verticesProp;
                QDataStream verticesPropStream(&verticesProp, QIODevice::WriteOnly);
                verticesPropStream.setByteOrder(QDataStream::LittleEndian);
                encodeDoubleArrayProperty(verticesPropStream, verticesDouble);

                quint32 verticesHeaderSize = 4 + 4 + 4 + 1 + 8;  // "Vertices"
                quint32 verticesNodeSize = verticesHeaderSize + verticesProp.size() + 13;
                //quint32 verticesPositionInBuffer = positionInBuffer;
                positionInBuffer += verticesNodeSize;

                // Build PolygonVertexIndex child node
                QList<qint32> indexArray;
                for (int i = 0; i < triangles.size(); i += 3)
                {
                    // To flip the faces outward, swap the winding order from
                    // (0, 1, 2) to (0, 2, 1)
                    indexArray.append(triangles[i]);
                    indexArray.append(triangles[i + 2]);
                    indexArray.append(-(triangles[i + 1] + 1));  // FBX polygon terminator
                }

                QByteArray indicesProp;
                QDataStream indicesPropStream(&indicesProp, QIODevice::WriteOnly);
                indicesPropStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32ArrayProperty(indicesPropStream, indexArray);

                quint32 indicesHeaderSize = 4 + 4 + 4 + 1 + 18;  // "PolygonVertexIndex"
                quint32 indicesNodeSize = indicesHeaderSize + indicesProp.size() + 13;
                //quint32 indicesPositionInBuffer = positionInBuffer;
                positionInBuffer += indicesNodeSize;

                // Build LayerElementNormal child node
                QByteArray normalsDirectProp;
                QDataStream normalsDirectStream(&normalsDirectProp, QIODevice::WriteOnly);
                normalsDirectStream.setByteOrder(QDataStream::LittleEndian);
                encodeDoubleArrayProperty(normalsDirectStream, normalsDouble);

                // Child: Version
                QByteArray lenVersionProp;
                QDataStream lenVersionStream(&lenVersionProp, QIODevice::WriteOnly);
                lenVersionStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(lenVersionStream, 101);

                // Child: Name
                QByteArray lenNameProp;
                QDataStream lenNameStream(&lenNameProp, QIODevice::WriteOnly);
                lenNameStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(lenNameStream, QByteArray(""));

                // Child: MappingInformationType
                QByteArray lenMappingProp;
                QDataStream lenMappingStream(&lenMappingProp, QIODevice::WriteOnly);
                lenMappingStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(lenMappingStream, QByteArray("ByVertice"));

                // Child: ReferenceInformationType
                QByteArray lenRefProp;
                QDataStream lenRefStream(&lenRefProp, QIODevice::WriteOnly);
                lenRefStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(lenRefStream, QByteArray("Direct"));

                quint32 lenHeaderSize = 4 + 4 + 4 + 1 + 18; // "LayerElementNormal"

                quint32 lenVersionHeaderSize = 4 + 4 + 4 + 1 + 7;   // "Version"
                quint32 lenNameHeaderSize    = 4 + 4 + 4 + 1 + 4;   // "Name"
                quint32 lenMappingHeaderSize = 4 + 4 + 4 + 1 + 22;  // "MappingInformationType"
                quint32 lenRefHeaderSize     = 4 + 4 + 4 + 1 + 24;  // "ReferenceInformationType"
                quint32 lenNormalsHeaderSize = 4 + 4 + 4 + 1 + 7;   // "Normals"

                quint32 lenVersionNodeSize = lenVersionHeaderSize + lenVersionProp.size() + 13;
                quint32 lenNameNodeSize    = lenNameHeaderSize    + lenNameProp.size()    + 13;
                quint32 lenMappingNodeSize = lenMappingHeaderSize + lenMappingProp.size() + 13;
                quint32 lenRefNodeSize     = lenRefHeaderSize     + lenRefProp.size()     + 13;
                quint32 lenNormalsNodeSize = lenNormalsHeaderSize + normalsDirectProp.size() + 13;

                // Extra child props
                QByteArray layerProps;
                QDataStream layerPropsStream(&layerProps, QIODevice::WriteOnly);
                layerPropsStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(layerPropsStream, 0); // Layer index

                QByteArray lenProps;
                QDataStream lenPropsStream(&lenProps, QIODevice::WriteOnly);
                lenPropsStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(lenPropsStream, 0); // Layer element index

                // Build Layer child node
                //quint32 layerElementNormalPositionInBuffer = positionInBuffer;
                positionInBuffer += lenHeaderSize
                                    + lenProps.size()
                                    + lenVersionNodeSize
                                    + lenNameNodeSize
                                    + lenMappingNodeSize
                                    + lenRefNodeSize
                                    + lenNormalsNodeSize
                                    + 13; // LayerElementNormal child-list terminator

                QByteArray layerVersionProp;
                QDataStream layerVersionStream(&layerVersionProp, QIODevice::WriteOnly);
                layerVersionStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(layerVersionStream, 100);

                QByteArray layerElemTypeProp;
                QDataStream layerElemTypeStream(&layerElemTypeProp, QIODevice::WriteOnly);
                layerElemTypeStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(layerElemTypeStream, QByteArray("LayerElementNormal"));

                QByteArray layerElemTypedIndexProp;
                QDataStream layerElemTypedIndexStream(&layerElemTypedIndexProp, QIODevice::WriteOnly);
                layerElemTypedIndexStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(layerElemTypedIndexStream, 0);

                quint32 layerHeaderSize = 4 + 4 + 4 + 1 + 5; // "Layer"
                quint32 layerVersionHeaderSize       = 4 + 4 + 4 + 1 + 7;  // "Version"
                quint32 layerElemTypeHeaderSize      = 4 + 4 + 4 + 1 + 12; // "LayerElement"
                quint32 layerElemTypedIndexHeaderSize= 4 + 4 + 4 + 1 + 10; // "TypedIndex"

                quint32 layerVersionNodeSize        = layerVersionHeaderSize + layerVersionProp.size() + 13;
                quint32 layerElemTypeNodeSize       = layerElemTypeHeaderSize + layerElemTypeProp.size() + 13;
                quint32 layerElemTypedIndexNodeSize = layerElemTypedIndexHeaderSize + layerElemTypedIndexProp.size() + 13;

                //quint32 layerPositionInBuffer = positionInBuffer;
                positionInBuffer += layerHeaderSize
                                    + layerProps.size()
                                    + layerVersionNodeSize
                                    + layerElemTypeNodeSize
                                    + layerElemTypedIndexNodeSize
                                    + 13; // Layer child-list terminator

                // Calculate absolute file offsets using absolute Objects start position
                quint32 geomNodeStart = objectsNodeStart + objectsHeaderSize + bufferPos;
                quint32 geomVersionAbsStart = geomNodeStart + geomHeaderSize + geomPropsSize;
                quint32 geomVersionAbsEnd = geomVersionAbsStart + geomVersionHeaderSize + geomVersionProp.size() + 13;
                quint32 verticesAbsStart = geomVersionAbsStart + geomVersionNodeSize;
                quint32 indicesAbsStart = verticesAbsStart + verticesNodeSize;
                quint32 layerElementNormalAbsStart = indicesAbsStart + indicesNodeSize;
                quint32 layerAbsStart = layerElementNormalAbsStart
                                        + lenHeaderSize
                                        + lenProps.size()
                                        + lenVersionNodeSize
                                        + lenNameNodeSize
                                        + lenMappingNodeSize
                                        + lenRefNodeSize
                                        + lenNormalsNodeSize
                                        + 13;
                quint32 geomAbsEnd = layerAbsStart
                                     + layerHeaderSize
                                     + layerProps.size()
                                     + layerVersionNodeSize
                                     + layerElemTypeNodeSize
                                     + layerElemTypedIndexNodeSize
                                     + 13
                                     + 13; // Geometry child-list terminator
                quint32 verticesAbsEnd = verticesAbsStart + verticesNodeSize;
                quint32 indicesAbsEnd = indicesAbsStart + indicesNodeSize;

                // Write Geometry node header (with absolute file offset for end)
                childStream << geomAbsEnd;
                childStream << static_cast<quint32>(3);  // 3 properties (id, name, type)
                childStream << geomPropsSize;
                childStream << static_cast<quint8>(8);
                childStream.writeRawData("Geometry", 8);
                childStream.writeRawData(geomProps.constData(), geomProps.size());

                // Write GeometryVersion child node with absolute file offset for end
                childStream << geomVersionAbsEnd;
                childStream << static_cast<quint32>(1);  // 1 property
                childStream << static_cast<quint32>(geomVersionProp.size());
                childStream << static_cast<quint8>(15);
                childStream.writeRawData("GeometryVersion", 15);
                childStream.writeRawData(geomVersionProp.constData(), geomVersionProp.size());
                writeNullRecord(childStream);

                // Write Vertices child node with absolute file offset for end
                childStream << verticesAbsEnd;
                childStream << static_cast<quint32>(1);  // 1 property
                childStream << static_cast<quint32>(verticesProp.size());
                childStream << static_cast<quint8>(8);
                childStream.writeRawData("Vertices", 8);
                childStream.writeRawData(verticesProp.constData(), verticesProp.size());
                writeNullRecord(childStream);

                // Write PolygonVertexIndex child node with absolute file offset for end
                childStream << indicesAbsEnd;
                childStream << static_cast<quint32>(1);  // 1 property
                childStream << static_cast<quint32>(indicesProp.size());
                childStream << static_cast<quint8>(18);
                childStream.writeRawData("PolygonVertexIndex", 18);
                childStream.writeRawData(indicesProp.constData(), indicesProp.size());
                writeNullRecord(childStream);

                // Write LayerElementNormal node
                quint32 lenAbsStart = layerElementNormalAbsStart;
                quint32 lenVersionAbsStart = lenAbsStart + lenHeaderSize + lenProps.size();
                quint32 lenVersionAbsEnd = lenVersionAbsStart + lenVersionNodeSize;
                quint32 lenNameAbsStart = lenVersionAbsEnd;
                quint32 lenNameAbsEnd = lenNameAbsStart + lenNameNodeSize;
                quint32 lenMappingAbsStart = lenNameAbsEnd;
                quint32 lenMappingAbsEnd = lenMappingAbsStart + lenMappingNodeSize;
                quint32 lenRefAbsStart = lenMappingAbsEnd;
                quint32 lenRefAbsEnd = lenRefAbsStart + lenRefNodeSize;
                quint32 lenNormalsAbsStart = lenRefAbsEnd;
                quint32 lenNormalsAbsEnd = lenNormalsAbsStart + lenNormalsNodeSize;
                quint32 lenAbsEnd = lenNormalsAbsEnd + 13;

                childStream << lenAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(lenProps.size());
                childStream << static_cast<quint8>(18);
                childStream.writeRawData("LayerElementNormal", 18);
                childStream.writeRawData(lenProps.constData(), lenProps.size());

                // Version
                childStream << lenVersionAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(lenVersionProp.size());
                childStream << static_cast<quint8>(7);
                childStream.writeRawData("Version", 7);
                childStream.writeRawData(lenVersionProp.constData(), lenVersionProp.size());
                writeNullRecord(childStream);

                // Name
                childStream << lenNameAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(lenNameProp.size());
                childStream << static_cast<quint8>(4);
                childStream.writeRawData("Name", 4);
                childStream.writeRawData(lenNameProp.constData(), lenNameProp.size());
                writeNullRecord(childStream);

                // MappingInformationType
                childStream << lenMappingAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(lenMappingProp.size());
                childStream << static_cast<quint8>(22);
                childStream.writeRawData("MappingInformationType", 22);
                childStream.writeRawData(lenMappingProp.constData(), lenMappingProp.size());
                writeNullRecord(childStream);

                // ReferenceInformationType
                childStream << lenRefAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(lenRefProp.size());
                childStream << static_cast<quint8>(24);
                childStream.writeRawData("ReferenceInformationType", 24);
                childStream.writeRawData(lenRefProp.constData(), lenRefProp.size());
                writeNullRecord(childStream);

                // Normals
                childStream << lenNormalsAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(normalsDirectProp.size());
                childStream << static_cast<quint8>(7);
                childStream.writeRawData("Normals", 7);
                childStream.writeRawData(normalsDirectProp.constData(), normalsDirectProp.size());
                writeNullRecord(childStream);

                // Terminate LayerElementNormal children
                writeNullRecord(childStream);

                quint32 layerVersionAbsStart = layerAbsStart + layerHeaderSize + layerProps.size();
                quint32 layerVersionAbsEnd = layerVersionAbsStart + layerVersionNodeSize;
                quint32 layerElemTypeAbsStart = layerVersionAbsEnd;
                quint32 layerElemTypeAbsEnd = layerElemTypeAbsStart + layerElemTypeNodeSize;
                quint32 layerElemTypedIndexAbsStart = layerElemTypeAbsEnd;
                quint32 layerElemTypedIndexAbsEnd = layerElemTypedIndexAbsStart + layerElemTypedIndexNodeSize;
                quint32 layerAbsEnd = layerElemTypedIndexAbsEnd + 13;

                childStream << layerAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(layerProps.size());
                childStream << static_cast<quint8>(5);
                childStream.writeRawData("Layer", 5);
                childStream.writeRawData(layerProps.constData(), layerProps.size());

                // Version
                childStream << layerVersionAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(layerVersionProp.size());
                childStream << static_cast<quint8>(7);
                childStream.writeRawData("Version", 7);
                childStream.writeRawData(layerVersionProp.constData(), layerVersionProp.size());
                writeNullRecord(childStream);

                // LayerElement
                childStream << layerElemTypeAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(layerElemTypeProp.size());
                childStream << static_cast<quint8>(12);
                childStream.writeRawData("LayerElement", 12);
                childStream.writeRawData(layerElemTypeProp.constData(), layerElemTypeProp.size());
                writeNullRecord(childStream);

                // TypedIndex
                childStream << layerElemTypedIndexAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(layerElemTypedIndexProp.size());
                childStream << static_cast<quint8>(10);
                childStream.writeRawData("TypedIndex", 10);
                childStream.writeRawData(layerElemTypedIndexProp.constData(), layerElemTypedIndexProp.size());
                writeNullRecord(childStream);

                // Terminate Layer children
                writeNullRecord(childStream);

                // Terminate Geometry node's child list
                writeNullRecord(childStream);

                bufferPos += positionInBuffer + 13;
            }

            // === Create Model nodes for each Geometry ===
            for (int i = 0; i < geometryIds.size(); ++i)
            {
                quint32 positionInBuffer = 0;
                quint32 modelHeaderSize = 4 + 4 + 4 + 1 + 5;  // "Model"

                // Build Model properties: id, name (Model::ObjectN), subtype (Mesh)
                qint64 modelObjectId = objectId;
                modelIds.append(modelObjectId);
                QByteArray modelProps;
                QDataStream modelPropStream(&modelProps, QIODevice::WriteOnly);
                modelPropStream.setByteOrder(QDataStream::LittleEndian);

                encodeInt64Property(modelPropStream, modelObjectId);  // Model ID

                // Blender expects FBX object-style nodes like:
                // Model: [id, "Name\0\1Model", "Mesh"]
                QByteArray modelNameClass = exportedObjectNames[i] + QByteArray("\x00\x01", 2) + QByteArray("Model");
                encodeStringProperty(modelPropStream, modelNameClass);

                encodeStringProperty(modelPropStream, QByteArray("Mesh"));  // Subtype

                // Increase obejctID
                objectId++;

                quint32 modelPropsSize = modelProps.size();

                // Build Model child nodes: Version, Shading, Culling
                QByteArray modelVersionProp;
                QDataStream modelVersionStream(&modelVersionProp, QIODevice::WriteOnly);
                modelVersionStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(modelVersionStream, 232);

                QByteArray modelShadingProp;
                QDataStream modelShadingStream(&modelShadingProp, QIODevice::WriteOnly);
                modelShadingStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(modelShadingStream, 1);

                QByteArray modelCullingProp;
                QDataStream modelCullingStream(&modelCullingProp, QIODevice::WriteOnly);
                modelCullingStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(modelCullingStream, QByteArray("CullingOff"));

                // Build Properties70 children: Lcl Translation, Lcl Rotation, Lcl Scaling
                QByteArray p70TranslationProp;
                QDataStream p70TranslationStream(&p70TranslationProp, QIODevice::WriteOnly);
                p70TranslationStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(p70TranslationStream, QByteArray("Lcl Translation"));
                encodeStringProperty(p70TranslationStream, QByteArray("Lcl Translation"));
                encodeStringProperty(p70TranslationStream, QByteArray(""));
                encodeStringProperty(p70TranslationStream, QByteArray("A"));
                encodeDoubleProperty(p70TranslationStream, 0.0);
                encodeDoubleProperty(p70TranslationStream, 0.0);
                encodeDoubleProperty(p70TranslationStream, 0.0);

                QByteArray p70RotationProp;
                QDataStream p70RotationStream(&p70RotationProp, QIODevice::WriteOnly);
                p70RotationStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(p70RotationStream, QByteArray("Lcl Rotation"));
                encodeStringProperty(p70RotationStream, QByteArray("Lcl Rotation"));
                encodeStringProperty(p70RotationStream, QByteArray(""));
                encodeStringProperty(p70RotationStream, QByteArray("A"));
                encodeDoubleProperty(p70RotationStream, 0.0);
                encodeDoubleProperty(p70RotationStream, 0.0);
                encodeDoubleProperty(p70RotationStream, 0.0);

                QByteArray p70ScalingProp;
                QDataStream p70ScalingStream(&p70ScalingProp, QIODevice::WriteOnly);
                p70ScalingStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(p70ScalingStream, QByteArray("Lcl Scaling"));
                encodeStringProperty(p70ScalingStream, QByteArray("Lcl Scaling"));
                encodeStringProperty(p70ScalingStream, QByteArray(""));
                encodeStringProperty(p70ScalingStream, QByteArray("A"));
                encodeDoubleProperty(p70ScalingStream, 1.0);
                encodeDoubleProperty(p70ScalingStream, 1.0);
                encodeDoubleProperty(p70ScalingStream, 1.0);

                quint32 props70HeaderSize = 4 + 4 + 4 + 1 + 12; // "Properties70"
                quint32 pHeaderSize = 4 + 4 + 4 + 1 + 1;        // "P"

                quint32 p70TranslationNodeSize = pHeaderSize + p70TranslationProp.size() + 13;
                quint32 p70RotationNodeSize    = pHeaderSize + p70RotationProp.size() + 13;
                quint32 p70ScalingNodeSize     = pHeaderSize + p70ScalingProp.size() + 13;

                quint32 props70NodeSize = props70HeaderSize
                                          + p70TranslationNodeSize
                                          + p70RotationNodeSize
                                          + p70ScalingNodeSize
                                          + 13;

                quint32 modelVersionHeaderSize = 4 + 4 + 4 + 1 + 7;  // "Version"
                quint32 modelVersionNodeSize = modelVersionHeaderSize + modelVersionProp.size() + 13;
                quint32 modelShadingHeaderSize = 4 + 4 + 4 + 1 + 7;  // "Shading"
                quint32 modelShadingNodeSize = modelShadingHeaderSize + modelShadingProp.size() + 13;
                quint32 modelCullingHeaderSize = 4 + 4 + 4 + 1 + 7;  // "Culling"
                quint32 modelCullingNodeSize = modelCullingHeaderSize + modelCullingProp.size() + 13;

                positionInBuffer = modelHeaderSize
                                   + modelPropsSize
                                   + modelVersionNodeSize
                                   + modelShadingNodeSize
                                   + modelCullingNodeSize
                                   + props70NodeSize
                                   + 13; // Model child-list terminator

                // Calculate absolute file offsets for Model node
                quint32 modelNodeStart = objectsNodeStart + objectsHeaderSize + bufferPos;

                quint32 modelVersionAbsStart = modelNodeStart + modelHeaderSize + modelPropsSize;
                quint32 modelVersionAbsEnd = modelVersionAbsStart + modelVersionNodeSize;
                quint32 modelShadingAbsStart = modelVersionAbsEnd;
                quint32 modelShadingAbsEnd = modelShadingAbsStart + modelShadingNodeSize;
                quint32 modelCullingAbsStart = modelShadingAbsEnd;
                quint32 modelCullingAbsEnd = modelCullingAbsStart + modelCullingNodeSize;
                quint32 props70AbsStart = modelCullingAbsEnd;
                quint32 props70AbsEnd = props70AbsStart + props70NodeSize;
                quint32 modelAbsEnd = props70AbsEnd + 13;

                // Write Model node header
                childStream << modelAbsEnd;
                childStream << static_cast<quint32>(3);  // 3 properties (id, name, type)
                childStream << modelPropsSize;
                childStream << static_cast<quint8>(5);
                childStream.writeRawData("Model", 5);
                childStream.writeRawData(modelProps.constData(), modelProps.size());

                // Write Version child node
                childStream << modelVersionAbsEnd;
                childStream << static_cast<quint32>(1);  // 1 property
                childStream << static_cast<quint32>(modelVersionProp.size());
                childStream << static_cast<quint8>(7);
                childStream.writeRawData("Version", 7);
                childStream.writeRawData(modelVersionProp.constData(), modelVersionProp.size());
                writeNullRecord(childStream);

                // Write Shading child node
                childStream << modelShadingAbsEnd;
                childStream << static_cast<quint32>(1);  // 1 property
                childStream << static_cast<quint32>(modelShadingProp.size());
                childStream << static_cast<quint8>(7);
                childStream.writeRawData("Shading", 7);
                childStream.writeRawData(modelShadingProp.constData(), modelShadingProp.size());
                writeNullRecord(childStream);

                // Write Culling child node
                childStream << modelCullingAbsEnd;
                childStream << static_cast<quint32>(1);  // 1 property
                childStream << static_cast<quint32>(modelCullingProp.size());
                childStream << static_cast<quint8>(7);
                childStream.writeRawData("Culling", 7);
                childStream.writeRawData(modelCullingProp.constData(), modelCullingProp.size());
                writeNullRecord(childStream);

                // Write Properties70
                quint32 p70TranslationAbsStart = props70AbsStart + props70HeaderSize;
                quint32 p70TranslationAbsEnd   = p70TranslationAbsStart + p70TranslationNodeSize;
                quint32 p70RotationAbsStart    = p70TranslationAbsEnd;
                quint32 p70RotationAbsEnd      = p70RotationAbsStart + p70RotationNodeSize;
                quint32 p70ScalingAbsStart     = p70RotationAbsEnd;
                quint32 p70ScalingAbsEnd       = p70ScalingAbsStart + p70ScalingNodeSize;

                childStream << props70AbsEnd;
                childStream << static_cast<quint32>(0);
                childStream << static_cast<quint32>(0);
                childStream << static_cast<quint8>(12);
                childStream.writeRawData("Properties70", 12);

                // P: Lcl Translation
                childStream << p70TranslationAbsEnd;
                childStream << static_cast<quint32>(7);
                childStream << static_cast<quint32>(p70TranslationProp.size());
                childStream << static_cast<quint8>(1);
                childStream.writeRawData("P", 1);
                childStream.writeRawData(p70TranslationProp.constData(), p70TranslationProp.size());
                writeNullRecord(childStream);

                // P: Lcl Rotation
                childStream << p70RotationAbsEnd;
                childStream << static_cast<quint32>(7);
                childStream << static_cast<quint32>(p70RotationProp.size());
                childStream << static_cast<quint8>(1);
                childStream.writeRawData("P", 1);
                childStream.writeRawData(p70RotationProp.constData(), p70RotationProp.size());
                writeNullRecord(childStream);

                // P: Lcl Scaling
                childStream << p70ScalingAbsEnd;
                childStream << static_cast<quint32>(7);
                childStream << static_cast<quint32>(p70ScalingProp.size());
                childStream << static_cast<quint8>(1);
                childStream.writeRawData("P", 1);
                childStream.writeRawData(p70ScalingProp.constData(), p70ScalingProp.size());
                writeNullRecord(childStream);

                // Terminate Properties70 children
                writeNullRecord(childStream);

                // Terminate Model node's child list
                writeNullRecord(childStream);

                bufferPos += positionInBuffer;
            }

            // === Create Materials nodes for each Geometry ===
            for (int i = 0; i < geometryIds.size(); ++i)
            {
                qint64 materialObjectId = objectId++;
                materialIds.append(materialObjectId);

                QByteArray materialProps;
                QDataStream materialPropStream(&materialProps, QIODevice::WriteOnly);
                materialPropStream.setByteOrder(QDataStream::LittleEndian);

                encodeInt64Property(materialPropStream, materialObjectId); // Material ID

                // Blender expects FBX object-style nodes like:
                // Material: [id, "Name\0\1Material", ""]
                QByteArray materialName = exportedObjectNames[i] + QByteArray("\x00\x01", 2) + QByteArray("Material");
                encodeStringProperty(materialPropStream, materialName);

                encodeStringProperty(materialPropStream, QByteArray("")); // Empty subtype

                FBXColor col = materialColors[i];

                QByteArray versionProp;
                QDataStream versionStream(&versionProp, QIODevice::WriteOnly);
                versionStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(versionStream, 102);

                QByteArray shadingModelProp;
                QDataStream shadingModelStream(&shadingModelProp, QIODevice::WriteOnly);
                shadingModelStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(shadingModelStream, QByteArray("phong"));

                QByteArray multiLayerProp;
                QDataStream multiLayerStream(&multiLayerProp, QIODevice::WriteOnly);
                multiLayerStream.setByteOrder(QDataStream::LittleEndian);
                encodeInt32Property(multiLayerStream, 0);

                QByteArray diffuseProp;
                QDataStream diffuseStream(&diffuseProp, QIODevice::WriteOnly);
                diffuseStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(diffuseStream, QByteArray("DiffuseColor"));
                encodeStringProperty(diffuseStream, QByteArray("Color"));
                encodeStringProperty(diffuseStream, QByteArray(""));
                encodeStringProperty(diffuseStream, QByteArray("A"));
                encodeDoubleProperty(diffuseStream, col.r);
                encodeDoubleProperty(diffuseStream, col.g);
                encodeDoubleProperty(diffuseStream, col.b);

                QByteArray ambientProp;
                QDataStream ambientStream(&ambientProp, QIODevice::WriteOnly);
                ambientStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(ambientStream, QByteArray("AmbientColor"));
                encodeStringProperty(ambientStream, QByteArray("Color"));
                encodeStringProperty(ambientStream, QByteArray(""));
                encodeStringProperty(ambientStream, QByteArray("A"));
                encodeDoubleProperty(ambientStream, col.r);
                encodeDoubleProperty(ambientStream, col.g);
                encodeDoubleProperty(ambientStream, col.b);

                quint32 materialHeaderSize = 4 + 4 + 4 + 1 + 8; // "Material"
                quint32 versionHeaderSize = 4 + 4 + 4 + 1 + 7;  // "Version"
                quint32 shadingModelHeaderSize = 4 + 4 + 4 + 1 + 12; // "ShadingModel"
                quint32 multiLayerHeaderSize = 4 + 4 + 4 + 1 + 10;   // "MultiLayer"
                quint32 props70HeaderSize = 4 + 4 + 4 + 1 + 12;      // "Properties70"
                quint32 pHeaderSize = 4 + 4 + 4 + 1 + 1;             // "P"

                quint32 versionNodeSize = versionHeaderSize + versionProp.size() + 13;
                quint32 shadingModelNodeSize = shadingModelHeaderSize + shadingModelProp.size() + 13;
                quint32 multiLayerNodeSize = multiLayerHeaderSize + multiLayerProp.size() + 13;
                quint32 diffuseNodeSize = pHeaderSize + diffuseProp.size() + 13;
                quint32 ambientNodeSize = pHeaderSize + ambientProp.size() + 13;
                quint32 props70NodeSize = props70HeaderSize + diffuseNodeSize + ambientNodeSize + 13;

                quint32 materialNodeStart = objectsNodeStart + objectsHeaderSize + bufferPos;
                quint32 materialPropsSize = materialProps.size();

                quint32 versionAbsStart = materialNodeStart + materialHeaderSize + materialPropsSize;
                quint32 versionAbsEnd = versionAbsStart + versionNodeSize;

                quint32 shadingModelAbsStart = versionAbsEnd;
                quint32 shadingModelAbsEnd = shadingModelAbsStart + shadingModelNodeSize;

                quint32 multiLayerAbsStart = shadingModelAbsEnd;
                quint32 multiLayerAbsEnd = multiLayerAbsStart + multiLayerNodeSize;

                quint32 props70AbsStart = multiLayerAbsEnd;
                quint32 pDiffuseAbsStart = props70AbsStart + props70HeaderSize;
                quint32 pDiffuseAbsEnd = pDiffuseAbsStart + diffuseNodeSize;
                quint32 pAmbientAbsStart = pDiffuseAbsEnd;
                quint32 pAmbientAbsEnd = pAmbientAbsStart + ambientNodeSize;
                quint32 props70AbsEnd = pAmbientAbsEnd + 13;

                quint32 materialAbsEnd = props70AbsEnd + 13;

                childStream << materialAbsEnd;
                childStream << static_cast<quint32>(3);
                childStream << static_cast<quint32>(materialProps.size());
                childStream << static_cast<quint8>(8);
                childStream.writeRawData("Material", 8);
                childStream.writeRawData(materialProps.constData(), materialProps.size());

                childStream << versionAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(versionProp.size());
                childStream << static_cast<quint8>(7);
                childStream.writeRawData("Version", 7);
                childStream.writeRawData(versionProp.constData(), versionProp.size());
                writeNullRecord(childStream);

                childStream << shadingModelAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(shadingModelProp.size());
                childStream << static_cast<quint8>(12);
                childStream.writeRawData("ShadingModel", 12);
                childStream.writeRawData(shadingModelProp.constData(), shadingModelProp.size());
                writeNullRecord(childStream);

                childStream << multiLayerAbsEnd;
                childStream << static_cast<quint32>(1);
                childStream << static_cast<quint32>(multiLayerProp.size());
                childStream << static_cast<quint8>(10);
                childStream.writeRawData("MultiLayer", 10);
                childStream.writeRawData(multiLayerProp.constData(), multiLayerProp.size());
                writeNullRecord(childStream);

                childStream << props70AbsEnd;
                childStream << static_cast<quint32>(0);
                childStream << static_cast<quint32>(0);
                childStream << static_cast<quint8>(12);
                childStream.writeRawData("Properties70", 12);

                childStream << pDiffuseAbsEnd;
                childStream << static_cast<quint32>(7);
                childStream << static_cast<quint32>(diffuseProp.size());
                childStream << static_cast<quint8>(1);
                childStream.writeRawData("P", 1);
                childStream.writeRawData(diffuseProp.constData(), diffuseProp.size());
                writeNullRecord(childStream);

                childStream << pAmbientAbsEnd;
                childStream << static_cast<quint32>(7);
                childStream << static_cast<quint32>(ambientProp.size());
                childStream << static_cast<quint8>(1);
                childStream.writeRawData("P", 1);
                childStream.writeRawData(ambientProp.constData(), ambientProp.size());
                writeNullRecord(childStream);

                writeNullRecord(childStream); // end Properties70
                writeNullRecord(childStream); // end Material

                bufferPos += materialHeaderSize + materialPropsSize
                             + versionNodeSize + shadingModelNodeSize + multiLayerNodeSize
                             + props70NodeSize + 13;
            }

            // Terminate Objects node's child list
            writeNullRecord(childStream);
        }

        // Write Objects node with absolute endOffset
        quint32 objectsEndOffset = objectsNodeStart + objectsHeaderSize + childrenBuffer.size();
        outFile << objectsEndOffset;
        outFile << static_cast<quint32>(0);
        outFile << static_cast<quint32>(0);
        outFile << static_cast<quint8>(7);
        outFile.writeRawData("Objects", 7);
        outFile.writeRawData(childrenBuffer.constData(), childrenBuffer.size());
    }

    // === Connections node (TOP-LEVEL SIBLING 5) ===
    {
        quint32 connectionsNodeStart = getAbsolutePos();
        quint32 connectionsHeaderSize = 4 + 4 + 4 + 1 + 11;

        // Build connection child nodes in buffer
        QByteArray connectionsBuffer;
        {
            QDataStream connectStream(&connectionsBuffer, QIODevice::WriteOnly);
            connectStream.setByteOrder(QDataStream::LittleEndian);

            quint32 connectBufPos = 0;

            // For each geometry, create:
            // 1. Geometry -> Model connection
            // 2. Model -> RootModel (0) connection
            // 3. Material -> Model connection
            for (int i = 0; i < geometryIds.size() && i < modelIds.size(); ++i)
            {
                qint64 geometryId = geometryIds[i];
                qint64 currentModelId = modelIds[i];

                // === Connection 1: Geometry -> Model ===
                QByteArray connGeomProps;
                QDataStream connGeomStream(&connGeomProps, QIODevice::WriteOnly);
                connGeomStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(connGeomStream, QByteArray("OO"));  // Object-to-Object
                encodeInt64Property(connGeomStream, geometryId);        // Source (Geometry)
                encodeInt64Property(connGeomStream, currentModelId);    // Target (Model)

                quint32 connGeomHeaderSize = 4 + 4 + 4 + 1 + 1;  // endOffset + numProps + propListLen + nameLen + "C"
                quint32 connGeomNodeStart = connectionsNodeStart + connectionsHeaderSize + connectBufPos;
                quint32 connGeomAbsEnd = connGeomNodeStart + connGeomHeaderSize + connGeomProps.size() + 13;

                connectStream << connGeomAbsEnd;
                connectStream << static_cast<quint32>(3);  // 3 properties
                connectStream << static_cast<quint32>(connGeomProps.size());
                connectStream << static_cast<quint8>(1);   // Name length = 1
                connectStream.writeRawData("C", 1);         // Node name "C" for Connection
                connectStream.writeRawData(connGeomProps.constData(), connGeomProps.size());
                writeNullRecord(connectStream);

                connectBufPos += connGeomHeaderSize + connGeomProps.size() + 13;

                // === Connection 2: Model -> RootModel ===
                QByteArray connModelProps;
                QDataStream connModelStream(&connModelProps, QIODevice::WriteOnly);
                connModelStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(connModelStream, QByteArray("OO"));  // Object-to-Object
                encodeInt64Property(connModelStream, currentModelId);     // Source (Model)
                encodeInt64Property(connModelStream, 0);                  // Target (RootModel)

                quint32 connModelHeaderSize = 4 + 4 + 4 + 1 + 1;  // endOffset + numProps + propListLen + nameLen + "C"
                quint32 connModelNodeStart = connectionsNodeStart + connectionsHeaderSize + connectBufPos;
                quint32 connModelAbsEnd = connModelNodeStart + connModelHeaderSize + connModelProps.size() + 13;

                connectStream << connModelAbsEnd;
                connectStream << static_cast<quint32>(3);  // 3 properties
                connectStream << static_cast<quint32>(connModelProps.size());
                connectStream << static_cast<quint8>(1);   // Name length = 1
                connectStream.writeRawData("C", 1);         // Node name "C" for Connection
                connectStream.writeRawData(connModelProps.constData(), connModelProps.size());
                writeNullRecord(connectStream);

                connectBufPos += connModelHeaderSize + connModelProps.size() + 13;

                // === Connection 3: Material -> Model ===
                QByteArray connMatProps;
                QDataStream connMatStream(&connMatProps, QIODevice::WriteOnly);
                connMatStream.setByteOrder(QDataStream::LittleEndian);
                encodeStringProperty(connMatStream, QByteArray("OO"));
                encodeInt64Property(connMatStream, materialIds[i]);
                encodeInt64Property(connMatStream, modelIds[i]);

                quint32 connMatHeaderSize = 4 + 4 + 4 + 1 + 1;  // endOffset + numProps + propListLen + nameLen + "C"
                quint32 connMatNodeStart = connectionsNodeStart + connectionsHeaderSize + connectBufPos;
                quint32 connMatAbsEnd = connMatNodeStart + connMatHeaderSize + connMatProps.size() + 13;

                connectStream << connMatAbsEnd;
                connectStream << static_cast<quint32>(3);  // 3 properties
                connectStream << static_cast<quint32>(connMatProps.size());
                connectStream << static_cast<quint8>(1);   // Name length = 1
                connectStream.writeRawData("C", 1);         // Node name "C" for Connection
                connectStream.writeRawData(connMatProps.constData(), connMatProps.size());
                writeNullRecord(connectStream);

                connectBufPos += connMatHeaderSize + connMatProps.size() + 13;
            }

            // Terminate Connections node's child list
            writeNullRecord(connectStream);
        }

        // Write Connections node header
        quint32 connectionsEndOffset = connectionsNodeStart + connectionsHeaderSize + connectionsBuffer.size();
        outFile << connectionsEndOffset;
        outFile << static_cast<quint32>(0);
        outFile << static_cast<quint32>(0);
        outFile << static_cast<quint8>(11);
        outFile.writeRawData("Connections", 11);
        outFile.writeRawData(connectionsBuffer.constData(), connectionsBuffer.size());
    }

    // === Top-level null record (terminates the list of top-level sibling nodes) ===
    writeNullRecord(outFile);

    return outFile.status() == QDataStream::Ok;
}

bool FBXExporter::writeNodeHeader(QDataStream &out, const QByteArray &name,
                                  quint32 endOffset, quint32 numProps, quint32 propListLen)
{
    out << endOffset;
    out << numProps;
    out << propListLen;
    out << static_cast<quint8>(name.size());
    out.writeRawData(name.constData(), name.size());
    return out.status() == QDataStream::Ok;
}

bool FBXExporter::writeFBXFooter(QDataStream &outFile)
{
    // Write 16 null bytes
    for (int i = 0; i < 16; i++)
    {
        outFile << static_cast<quint8>(0);
    }

    // Write version
    quint32 version = 7300;
    outFile << version;

    // Write 120 null bytes
    for (int i = 0; i < 120; i++)
    {
        outFile << static_cast<quint8>(0);
    }

    return outFile.status() == QDataStream::Ok;
}

float FBXExporter::transparencyToAlpha(int transparency)
{
    return 1.0f - (transparency * 0.25f);
}

float FBXExporter::shininessToRoughness(int shininess)
{
    static const float roughnessMap[] = { 0.8f, 0.5f, 0.2f, 0.0f };
    int idx = (shininess < 0) ? 0 : ((shininess > 3) ? 3 : shininess);
    return roughnessMap[idx];
}
