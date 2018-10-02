#include "SPIERSviewglobals.h"

QString fname; //filename passed in argv
QString currentfile; //SPV file being processed at moment (used for status)
bool IsSP2;
MainWindow *MainWin;
bool validprogbar, FileDirty=false;
int NextActualDlist=1;
int FirstObject;
int TotalTriangles=0;
bool SP2_lock=false;
bool ContainsPresurfaced=false;
bool ContainsNonPresurfaced=false;
int back_red, back_green, back_blue;
int grid_red, grid_green, grid_blue;
int grid_minor_red, grid_minor_green, grid_minor_blue;
float Xrot, Yrot, Zrot;
float Xtrans, Ytrans, Ztrans;

QString STLHash;

float globalrescale;
QProgressBar *PB;
QLabel *OL;
int model_ktr;
QOpenGLBuffer cubeBuffer;


