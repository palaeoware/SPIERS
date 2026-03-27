/**
 * @file
 * Header: 3D Preview Widget
 *
 * Real-time volume preview for SPIERSedit using GPU ray-cast rendering.
 * Two render modes: Segment (activated segments) and Output (output list).
 *
 * Texture building runs on a background thread (PreviewBuilderWorker) so
 * the main thread stays responsive during full rebuilds on large datasets.
 * A "Loading… X%" pill is drawn over the GL viewport while the worker runs.
 */

#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <atomic>

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QByteArray>
#include <QThread>
#include <QTimer>
#include <QSet>
#include <QPoint>

// ---------------------------------------------------------------------------
// PreviewBuilderWorker
// ---------------------------------------------------------------------------

/**
 * @brief Background worker that builds RGBA slice data for the 3D texture.
 *
 * Moved to a dedicated QThread via moveToThread().  Processes slices
 * serially under the global mutex (LoadGreyData / LoadMasks write shared
 * globals GA[] and Masks, so cross-slice parallelism is not safe without
 * separate per-thread buffers).
 *
 * Each completed slice is emitted via sliceReady() as a QueuedConnection
 * so the main thread can upload it to the GPU incrementally.
 * A buildId token lets the main thread discard signals from a superseded
 * (cancelled) build.
 */
class PreviewBuilderWorker : public QObject
{
    Q_OBJECT

public:
    explicit PreviewBuilderWorker(int buildId,
                                  int fileCount,
                                  int segCount,
                                  int w, int h,
                                  int renderMode,
                                  int currentFile,
                                  QByteArray currentFileMasks,
                                  int zMin,
                                  int zMax,
                                  int xyStep,
                                  QObject *parent = nullptr);

    /** Thread-safe cancel flag.  Set by PreviewWidget on the main thread. */
    std::atomic<bool> cancelFlag { false };

public slots:
    /** Entry point — called via QThread::started signal. */
    void process();

signals:
    /** One RGBA buffer per slice, delivered to main thread via QueuedConnection. */
    void sliceReady(int buildId, int z, QByteArray rgba);
    /** Emitted once when all slices are done or cancellation is detected. */
    void finished(int buildId);

private:
    /**
     * Loads grey and mask data for slice @p z under the global mutex, builds
     * the RGBA buffer inline (equivalent to PreviewWidget::buildSliceRGBA()),
     * releases the mutex, and returns the buffer.
     * Returns an empty QByteArray if cancelled mid-flight.
     *
     * NOTE: The RGBA-building logic here is intentionally a local copy of
     * PreviewWidget::buildSliceRGBA().  Calling that method from this thread
     * would require cross-thread QObject access, which is unsafe.  Keep both
     * copies in sync when the classification logic changes.
     */
    QByteArray buildSliceLocal(int z);

    int        m_buildId;
    int        m_fileCount;
    int        m_segCount;
    int        m_width;
    int        m_height;
    int        m_renderMode;        // 0 = SegmentMode, 1 = OutputMode
    int        m_currentFile;       // slice that was active at rebuildAll() time
    QByteArray m_currentFileMasks;  // in-memory Masks snapshot for m_currentFile
    int        m_zMin;              // first slice to process (absolute index)
    int        m_zMax;              // last  slice to process (inclusive)
    int        m_xyStep;            // pixel loop stride: 1=full, 2=half, 4=quarter, 8=eighth
};

// ---------------------------------------------------------------------------
// PreviewWidget
// ---------------------------------------------------------------------------

/**
 * @brief Real-time volume preview dock widget.
 *
 * Maintains a GL_TEXTURE_3D whose voxels hold the RGBA colour of the
 * dominant activated segment (Segment mode) or matching output object
 * (Output mode).  A ray-cast fragment shader composites the volume
 * front-to-back.
 *
 * When no dataset is loaded an RGB debug cube is shown at a 45-degree
 * corner-on orientation so the user can confirm the GL pipeline is live.
 *
 * Full rebuilds are performed asynchronously by PreviewBuilderWorker.
 * A "Loading… X%" pill overlay is shown until the build completes.
 */
class PreviewWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    /** Selects what the volume texture encodes. */
    enum class RenderMode { SegmentMode, OutputMode };

    explicit PreviewWidget(QWidget *parent = nullptr);
    ~PreviewWidget();

    /** Full async rebuild across all slices — call after opening/closing a dataset. */
    void rebuildAll();

    /**
     * Estimated GPU texture memory for the given XY stride and Z-slice count.
     * Returns 0 when no dataset is loaded.  Used by the UI to disable modes
     * that would exceed available VRAM.
     */
    qint64 estimatedVRAMBytes(int xyStep, int texD) const;

    /**
     * Available GPU memory in bytes, queried once in initializeGL().
     * Uses vendor extensions where possible; falls back to 2 GB.
     */
    qint64 availableVRAMBytes() const { return m_availableVRAMBytes; }

    /** Mark slice @p z as dirty; schedules a debounced incremental upload. */
    void markSliceDirty(int z);

    /** Clear the texture (no dataset loaded). */
    void invalidate();

    /** Debounce delay in milliseconds (default 150). */
    void setDebounceDelay(int ms);

    /** Switch between segment and output render modes. */
    void setRenderMode(RenderMode mode);

    RenderMode renderMode() const { return m_renderMode; }

public slots:
    /** Reset camera to default orientation and zoom. */
    void resetView();
    void zoomIn();
    void zoomOut();

    /**
     * Restrict the rebuilt volume to slices [zMin, zMax] (absolute indices).
     * Values are clamped to [0, FileCount-1].  Triggers a full async rebuild.
     */
    void setZRange(int zMin, int zMax);

    /**
     * Set the XY pixel-loop stride used when building the RGBA texture.
     * @p step must be 1, 2, 4, or 8.  Triggers a full async rebuild.
     */
    void setXYStep(int step);

    /**
     * Reset Z-range and XY step to defaults (full dataset, full resolution).
     * Call after opening a new project so the controls reset without triggering
     * a double rebuild (set the widgets first with QSignalBlocker, then call this).
     */
    void resetPreviewBounds();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;

private slots:
    void onDebounceTimeout();
    void onSliceReady(int buildId, int z, const QByteArray &rgba);
    void onBuildFinished(int buildId);

private:
    // GL resource helpers
    void initShaders();
    void initCubeGeometry();
    void initTexture(int w, int h, int d);
    void releaseTexture();

    // Single-slice upload — caller must hold GL context (makeCurrent)
    void uploadSlice(int z, const QByteArray &rgba);

    // Builds RGBA for the currently-loaded GA/Masks globals (main thread only)
    QByteArray buildSliceRGBA() const;

    // Camera
    QMatrix4x4 mvpMatrix() const;
    QVector3D  cameraPosObj() const;
    static QQuaternion defaultRotation();

    // Worker thread management
    void cancelWorker();

    // GL objects
    GLuint                   m_volumeTexture = 0;
    QOpenGLShaderProgram     m_debugShader;
    QOpenGLShaderProgram     m_volumeShader;
    QOpenGLBuffer            m_cubeVBO { QOpenGLBuffer::VertexBuffer };
    QOpenGLBuffer            m_cubeIBO { QOpenGLBuffer::IndexBuffer };
    QOpenGLVertexArrayObject m_vao;

    // Texture dimensions
    int m_texW = 0, m_texH = 0, m_texD = 0;

    // Camera state
    QQuaternion m_rotation;
    float       m_zoom      = 1.0f;
    QPoint      m_lastMouse;

    // Debounce
    QTimer    m_debounceTimer;
    QSet<int> m_dirtySlices;

    // Async build state
    bool    m_loading         = false;
    int     m_loadingProgress = 0;    // 0–100
    int     m_buildId         = 0;    // incremented each rebuildAll()

    QThread              *m_workerThread  = nullptr;
    PreviewBuilderWorker *m_worker        = nullptr;
    QByteArray            m_masksAtRebuild;  // Masks snapshot passed to worker

    // ROI / resolution controls
    int m_zMin   = 0;   // first slice included in texture (absolute index)
    int m_zMax   = 0;   // last  slice included (inclusive); set to FileCount-1 on open
    int m_xyStep = 8;   // pixel-loop stride: 1=full res, 2=half, 4=quarter, 8=eighth
                        // Default is ⅛ for fast initial load; user can raise quality.

    // VRAM
    qint64 m_availableVRAMBytes = 2LL * 1024 * 1024 * 1024;  // set in initializeGL()

    RenderMode m_renderMode   = RenderMode::SegmentMode;
    bool       m_hasData      = false;
    bool       m_glReady      = false;
    bool       m_pendingRebuild = false;

    QOpenGLExtraFunctions *m_ef = nullptr;
};

#endif // PREVIEWWIDGET_H
