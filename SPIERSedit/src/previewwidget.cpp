/**
 * @file
 * Source: 3D Preview Widget
 *
 * All SPIERSedit code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSview code is Copyright 2008-2019 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

#include "previewwidget.h"
#include "globals.h"
#include "fileio.h"

#include <array>

#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QOpenGLContext>
#include <QtMath>

// ============================================================================
// PreviewBuilderWorker implementation
// ============================================================================

PreviewBuilderWorker::PreviewBuilderWorker(int buildId,
                                           int fileCount,
                                           int segCount,
                                           int w, int h,
                                           int renderMode,
                                           int currentFile,
                                           QByteArray currentFileMasks,
                                           int zMin,
                                           int zMax,
                                           int xyStep,
                                           int zStep,
                                           QObject *parent)
    : QObject(parent)
    , m_buildId(buildId)
    , m_fileCount(fileCount)
    , m_segCount(segCount)
    , m_width(w)
    , m_height(h)
    , m_renderMode(renderMode)
    , m_currentFile(currentFile)
    , m_currentFileMasks(std::move(currentFileMasks))
    , m_zMin(zMin)
    , m_zMax(zMax)
    , m_xyStep(xyStep)
    , m_zStep(qMax(1, zStep))
{
}

void PreviewBuilderWorker::process()
{
    // Only iterate over the user-specified Z-range.  Slices outside [m_zMin, m_zMax]
    // are not loaded at all — direct proportional reduction in build time.
    // The signal carries the texture *layer index* (z − m_zMin), not the absolute
    // slice number, so onSliceReady() can forward it straight to uploadSlice().
    // Step through slices by m_zStep.  Layer index is (z - m_zMin) / m_zStep so
    // that texel 0 = first included slice, texel 1 = first + m_zStep, etc.
    // The rendered cube is scaled to the physical depth (m_physicalDepth passed via
    // dataScale in PreviewWidget), so each thicker texel still maps to the correct
    // world-space Z extent.
    for (int z = m_zMin; z <= m_zMax; z += m_zStep)
    {
        if (cancelFlag.load(std::memory_order_relaxed)) break;

        QByteArray rgba = buildSliceLocal(z);

        if (cancelFlag.load(std::memory_order_relaxed)) break;

        if (!rgba.isEmpty())
            emit sliceReady(m_buildId, (z - m_zMin) / m_zStep, rgba);
    }
    emit finished(m_buildId);
}

QByteArray PreviewBuilderWorker::buildSliceLocal(int z)
{
    // Hold the outer recursive lock for the entire load+build sequence.
    // LoadGreyData() and LoadMasks() also call mutex.lock() internally —
    // that is safe because mutex is a QRecursiveMutex.
    mutex.lock();

    if (cancelFlag.load(std::memory_order_relaxed))
    {
        mutex.unlock();
        return QByteArray();
    }

    for (int s = 0; s < m_segCount; s++)
        LoadGreyData(z, s);

    // Determine which masks to use for this slice without permanently altering
    // the global Masks QByteArray visible to the main thread:
    //
    //  • Current file: cache may be stale (painting flushes only on slice
    //    change).  Use the in-memory snapshot captured at rebuildAll() time.
    //    Do NOT write to global Masks at all.
    //
    //  • All other slices: load from cache into a local copy, then restore
    //    global Masks to whatever it was before.  This keeps the editor's
    //    display engine seeing the correct mask between worker iterations.
    QByteArray sliceMasks;
    if (z == m_currentFile)
    {
        sliceMasks = m_currentFileMasks;
        // Leave global Masks untouched.
    }
    else
    {
        const QByteArray savedMasks = Masks;
        LoadMasks(z);           // sets global Masks to slice z's cache data
        sliceMasks = Masks;     // take a local copy
        Masks = savedMasks;     // restore global immediately (still inside mutex)
    }

    // -----------------------------------------------------------------
    // Inline RGBA build — equivalent to PreviewWidget::buildSliceRGBA().
    // Kept as a local copy to avoid cross-thread QObject method calls.
    // Keep in sync with buildSliceRGBA() when classification logic changes.
    // -----------------------------------------------------------------
    static const int THRESHOLD = 128;

    // Use ceiling division so the allocated row width matches the number of
    // x-loop iterations exactly.  Floor division would under-allocate by 1
    // pixel per row whenever fwidth/fheight is not a multiple of m_xyStep,
    // causing a 1-pixel-per-row buffer overflow that shifts every subsequent
    // row and corrupts the texture (appears as tiling + apparent rotation).
    const int outW = (m_width  + m_xyStep - 1) / m_xyStep;
    const int outH = (m_height + m_xyStep - 1) / m_xyStep;
    QByteArray out(outW * outH * 4, static_cast<char>(0));
    uchar *dst = reinterpret_cast<uchar *>(out.data());

    if (m_renderMode == static_cast<int>(PreviewWidget::RenderMode::SegmentMode))
    {
        for (int y = 0; y < m_height; y += m_xyStep)
        {
            for (int x = 0; x < m_width; x += m_xyStep)
            {
                int bestVal = THRESHOLD;
                int bestSeg = -1;

                for (int s = 0; s < m_segCount; s++)
                {
                    if (!Segments[s]->Activated) continue;
                    if (!GA[s] || GA[s]->isNull()) continue;
                    const int val = static_cast<int>(
                        *(GA[s]->bits() + (m_height - 1 - y) * fwidth4 + x));
                    if (val >= bestVal) { bestVal = val; bestSeg = s; }
                }

                if (bestSeg >= 0)
                {
                    dst[0] = static_cast<uchar>(Segments[bestSeg]->Colour[0]);
                    dst[1] = static_cast<uchar>(Segments[bestSeg]->Colour[1]);
                    dst[2] = static_cast<uchar>(Segments[bestSeg]->Colour[2]);
                    dst[3] = 255;
                }
                dst += 4;
            }
        }
    }
    else  // OutputMode
    {
        const int objCount = OutputObjects.count();

        // Build per-object mask lookup: objMasks[i][maskByte] == true iff that
        // mask index is in the object's ComponentMasks list.
        // Mirrors the UseMasks[] table in output.cpp so the preview matches
        // exactly what will be exported: segment data is only shown where the
        // per-pixel mask byte belongs to the output object's component masks.
        // sliceMasks[y * m_width + x] uses the same coordinate frame as GA[]:
        //   y=0 → bottom row (same as (m_height-1-y)=m_height-1 in GA bitmap).
        QVector<std::array<bool, 256>> objMasks(objCount);
        for (auto &arr : objMasks) arr.fill(false);
        for (int i = 0; i < objCount; i++)
        {
            const OutputObject *obj = OutputObjects[i];
            if (!obj->Show || obj->IsGroup) continue;
            for (int mi : obj->ComponentMasks)
                if (mi >= 0 && mi < 256) objMasks[i][mi] = true;
        }

        for (int y = 0; y < m_height; y += m_xyStep)
        {
            for (int x = 0; x < m_width; x += m_xyStep)
            {
                const uchar maskIdx = static_cast<uchar>(sliceMasks[y * m_width + x]);
                int  bestVal = THRESHOLD;
                int  bestR = 0, bestG = 0, bestB = 0;
                bool hit    = false;

                for (int i = 0; i < objCount; i++)
                {
                    const OutputObject *obj = OutputObjects[i];
                    if (!obj->Show || obj->IsGroup) continue;
                    if (!objMasks[i][maskIdx]) continue;  // pixel outside this object's masks

                    for (int j = 0; j < obj->ComponentSegments.count(); j++)
                    {
                        const int segIdx = obj->ComponentSegments[j];
                        if (segIdx < 0 || segIdx >= m_segCount) continue;
                        if (!GA[segIdx] || GA[segIdx]->isNull()) continue;
                        const int val = static_cast<int>(
                            *(GA[segIdx]->bits() + (m_height - 1 - y) * fwidth4 + x));
                        if (val >= bestVal)
                        {
                            bestVal = val;
                            bestR = obj->Colour[0];
                            bestG = obj->Colour[1];
                            bestB = obj->Colour[2];
                            hit   = true;
                        }
                    }
                }

                if (hit)
                {
                    dst[0] = static_cast<uchar>(bestR);
                    dst[1] = static_cast<uchar>(bestG);
                    dst[2] = static_cast<uchar>(bestB);
                    dst[3] = 255;
                }
                dst += 4;
            }
        }
    }

    mutex.unlock();
    return out;
}

// ============================================================================
// Cube geometry
// ============================================================================

// 8 vertices of a unit cube centred at origin [-0.5, 0.5]
static const float s_cubeVerts[24] =
{
    -0.5f, -0.5f, -0.5f,   // 0
     0.5f, -0.5f, -0.5f,   // 1
     0.5f,  0.5f, -0.5f,   // 2
    -0.5f,  0.5f, -0.5f,   // 3
    -0.5f, -0.5f,  0.5f,   // 4
     0.5f, -0.5f,  0.5f,   // 5
     0.5f,  0.5f,  0.5f,   // 6
    -0.5f,  0.5f,  0.5f,   // 7
};

// All wound CCW from outside (outward normals verified by cross-product).
static const GLushort s_cubeIndices[36] =
{
    4, 5, 6,  6, 7, 4,   // +z front  (normal +z)
    0, 3, 2,  2, 1, 0,   // -z back   (normal -z)
    0, 1, 5,  5, 4, 0,   // -y bottom (normal -y)
    2, 3, 7,  7, 6, 2,   // +y top    (normal +y)
    0, 4, 7,  7, 3, 0,   // -x left   (normal -x)
    1, 2, 6,  6, 5, 1,   // +x right  (normal +x)
};

static const int VOXEL_THRESHOLD = 128;
static const float RAY_STEP      = 0.005f;

// ============================================================================
// GLSL sources
// ============================================================================

static const char *s_vertSrc = R"(
#version 330 core
in  vec3 position;
out vec3 vPos;
uniform mat4 mvp;
void main() {
    vPos = position + vec3(0.5);
    gl_Position = mvp * vec4(position, 1.0);
}
)";

static const char *s_debugFragSrc = R"(
#version 330 core
in  vec3 vPos;
out vec4 fragColor;
void main() {
    fragColor = vec4(vPos, 1.0);
}
)";

static const char *s_volumeFragSrc = R"(
#version 330 core
in  vec3 vPos;
out vec4 fragColor;
uniform sampler3D volumeTex;
uniform vec3      cameraPosObj;
uniform float     stepSize;

void main() {
    vec3 rayOrigin = vPos;
    vec3 rayDir    = normalize(vPos - cameraPosObj);

    vec3 tMin = (vec3(0.0) - rayOrigin) / rayDir;
    vec3 tMax = (vec3(1.0) - rayOrigin) / rayDir;
    vec3 t1   = min(tMin, tMax);
    vec3 t2   = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar  = min(min(t2.x, t2.y), t2.z);

    if (tNear >= tFar) discard;
    tNear = max(tNear, 0.0);

    vec4 acc = vec4(0.0);
    float t  = tNear + stepSize * 0.5;

    for (int i = 0; i < 1024 && t < tFar; i++, t += stepSize) {
        vec4 s = texture(volumeTex, rayOrigin + t * rayDir);
        if (s.a > 0.01) {
            float alpha = s.a * (1.0 - acc.a);
            acc.rgb += alpha * s.rgb;
            acc.a   += alpha;
            if (acc.a > 0.99) break;
        }
    }

    if (acc.a < 0.01) discard;
    acc.rgb /= acc.a;
    fragColor = acc;
}
)";

// Overlay: simple pass-through with a per-draw uniform colour
static const char *s_overlayVertSrc = R"(
#version 330 core
in  vec3 position;
uniform mat4 mvp;
void main() {
    gl_Position = mvp * vec4(position, 1.0);
}
)";

static const char *s_overlayFragSrc = R"(
#version 330 core
uniform vec4 overlayColor;
out vec4 fragColor;
void main() {
    fragColor = overlayColor;
}
)";

// ============================================================================
// PreviewWidget — construction / destruction
// ============================================================================

PreviewWidget::PreviewWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    // No per-widget setFormat() — main.cpp sets QSurfaceFormat::setDefaultFormat()
    // with a Core Profile before QApplication is created.

    m_debounceTimer.setSingleShot(true);
    m_debounceTimer.setInterval(150);
    connect(&m_debounceTimer, &QTimer::timeout,
            this, &PreviewWidget::onDebounceTimeout);

    m_rotation = defaultRotation();
    setMinimumSize(200, 200);
}

PreviewWidget::~PreviewWidget()
{
    cancelWorker();
    makeCurrent();
    releaseTexture();
    m_cubeVBO.destroy();
    m_cubeIBO.destroy();
    m_vao.destroy();
    doneCurrent();
}

// ============================================================================
// Public API
// ============================================================================

void PreviewWidget::rebuildAll()
{
    if (m_renderMode == RenderMode::NoneMode)
    {
        cancelWorker();
        m_hasData = false;
        update();
        return;
    }

    if (!m_autoRender) return;

    if (!m_glReady)
    {
        m_pendingRebuild = true;
        return;
    }

    if (!IsDatasetLoaded())
    {
        invalidate();
        return;
    }

    // Cancel any in-progress build before starting a new one
    cancelWorker();

    // Clamp Z-range to the actual dataset extent.
    // m_zMax == 0 on first open (before resetPreviewBounds() sets it) — treat
    // that as "full range" so the very first build always shows everything.
    if (m_zMax <= 0 || m_zMax >= FileCount)  m_zMax = FileCount - 1;
    if (m_zMin < 0)                          m_zMin = 0;
    if (m_zMin > m_zMax)                     m_zMin = m_zMax;

    // Physical depth = actual slices in range; used for correct aspect ratio in
    // the rendered cube regardless of z-step.  texD = number of texture layers.
    m_physicalDepth = m_zMax - m_zMin + 1;

    // Ceiling division — matches the loop iteration counts in buildSliceLocal().
    const int texW = qMax(1, (fwidth  + m_xyStep - 1) / m_xyStep);
    const int texH = qMax(1, (fheight + m_xyStep - 1) / m_xyStep);
    const int texD = qMax(1, (m_physicalDepth + m_zStep - 1) / m_zStep);

    makeCurrent();
    if (m_texW != texW || m_texH != texH || m_texD != texD)
        initTexture(texW, texH, texD);
    doneCurrent();

    // Show partial renders immediately as slices arrive
    m_hasData         = true;
    m_loading         = true;
    m_loadingProgress = 0;
    m_buildId++;
    m_dirtySlices.clear();

    const int buildId = m_buildId;

    // Snapshot the current in-memory Masks so the worker can use it for the
    // active slice (cache may be stale — see onDebounceTimeout() comment).
    m_masksAtRebuild = Masks;

    m_worker = new PreviewBuilderWorker(buildId,
                                        FileCount, SegmentCount,
                                        fwidth, fheight,
                                        static_cast<int>(m_renderMode),
                                        CurrentFile,
                                        m_masksAtRebuild,
                                        m_zMin, m_zMax,
                                        m_xyStep,
                                        m_zStep);
    m_workerThread = new QThread(this);
    m_worker->moveToThread(m_workerThread);

    connect(m_workerThread, &QThread::started,
            m_worker,       &PreviewBuilderWorker::process);

    connect(m_worker, &PreviewBuilderWorker::sliceReady,
            this,     &PreviewWidget::onSliceReady);

    connect(m_worker, &PreviewBuilderWorker::finished,
            this,     &PreviewWidget::onBuildFinished);

    // Lifecycle cleanup — worker and thread delete themselves after finishing
    connect(m_worker,       &PreviewBuilderWorker::finished,
            m_workerThread, &QThread::quit);
    connect(m_workerThread, &QThread::finished,
            m_worker,       &QObject::deleteLater);
    connect(m_workerThread, &QThread::finished,
            m_workerThread, &QObject::deleteLater);

    m_workerThread->start();
    update();
}

void PreviewWidget::markSliceDirty(int z)
{
    if (z < 0 || z >= FileCount) return;
    if (z < m_zMin || z > m_zMax) return;  // outside current Z-range ROI — skip
    m_dirtySlices.insert(z);
    m_debounceTimer.start();
}

void PreviewWidget::invalidate()
{
    cancelWorker();
    m_hasData = false;
    m_dirtySlices.clear();
    m_debounceTimer.stop();
    m_rotation    = defaultRotation();
    m_zoom        = 1.0f;
    m_translation = QVector2D(0.0f, 0.0f);
    update();
}

void PreviewWidget::setDebounceDelay(int ms)
{
    m_debounceTimer.setInterval(ms);
}

void PreviewWidget::setRenderMode(RenderMode mode)
{
    if (m_renderMode == mode) return;
    m_renderMode = mode;
    if (mode == RenderMode::NoneMode)
    {
        cancelWorker();
        m_hasData = false;
        update();
    }
    else
    {
        rebuildAll();
    }
}

void PreviewWidget::setZStep(int step)
{
    if (step != 1 && step != 2 && step != 4 && step != 8) step = 1;
    if (step == m_zStep) return;
    m_zStep = step;
    rebuildAll();
}

void PreviewWidget::setAutoRender(bool enabled)
{
    m_autoRender = enabled;
}

void PreviewWidget::forceRebuild()
{
    if (m_renderMode == RenderMode::NoneMode) return;
    const bool saved = m_autoRender;
    m_autoRender = true;
    rebuildAll();
    m_autoRender = saved;
}

void PreviewWidget::setZRange(int zMin, int zMax)
{
    zMin = qMax(0, zMin);
    zMax = qMin(zMax, FileCount - 1);
    if (zMin > zMax) return;       // invalid range — ignore
    if (zMin == m_zMin && zMax == m_zMax) return;
    m_zMin = zMin;
    m_zMax = zMax;
    rebuildAll();
}

void PreviewWidget::setXYStep(int step)
{
    // Clamp to valid power-of-2 values
    if (step != 1 && step != 2 && step != 4 && step != 8) step = 1;
    if (step == m_xyStep) return;
    m_xyStep = step;
    rebuildAll();
}

void PreviewWidget::resetPreviewBounds()
{
    m_zMin   = 0;
    m_zMax   = qMax(0, FileCount - 1);
    m_xyStep = 8;   // ⅛ default — fast initial load; user raises quality as needed
    m_zStep  = 8;   // ⅛ default — matches XY default
    rebuildAll();
}

qint64 PreviewWidget::estimatedVRAMBytes(int xyStep, int physicalDepth, int zStep) const
{
    if (!IsDatasetLoaded() || fwidth <= 0 || fheight <= 0
            || physicalDepth <= 0 || xyStep <= 0 || zStep <= 0)
        return 0;
    const qint64 tw = (static_cast<qint64>(fwidth)        + xyStep - 1) / xyStep;
    const qint64 th = (static_cast<qint64>(fheight)       + xyStep - 1) / xyStep;
    const qint64 td = (static_cast<qint64>(physicalDepth) + zStep  - 1) / zStep;
    return tw * th * td * 4;  // RGBA = 4 bytes/texel
}

// ============================================================================
// Public slots (control bar)
// ============================================================================

void PreviewWidget::resetView()
{
    m_rotation    = m_hasData ? QQuaternion() : defaultRotation();
    m_zoom        = 1.0f;
    m_translation = QVector2D(0.0f, 0.0f);
    update();
}

void PreviewWidget::zoomIn()
{
    m_zoom = qMin(m_zoom * 1.25f, 20.0f);
    update();
}

void PreviewWidget::zoomOut()
{
    m_zoom = qMax(m_zoom / 1.25f, 0.05f);
    update();
}

// ============================================================================
// QOpenGLWidget overrides
// ============================================================================

void PreviewWidget::initializeGL()
{
    initializeOpenGLFunctions();
    m_ef = context()->extraFunctions();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    initShaders();
    initCubeGeometry();

    // ---- Query available GPU memory ----------------------------------------
    // Try vendor extensions; fall back to a conservative 2 GB default.
    // GL_NVX_gpu_memory_info  (NVIDIA) — GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX = 0x9049
    // GL_ATI_meminfo          (AMD)    — GL_TEXTURE_FREE_MEMORY_ATI = 0x87FC
    // Both report in KiB.
    {
        qint64 detectedBytes = 0;
        const QList<QByteArray> exts = context()->extensions().values();
        if (exts.contains("GL_NVX_gpu_memory_info")) {
            GLint kb = 0;
            glGetIntegerv(0x9049, &kb);
            if (kb > 0) detectedBytes = static_cast<qint64>(kb) * 1024;
        }
        if (detectedBytes == 0 && exts.contains("GL_ATI_meminfo")) {
            GLint info[4] = {};
            glGetIntegerv(0x87FC, info);
            if (info[0] > 0) detectedBytes = static_cast<qint64>(info[0]) * 1024;
        }
        // 2 GB conservative fallback (covers typical mid-range discrete GPUs
        // and Apple Silicon integrated GPUs with modest VRAM budgets).
        m_availableVRAMBytes = (detectedBytes > 0)
            ? detectedBytes
            : 2LL * 1024 * 1024 * 1024;

        qDebug() << "PreviewWidget: available VRAM ~"
                 << m_availableVRAMBytes / (1024 * 1024) << "MB"
                 << (detectedBytes > 0 ? "(detected)" : "(default)");
    }

    m_glReady = true;

    if (m_pendingRebuild)
    {
        m_pendingRebuild = false;
        QTimer::singleShot(0, this, &PreviewWidget::rebuildAll);
    }
}

void PreviewWidget::resizeGL(int, int)
{
    update();
}

void PreviewWidget::paintGL()
{
    glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_renderMode == RenderMode::NoneMode) return;

    if (m_hasData && m_volumeTexture != 0)
    {
        if (!m_volumeShader.isLinked()) return;
        m_volumeShader.bind();
        m_volumeShader.setUniformValue("mvp",          mvpMatrix());
        m_volumeShader.setUniformValue("cameraPosObj", cameraPosObj());
        m_volumeShader.setUniformValue("stepSize",     RAY_STEP);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_3D, m_volumeTexture);
        m_volumeShader.setUniformValue("volumeTex", 0);

        m_vao.bind();
        m_cubeIBO.bind();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
        m_cubeIBO.release();
        m_vao.release();

        glBindTexture(GL_TEXTURE_3D, 0);
        m_volumeShader.release();
    }
    else
    {
        if (!m_debugShader.isLinked()) return;
        m_debugShader.bind();
        m_debugShader.setUniformValue("mvp", mvpMatrix());

        m_vao.bind();
        m_cubeIBO.bind();
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);
        m_cubeIBO.release();
        m_vao.release();

        m_debugShader.release();
    }

    // ---- Position indicator overlay ----
    if (m_showPositionIndicator && m_hasData && m_overlayShader.isLinked())
        drawPositionIndicator();

    // ---- Loading overlay (QPainter composited over GL output) ----
    if (m_loading)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        const QString text = (m_loadingProgress > 0)
            ? QStringLiteral("Loading... %1%").arg(m_loadingProgress)
            : QStringLiteral("Loading...");

        const QFontMetrics fm(painter.font());
        const QRect tr   = fm.boundingRect(text);
        const int   pH   = 8, pV = 5;
        const int   pillW = tr.width()  + pH * 2;
        const int   pillH = tr.height() + pV * 2;
        const int   pillX = (width()  - pillW) / 2;
        const int   pillY =  height() - pillH - 12;

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 160));
        painter.drawRoundedRect(pillX, pillY, pillW, pillH, 4, 4);

        painter.setPen(Qt::white);
        painter.drawText(pillX + pH, pillY + pV + fm.ascent(), text);
    }
}

// ============================================================================
// Mouse / wheel
// ============================================================================

void PreviewWidget::mousePressEvent(QMouseEvent *e)
{
    m_lastMouse = e->pos();
}

void PreviewWidget::mouseMoveEvent(QMouseEvent *e)
{
    const QPoint delta = e->pos() - m_lastMouse;
    m_lastMouse = e->pos();

    if (e->buttons() & Qt::RightButton)
    {
        // Right drag → rotate
        m_rotation = (QQuaternion::fromAxisAndAngle(0, 1, 0, delta.x() * 0.5f)
                    * QQuaternion::fromAxisAndAngle(1, 0, 0, delta.y() * 0.5f)
                    * m_rotation).normalized();
        update();
    }
    else if (e->buttons() & Qt::LeftButton)
    {
        // Left drag → pan.  Scale so a full-height drag moves ~one volume width
        // at the current zoom level (frustum half-height at focus ≈ 0.83/zoom).
        const float scale = 1.66f / (m_zoom * qMax(1, height()));
        m_translation += QVector2D(-delta.x() * scale, delta.y() * scale);
        update();
    }
}

void PreviewWidget::mouseDoubleClickEvent(QMouseEvent *)
{
    resetView();
}

void PreviewWidget::wheelEvent(QWheelEvent *e)
{
    m_zoom *= qPow(1.15f, e->angleDelta().y() / 120.0f);
    m_zoom  = qBound(0.05f, m_zoom, 20.0f);
    update();
}

// ============================================================================
// Private slots
// ============================================================================

void PreviewWidget::onDebounceTimeout()
{
    // Suppress incremental updates during a full async rebuild —
    // rebuildAll() will cover all slices when it finishes.
    if (m_loading) return;

    // Don't update texture when auto-render is off.
    if (!m_autoRender) return;

    if (m_renderMode == RenderMode::NoneMode) return;

    if (!m_glReady || m_dirtySlices.isEmpty() || !IsDatasetLoaded()) return;

    makeCurrent();

    const int physD = m_zMax - m_zMin + 1;
    const int expW = qMax(1, (fwidth  + m_xyStep - 1) / m_xyStep);  // ceiling
    const int expH = qMax(1, (fheight + m_xyStep - 1) / m_xyStep);  // ceiling
    const int expD = qMax(1, (physD   + m_zStep  - 1) / m_zStep);

    if (m_texW != expW || m_texH != expH || m_texD != expD)
    {
        initTexture(expW, expH, expD);
        doneCurrent();
        rebuildAll();
        return;
    }

    const int savedFile = CurrentFile;

    // Snapshot the current in-memory Masks before touching any other slice.
    //
    // IMPORTANT: Masks is a value-copy QByteArray (not a pointer into the
    // cache).  Painting a mask sets MasksDirty=true but only flushes to the
    // cache when WriteAllData() is called on slice change.  Calling
    // LoadMasks(savedFile) after the loop would read the stale pre-paint
    // cache entry and silently destroy any in-progress mask strokes.
    //
    // GA[] by contrast IS the cache pointer (CacheCompressionLevel==0 path),
    // so LoadGreyData(savedFile,s) is safe and still needed if we loaded
    // other slices during this update.
    const QByteArray maskSnapshot = Masks;

    bool loadedOtherSlice = false;
    for (int z : m_dirtySlices)
    {
        // Skip slices that have drifted outside the current Z-range
        // (e.g. the range was narrowed after the dirty flag was set).
        if (z < m_zMin || z > m_zMax) continue;

        const int layer = z - m_zMin;   // texture layer index

        if (z == savedFile)
        {
            // In-memory GA[] and Masks are already authoritative — no load needed.
            uploadSlice(layer, buildSliceRGBA());
        }
        else
        {
            for (int s = 0; s < SegmentCount; s++) LoadGreyData(z, s);
            LoadMasks(z);
            uploadSlice(layer, buildSliceRGBA());
            loadedOtherSlice = true;
        }
    }

    // Restore GA[] to the current file if we loaded any other slice.
    // Restore Masks from the snapshot (never from cache) to preserve
    // any unpersisted mask painting on the current slice.
    if (loadedOtherSlice)
        for (int s = 0; s < SegmentCount; s++) LoadGreyData(savedFile, s);
    Masks = maskSnapshot;

    m_dirtySlices.clear();
    m_hasData = true;
    doneCurrent();
    update();
}

void PreviewWidget::onSliceReady(int buildId, int z, const QByteArray &rgba)
{
    // Discard slices from a superseded (cancelled) build
    if (buildId != m_buildId) return;

    makeCurrent();
    uploadSlice(z, rgba);
    doneCurrent();

    m_loadingProgress = (m_texD > 0) ? qRound((z + 1) * 100.0 / m_texD) : 100;
    update();
}

void PreviewWidget::onBuildFinished(int buildId)
{
    if (buildId != m_buildId) return;

    // Restore GA[] for the currently-displayed slice (worker left it at the
    // last slice it processed).  GA[] is safe to reload — with no cache
    // compression it IS the cache pointer, so any in-progress segment
    // painting is already reflected in the cache.
    //
    // Do NOT call LoadMasks(CurrentFile) here.  Masks is a value-copy
    // QByteArray; the cache is only updated by WriteAllData() on slice
    // change.  Calling LoadMasks() would silently discard any mask strokes
    // painted since rebuildAll() was triggered.  Leave Masks as-is: it
    // already holds the correct in-memory state.
    if (IsDatasetLoaded())
        for (int s = 0; s < SegmentCount; s++) LoadGreyData(CurrentFile, s);

    m_loading         = false;
    m_loadingProgress = 100;

    // deleteLater connections in rebuildAll() handle memory cleanup;
    // null the pointers so cancelWorker() doesn't double-act on them.
    m_worker       = nullptr;
    m_workerThread = nullptr;

    // If the user painted on the current slice while the rebuild was running
    // (debounce was suppressed by m_loading), kick it now so the texture
    // picks up those strokes.
    if (!m_dirtySlices.isEmpty())
        m_debounceTimer.start();

    update();
}

// ============================================================================
// Worker management
// ============================================================================

void PreviewWidget::cancelWorker()
{
    if (!m_workerThread) return;

    if (m_worker)
        m_worker->cancelFlag.store(true, std::memory_order_relaxed);

    m_workerThread->quit();
    if (!m_workerThread->wait(3000))
        m_workerThread->terminate();  // last resort — should never be reached

    m_worker       = nullptr;
    m_workerThread = nullptr;
    m_loading      = false;
}

// ============================================================================
// Position indicator
// ============================================================================

void PreviewWidget::setShowPositionIndicator(bool show)
{
    m_showPositionIndicator = show;
    update();
}

void PreviewWidget::drawPositionIndicator()
{
    // ---- Compute Z position of the current slice in object space [-0.5, 0.5] ----
    // Use physical depth so the indicator tracks correctly at all z-step values.
    float zObj = 0.0f;
    const int physD = (m_physicalDepth > 0) ? m_physicalDepth : m_texD;
    if (physD > 1)
        zObj = qBound(-0.5f,
                      float(CurrentFile - m_zMin) / float(physD - 1) - 0.5f,
                      0.5f);

    // ---- Compute XY hover position (LastMouseX/Y are image-pixel coords) ----
    const bool hasHover = (fwidth > 0 && fheight > 0 &&
                           LastMouseX >= 0 && LastMouseX < fwidth &&
                           LastMouseY >= 0 && LastMouseY < fheight);
    const float xObj = hasHover ? float(LastMouseX) / float(fwidth)  - 0.5f : 0.0f;
    const float yObj = hasHover ? 0.5f - float(LastMouseY) / float(fheight) : 0.0f;

    // ---- Build vertex data ----
    // Rectangle: 4 edges × 2 endpoints = 8 vertices (verts 0–7)
    // Crosshair: 2 lines × 2 endpoints = 4 vertices  (verts 8–11, only if hover valid)
    float verts[12 * 3];
    const float z = zObj;

    // Bottom edge
    verts[ 0]=-0.5f; verts[ 1]=-0.5f; verts[ 2]=z;
    verts[ 3]= 0.5f; verts[ 4]=-0.5f; verts[ 5]=z;
    // Right edge
    verts[ 6]= 0.5f; verts[ 7]=-0.5f; verts[ 8]=z;
    verts[ 9]= 0.5f; verts[10]= 0.5f; verts[11]=z;
    // Top edge
    verts[12]= 0.5f; verts[13]= 0.5f; verts[14]=z;
    verts[15]=-0.5f; verts[16]= 0.5f; verts[17]=z;
    // Left edge
    verts[18]=-0.5f; verts[19]= 0.5f; verts[20]=z;
    verts[21]=-0.5f; verts[22]=-0.5f; verts[23]=z;

    if (hasHover)
    {
        // Vertical crosshair line
        verts[24]=xObj; verts[25]=-0.5f; verts[26]=z;
        verts[27]=xObj; verts[28]= 0.5f; verts[29]=z;
        // Horizontal crosshair line
        verts[30]=-0.5f; verts[31]=yObj; verts[32]=z;
        verts[33]= 0.5f; verts[34]=yObj; verts[35]=z;
    }

    // ---- Upload vertices ----
    m_overlayVBO.bind();
    m_overlayVBO.write(0, verts, (hasHover ? 12 : 8) * 3 * sizeof(float));
    m_overlayVBO.release();

    // ---- GL state for overlay ----
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(2.0f);

    m_overlayShader.bind();
    m_overlayShader.setUniformValue("mvp", mvpMatrix());

    m_overlayVAO.bind();

    // Draw slice-plane rectangle (yellow)
    m_overlayShader.setUniformValue("overlayColor", QVector4D(1.0f, 0.85f, 0.0f, 0.9f));
    glDrawArrays(GL_LINES, 0, 8);

    // Draw crosshair (cyan), only when hover position is valid
    if (hasHover)
    {
        m_overlayShader.setUniformValue("overlayColor", QVector4D(0.0f, 0.9f, 1.0f, 0.9f));
        glDrawArrays(GL_LINES, 8, 4);
    }

    m_overlayVAO.release();
    m_overlayShader.release();

    // ---- Restore GL state ----
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

// ============================================================================
// GL resource helpers
// ============================================================================

void PreviewWidget::initShaders()
{
    auto compile = [](QOpenGLShaderProgram &prog, const char *vert, const char *frag,
                      const char *label)
    {
        if (!prog.addShaderFromSourceCode(QOpenGLShader::Vertex,   vert))
            qWarning() << "PreviewWidget" << label << "vertex:"   << prog.log();
        if (!prog.addShaderFromSourceCode(QOpenGLShader::Fragment, frag))
            qWarning() << "PreviewWidget" << label << "fragment:" << prog.log();
        if (!prog.link())
            qWarning() << "PreviewWidget" << label << "link:"     << prog.log();
    };

    compile(m_debugShader,   s_vertSrc,        s_debugFragSrc,   "debug");
    compile(m_volumeShader,  s_vertSrc,        s_volumeFragSrc,  "volume");
    compile(m_overlayShader, s_overlayVertSrc, s_overlayFragSrc, "overlay");
}

void PreviewWidget::initCubeGeometry()
{
    m_vao.create();
    m_vao.bind();

    m_cubeVBO.create();
    m_cubeVBO.bind();
    m_cubeVBO.allocate(s_cubeVerts, sizeof(s_cubeVerts));

    m_cubeIBO.create();
    m_cubeIBO.bind();
    m_cubeIBO.allocate(s_cubeIndices, sizeof(s_cubeIndices));

    // Bind 'position' attribute for both shaders so the same VAO works for both
    for (QOpenGLShaderProgram *prog : { &m_debugShader, &m_volumeShader })
    {
        prog->bind();
        const int loc = prog->attributeLocation("position");
        prog->enableAttributeArray(loc);
        prog->setAttributeBuffer(loc, GL_FLOAT, 0, 3, 3 * sizeof(float));
        prog->release();
    }

    m_cubeVBO.release();
    m_vao.release();

    // Overlay VAO/VBO: 12 vertices × 3 floats, updated dynamically each frame
    m_overlayVAO.create();
    m_overlayVAO.bind();

    m_overlayVBO.create();
    m_overlayVBO.bind();
    m_overlayVBO.allocate(12 * 3 * sizeof(float));  // pre-allocate; data written per frame

    m_overlayShader.bind();
    const int overlayLoc = m_overlayShader.attributeLocation("position");
    m_overlayShader.enableAttributeArray(overlayLoc);
    m_overlayShader.setAttributeBuffer(overlayLoc, GL_FLOAT, 0, 3, 3 * sizeof(float));
    m_overlayShader.release();

    m_overlayVBO.release();
    m_overlayVAO.release();
}

void PreviewWidget::initTexture(int w, int h, int d)
{
    releaseTexture();

    glGenTextures(1, &m_volumeTexture);
    glBindTexture(GL_TEXTURE_3D, m_volumeTexture);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    QByteArray empty(w * h * d * 4, static_cast<char>(0));
    m_ef->glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA8, w, h, d, 0,
                       GL_RGBA, GL_UNSIGNED_BYTE, empty.constData());
    glBindTexture(GL_TEXTURE_3D, 0);

    m_texW = w; m_texH = h; m_texD = d;
}

void PreviewWidget::releaseTexture()
{
    if (m_volumeTexture)
    {
        glDeleteTextures(1, &m_volumeTexture);
        m_volumeTexture = 0;
    }
    m_texW = m_texH = m_texD = 0;
    m_hasData = false;
}

// ============================================================================
// Slice upload (must be called with GL context current)
// ============================================================================

void PreviewWidget::uploadSlice(int z, const QByteArray &rgba)
{
    if (!m_volumeTexture || rgba.isEmpty()) return;

    // Reset pixel-unpack state so Qt's QPainter compositing (which may set
    // GL_UNPACK_ROW_LENGTH to a non-zero value internally) cannot misalign
    // the upload.
    glPixelStorei(GL_UNPACK_ALIGNMENT,  4);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    glBindTexture(GL_TEXTURE_3D, m_volumeTexture);
    m_ef->glTexSubImage3D(GL_TEXTURE_3D, 0,
                          0, 0, z,
                          m_texW, m_texH, 1,
                          GL_RGBA, GL_UNSIGNED_BYTE,
                          rgba.constData());
    glBindTexture(GL_TEXTURE_3D, 0);
}

// ============================================================================
// buildSliceRGBA — main-thread version (used by debounce dirty-slice path)
// ============================================================================

QByteArray PreviewWidget::buildSliceRGBA() const
{
    // Ceiling division — must match buildSliceLocal() for identical buffer layout.
    const int outW = qMax(1, (fwidth  + m_xyStep - 1) / m_xyStep);
    const int outH = qMax(1, (fheight + m_xyStep - 1) / m_xyStep);
    QByteArray out(outW * outH * 4, static_cast<char>(0));
    uchar *dst = reinterpret_cast<uchar *>(out.data());

    if (m_renderMode == RenderMode::SegmentMode)
    {
        for (int y = 0; y < fheight; y += m_xyStep)
        {
            for (int x = 0; x < fwidth; x += m_xyStep)
            {
                int bestVal = VOXEL_THRESHOLD, bestSeg = -1;
                for (int s = 0; s < SegmentCount; s++)
                {
                    if (!Segments[s]->Activated) continue;
                    if (!GA[s] || GA[s]->isNull()) continue;
                    const int val = static_cast<int>(
                        *(GA[s]->bits() + (fheight - 1 - y) * fwidth4 + x));
                    if (val >= bestVal) { bestVal = val; bestSeg = s; }
                }
                if (bestSeg >= 0)
                {
                    dst[0] = static_cast<uchar>(Segments[bestSeg]->Colour[0]);
                    dst[1] = static_cast<uchar>(Segments[bestSeg]->Colour[1]);
                    dst[2] = static_cast<uchar>(Segments[bestSeg]->Colour[2]);
                    dst[3] = 255;
                }
                dst += 4;
            }
        }
    }
    else  // OutputMode
    {
        const int objCount = OutputObjects.count();

        // Build per-object mask lookup — see buildSliceLocal() for full comment.
        QVector<std::array<bool, 256>> objMasks(objCount);
        for (auto &arr : objMasks) arr.fill(false);
        for (int i = 0; i < objCount; i++)
        {
            const OutputObject *obj = OutputObjects[i];
            if (!obj->Show || obj->IsGroup) continue;
            for (int mi : obj->ComponentMasks)
                if (mi >= 0 && mi < 256) objMasks[i][mi] = true;
        }

        for (int y = 0; y < fheight; y += m_xyStep)
        {
            for (int x = 0; x < fwidth; x += m_xyStep)
            {
                const uchar maskIdx = static_cast<uchar>(Masks[y * fwidth + x]);
                int  bestVal = VOXEL_THRESHOLD;
                int  bestR = 0, bestG = 0, bestB = 0;
                bool hit    = false;

                for (int i = 0; i < objCount; i++)
                {
                    const OutputObject *obj = OutputObjects[i];
                    if (!obj->Show || obj->IsGroup) continue;
                    if (!objMasks[i][maskIdx]) continue;  // pixel outside this object's masks

                    for (int j = 0; j < obj->ComponentSegments.count(); j++)
                    {
                        const int segIdx = obj->ComponentSegments[j];
                        if (segIdx < 0 || segIdx >= SegmentCount) continue;
                        if (!GA[segIdx] || GA[segIdx]->isNull()) continue;
                        const int val = static_cast<int>(
                            *(GA[segIdx]->bits() + (fheight - 1 - y) * fwidth4 + x));
                        if (val >= bestVal)
                        {
                            bestVal = val;
                            bestR = obj->Colour[0];
                            bestG = obj->Colour[1];
                            bestB = obj->Colour[2];
                            hit   = true;
                        }
                    }
                }
                if (hit)
                {
                    dst[0] = static_cast<uchar>(bestR);
                    dst[1] = static_cast<uchar>(bestG);
                    dst[2] = static_cast<uchar>(bestB);
                    dst[3] = 255;
                }
                dst += 4;
            }
        }
    }

    return out;
}

// ============================================================================
// Camera
// ============================================================================

// static
QQuaternion PreviewWidget::defaultRotation()
{
    return (QQuaternion::fromAxisAndAngle(0, 1, 0, 45.0f)
          * QQuaternion::fromAxisAndAngle(1, 0, 0, 30.0f)).normalized();
}

// ---------------------------------------------------------------------------
// Helper: normalised (sx, sy, sz) scale factors for the model cube.
//
// The unit cube always occupies [0,1]³ in texture space regardless of how
// many texels we stored (i.e. regardless of m_xyStep).  Without a scale
// correction, reducing XY resolution by 2× keeps Z the same number of
// slices, so the rendered volume appears twice as "tall" in Z — objects
// look squashed and the user perceives a width/height distortion.
//
// The fix: scale the rendered cube so it always reflects the PHYSICAL DATA
// extent (fwidth × fheight pixels × m_texD slices), normalised to fit in a
// unit sphere.  Because fwidth and fheight are independent of m_xyStep, the
// rendered proportions are identical at all XY step values — just at
// different resolutions.  When no dataset is loaded the debug cube uses 1:1:1.
// ---------------------------------------------------------------------------
static QVector3D dataScale(bool hasData, int fw, int fh, int texD)
{
    if (!hasData || fw <= 0 || fh <= 0 || texD <= 0)
        return QVector3D(1.0f, 1.0f, 1.0f);
    const float maxDim = static_cast<float>(qMax(fw, qMax(fh, texD)));
    return QVector3D(fw / maxDim, fh / maxDim, texD / maxDim);
}

QMatrix4x4 PreviewWidget::mvpMatrix() const
{
    const float aspect = width() > 0
        ? static_cast<float>(width()) / static_cast<float>(height() > 0 ? height() : 1)
        : 1.0f;

    // Physical data proportions: scale cube axes so XY pixels and Z slices
    // are rendered at their true relative extents (independent of m_xyStep and
    // m_zStep).  Always use m_physicalDepth (actual slice count in range), not
    // m_texD (texel count), so z-step downsampling does not squash the volume.
    const int scaleD = (m_physicalDepth > 0) ? m_physicalDepth : m_texD;
    const QVector3D ds = dataScale(m_hasData, fwidth, fheight, scaleD);

    QMatrix4x4 proj, view, model;
    proj.perspective(45.0f, aspect, 0.01f, 100.0f);
    // m_translation shifts the camera and its target together so the model
    // appears to slide across the viewport (pan) without changing perspective.
    view.lookAt(QVector3D(m_translation.x(), m_translation.y(), 2.0f / m_zoom),
                QVector3D(m_translation.x(), m_translation.y(), 0.0f),
                QVector3D(0, 1, 0));
    // Apply rotation first, then scale in local (data) space:
    // model = R * S  =>  transforms vertex p as R*(S*p).
    model.rotate(m_rotation);
    model.scale(ds.x(), ds.y(), ds.z());
    return proj * view * model;
}

QVector3D PreviewWidget::cameraPosObj() const
{
    // Camera sits at (tx, ty, 2/zoom) in world space after pan (view.lookAt above).
    // To get texture-space position we must undo the full model transform
    // M = R * S:  c_tex = S^-1 * R^-1 * c_world + (0.5, 0.5, 0.5).
    const int scaleD2 = (m_physicalDepth > 0) ? m_physicalDepth : m_texD;
    const QVector3D ds = dataScale(m_hasData, fwidth, fheight, scaleD2);

    QMatrix4x4 invR;
    invR.rotate(m_rotation.inverted());
    QVector3D camObj = invR.map(QVector3D(m_translation.x(), m_translation.y(), 2.0f / m_zoom));

    // Apply S^-1: divide each component by the corresponding scale factor.
    camObj = QVector3D(camObj.x() / ds.x(),
                       camObj.y() / ds.y(),
                       camObj.z() / ds.z());

    // Shift from object space (±0.5 cube) to texture space ([0,1]³).
    return camObj + QVector3D(0.5f, 0.5f, 0.5f);
}
