#include "svglwidget.h"
#include "SPIERSviewglobals.h"
#include "svobject.h"
#include "spv.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QElapsedTimer>
#include <QDebug>
#include <QMatrix4x4>

float ScaleMatrix[16];
float GlobalMatrix[16];
float Default0Matrix[16];
float DefaultClipAngle;

int ScaleBallColour[3]; //info stuff
float ScaleBallScale;
float mm_per_unit;

 GLfloat SVGlWidget::LightAmbient[]= { 0.1f, 0.1f, 0.1f, 1.0f }; 				// Ambient Light Values
                                 // Diffuse Light Values
 GLfloat SVGlWidget::LightPosition[]= { 0.0f, 0.0f, 5.0f, 1.0f };				 // Light Position

SVGlWidget::SVGlWidget(QWidget *parent)
         : QGLWidget(QGLFormat(),parent)
{
    DefaultClipAngle=ClipAngle;
    LastMouseXpos=-1;
    LastMouseYpos=-1;
    setMouseTracking(true);//do nothing except pass parent on
    zoom=500;
    LightDiffuse[0]=.9f;
    LightDiffuse[1]=.9f;
    LightDiffuse[2]=.9f;
    LightDiffuse[3]=1.0f;
    

    LightRed[0]=.9f;
    LightRed[1]=.2f;
    LightRed[2]=.2f;
    LightRed[3]=1.0f;

    LightGreen[0]=.2f;
    LightGreen[1]=.2f;
    LightGreen[2]=.9f;
    LightGreen[3]=1.0f;

    StereoSeparation=.04;

	LastDrawQuad=false;


 }

void SVGlWidget::setsamples(int i)
{
    return;
    QGLFormat fmt;
    fmt.setDoubleBuffer(true);                 // single buffer
    fmt.setVersion(3,3);
    if (MainWin->ui->actionQuadBuffer_Stereo->isChecked()) fmt.setStereo(true);					//turns on quad buffering
    else  fmt.setStereo(false);
    //    fmt.setDirectRendering(false);
    if (i==0) fmt.setSampleBuffers(false);
    else fmt.setSampleBuffers(true);

    if (i>0) fmt.setSamples(i);

    setFormat(fmt);
}


void SVGlWidget::initializeGL()
{
    //initializeGLFunctions();
   // setsamples(4);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_VERTEX_ARRAY);

    qglClearColor(Qt::black);

    setlocale(LC_NUMERIC, "C");
    bool result=simple_shader.addShaderFromSourceFile(QGLShader::Vertex, ":/simple_shader_vertex.vert" );
    if ( !result )
        qDebug() << "Compile error vertex shader:"<< simple_shader.log();

    result=simple_shader.addShaderFromSourceFile(QGLShader::Fragment, "qrc:/simple_shader_fragment.frag"  );
    if ( !result )
        qDebug() << "Compile error fragment shader:" << simple_shader.log();

    // ...and finally we link them to resolve any references.
    result = simple_shader.link();
    if ( !result )
        qDebug() << "Could not link shader program:" << simple_shader.log();
    else
        qDebug()<<"Linked OK";

    if ( !simple_shader.bind() )
    {
        qDebug() << "Could not bind shader program to context";
        return;
    }
    setlocale(LC_ALL, "");
    // Enable depth buffer

    vertices << QVector3D(1, 0, -2) << QVector3D(0, 1, -2) << QVector3D(-1, 0, -2);
    // Enable back face culling
    //glEnable(GL_CULL_FACE);

/*OLD
     // Set up the rendering context, define display lists etc.:
    glEnable(GL_LIGHTING);
    //glShadeModel(GL_SMOOTH);
    glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1);
    glClearColor((float)back_red/(float)255, (float)back_green/(float)255, (float)back_blue/(float)255, 0.5f);				// Black Background
    glClearDepth(1.0f);									// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);				// Setup The Ambient Light

    glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);				// Setup The Diffuse Light
    glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);                            // Position The Light
    glEnable(GL_LIGHT1);							// Enable Light One

    quadratic=gluNewQuadric();			// Create A Pointer To The Quadric Object ( NEW )
    gluQuadricNormals(quadratic, GLU_SMOOTH);	// Create Smooth Normals ( NEW )
    gluQuadricTexture(quadratic, GL_TRUE);	// Create Texture Coords ( NEW )

    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColorMaterial ( GL_FRONT_AND_BACK, GL_DIFFUSE) ;
    glEnable(GL_NORMALIZE);

*/
}

void SVGlWidget::resizeGL(int w, int h)
{

    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = .001, zFar = 10000.0, fov = 60.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(ClipAngle, aspect, ClipStart,ClipDepth);
    glViewport(0,0,w,h);
/*
    xdim=w; ydim=h;
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix

    if (MainWin->ui->actionSplit_Stereo->isChecked())
    {
        glViewport(0,0,w/2,h);
        gluPerspective(ClipAngle,(GLfloat)(w/2)/(GLfloat)h,ClipStart,ClipDepth);
    }
    else
    {
        glViewport(0,0,w,h);
        gluPerspective(ClipAngle,(GLfloat)w/(GLfloat)h,ClipStart,ClipDepth);
    }

    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
    glLoadIdentity();									// Reset The Modelview Matrix
*/
}

bool SVGlWidget::CanISee(int index)
{
    //if not in a group - just return my visibility
    if (SVObjects[index]->Visible==false) return false;
    if (SVObjects[index]->InGroup==-1) return true; //not in a group,visble, fine

    //in a group and visible - return visibility of parents
    return CanISee(SVObjects[index]->Parent()); //run again on my parents
}

void SVGlWidget::recenter_ball()
{
    glLoadIdentity();
    glScalef(ScaleBallScale, ScaleBallScale, ScaleBallScale);
    if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, ScaleMatrix); //retrieve
    FileDirty=true;
    return;
}

void SVGlWidget::DrawObjects(bool rightview)
{


    if (MainWin->ui->actionShow_Ball_2->isChecked())
    {
        float factor=(float)(zoom-600)/(float)100;

        glLoadIdentity();
        if (rightview)
              gluLookAt(StereoSeparation*3,0,3.0,0,0,0, 0,1,0);
        else
              gluLookAt(0,0,3,0,0,0, 0,1,0);

        glTranslatef(0, 0, factor);
        glMultMatrixf(ScaleMatrix);

        glMultMatrixf(GlobalMatrix); //apply global rescaling - for VAXML primarily

        glEnable (GL_COLOR_MATERIAL);
        glColor4f(((GLfloat)(ScaleBallColour[0]))/255, ((GLfloat)(ScaleBallColour[1]))/255, ((GLfloat)(ScaleBallColour[2]))/255, 1.0f);
        gluSphere(quadratic,.5,50,50);
        glDisable (GL_COLOR_MATERIAL);

    }
  //do several times, draw opaques first, then trans
  for (int transflag=0; transflag<2; transflag++)
  for (int i=0; i<SVObjects.count(); i++)
  {
      if (!(SVObjects[i]->IsGroup))
      if (CanISee(i))
      {
          float factor=(float)(zoom-600)/(float)100;

          glLoadIdentity();
          if (rightview)
                gluLookAt(StereoSeparation*3,0,3.0,0,0,0, 0,1,0);
          else
                gluLookAt(0,0,3,0,0,0, 0,1,0);

          glTranslatef(0, 0, factor);
          glMultMatrixf(SVObjects[i]->matrix);

        glMultMatrixf(GlobalMatrix); //apply global rescaling - for VAXML primarily


          float mcolor[3];
          if (MainWin->ui->actionMute_Colours->isChecked())
          {
              mcolor[0]= ((GLfloat)(SVObjects[i]->Colour[0]/3+170))/255;
              mcolor[1]= ((GLfloat)(SVObjects[i]->Colour[1]/3+170))/255;
              mcolor[2]= ((GLfloat)(SVObjects[i]->Colour[2]/3+170))/255;
          }
         else
          {
              mcolor[0]= ((GLfloat)(SVObjects[i]->Colour[0]))/255;
              mcolor[1]= ((GLfloat)(SVObjects[i]->Colour[1]))/255;
              mcolor[2]= ((GLfloat)(SVObjects[i]->Colour[2]))/255;
          }

         if (transflag)
         if (SVObjects[i]->Transparency!=0 && transflag==1) //do trans on second run
          {
            glDepthMask(false);
            glEnable(GL_BLEND);
            glEnable (GL_COLOR_MATERIAL);
            float t;
            if (SVObjects[i]->Transparency==4) t=.05;
            if (SVObjects[i]->Transparency==3) t=.2;
            if (SVObjects[i]->Transparency==2) t=.4;
            if (SVObjects[i]->Transparency==1) t=.6;
            glColor4f(mcolor[0], mcolor[1], mcolor[2], t);

            //RENDER STUFF!
            /*if (SVObjects[i]->UsesVBOs) // new VBO code
                  for (int j=0; j<SVObjects[i]->VBOs.count(); j++)
                  {
                     SVObjects[i]->VBOs[j]->bind();
                     glDrawArrays( GL_TRIANGLES, 0, SVObjects[i]->VBOVertexCounts[j]);
                  }
            else //compatibility fallback*/
                  for (int j=0; j<SVObjects[i]->displaylists.count(); j++)
                     glCallList(SVObjects[i]->displaylists[j]);

            glDisable (GL_COLOR_MATERIAL);
            glDisable(GL_BLEND);
            glDepthMask(true);
         }
         if (!transflag)
         if (SVObjects[i]->Transparency==0) //do normal on first run
         {

            glEnable (GL_COLOR_MATERIAL);

              glColor4f(mcolor[0], mcolor[1], mcolor[2], 1.0f);

            //RENDER STUFF!
            /*if (SVObjects[i]->UsesVBOs) // new VBO code
                  for (int j=0; j<SVObjects[i]->VBOs.count(); j++)
                  {
                     SVObjects[i]->VBOs[j]->bind();
                     glDrawArrays( GL_TRIANGLES, 0, SVObjects[i]->VBOVertexCounts[j]);
                  }
            else //compatibility fallback*/
                  for (int j=0; j<SVObjects[i]->displaylists.count(); j++)
                     glCallList(SVObjects[i]->displaylists[j]);

            glDisable (GL_COLOR_MATERIAL);

         }
      }
  }

  //glMultMatrixf(GlobalMatrix); //apply global rescaling - for VAXML primarily

  if (MainWin->ui->actionShow_Scale_Grid->isChecked()  && rightview==false)
  {


     //Grid enabled

      glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE); //turn off any anaglyph colour masking

      glLoadIdentity();
      if (rightview)
            gluLookAt(StereoSeparation*3,0,3.0,0,0,0, 0,1,0);
      else
            gluLookAt(0,0,3,0,0,0, 0,1,0);

      float factor=(float)(zoom-600)/(float)100;
      glTranslatef(0, 0, factor);

      glEnable(GL_BLEND);
      glEnable (GL_COLOR_MATERIAL);
      glDisable( GL_LIGHTING );

      //draw a huge transparent quad to partially occlude rear of specimen
      glColor4f(.2, .2, .2, .8);
      glBegin(GL_QUADS);                      // Draw A Quad
          glVertex3f(-20.0f, 20.0f, -0.0001f);              // Top Left
          glVertex3f( 20.0f, 20.0f, -0.0001f);              // Top Right
          glVertex3f( 20.0f,-20.0f, -0.0001f);              // Bottom Right
          glVertex3f(-20.0f,-20.0f, -0.0001f);              // Bottom Left
      glEnd();                            // Done Drawing The Quad

      //Now draw grid lines

      glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
      glLoadIdentity();

      //turn off clipping - needed to avoid grid being clipped
      if (MainWin->ui->actionSplit_Stereo->isChecked())
          gluPerspective(ClipAngle,(GLfloat)(xdim/2)/(GLfloat)ydim,(GLfloat)0.1,(GLfloat)200.0);
      else
          gluPerspective(ClipAngle,(GLfloat)(xdim)/(GLfloat)ydim,(GLfloat)0.1,(GLfloat)200.0);

      glMatrixMode(GL_MODELVIEW);
      gluLookAt(0,0,3,0,0,0, 0,1,0);
      glTranslatef(0, 0, factor);

      glEnable(GL_BLEND);
      glEnable (GL_COLOR_MATERIAL);
      glDisable( GL_LIGHTING );

      glMultMatrixf(GlobalMatrix); //apply global rescaling - for VAXML primarily

     float divider = 10 * globalrescale;

      glClear(GL_DEPTH_BUFFER_BIT);
      float s=1/mm_per_unit;
      glScalef(s,s,s);
      glScalef(globalrescale,globalrescale,globalrescale);

      //work out x field of view in mm
      double fov;
      fov=(float)xdim * ClipAngle/(divider*s);
      fov/=(float)ydim;
      fov/=1.344; // a fudge factor!

      //OK, work out correct scale in mm
      //Look at fov and find coarse level to use
      //fov 5mm - want coarse 1, fine 0.1
      //fov 50mm - want coarse 10, fine 1
      //fov .5mm - want coarse .1, fine .01 etc
      //work out coarse below

      float coarse=pow((double)10.0,((double)((int)(log10(fov)+.7)))); //double cast is to round it
      float fine=coarse/10;

      QString message;
      glDisable(GL_DEPTH_TEST);
      int c=0;
      float labeloffset=fine/30;
      float lineedge=100.0*fov;
      for (float i=0-(coarse*15); i<coarse*15; i+=fine)
      {
          if (c-- == 0)
          {
              c=9;
              if (MainWin->ui->actionMute_Colours->isChecked())
                  glColor4f(((float)grid_red/3+170)/255.0, ((float)grid_green/3+170)/255.0, ((float)grid_blue/3+170)/255.0, 1.0);
              else
                  glColor4f(((float)grid_red)/255.0, ((float)grid_green)/255.0, ((float)grid_blue)/255.0, 1.0);
              glBegin(GL_LINES);
              glVertex3f(i/10, lineedge, 0.0f); // origin of the line
              glVertex3f(i/10, 0-lineedge, 0.0f); // ending point of the line
              glEnd();
              glBegin(GL_LINES);
              glVertex3f(lineedge,i/10, 0.0f); // origin of the line
              glVertex3f(0-lineedge,i/10,0.0f); // ending point of the line
              glEnd();
              if ((coarse/10)>=1)
                  message.sprintf("%.0fmm",i/10);
              if ((coarse/10)>=.09 && (coarse/10)<1)
                  message.sprintf("%.1fmm",i/10);
              if ((coarse/10)>=.009 && (coarse/10)<0.1)
                  message.sprintf("%.2fmm",i/10);
              if ((coarse/10)>=.0009 && (coarse/10)<0.01)
                  message.sprintf("%.3fmm",i/10);
              if ((coarse/10)<.0009) message.sprintf("%fmm",i/10);

              renderText(labeloffset,labeloffset+i/10,0.0f,message);
              if (i<0.001 || i>-0.001) renderText(i/10 +labeloffset,labeloffset, 0.0f,message);
          }
          else
          {
              if (MainWin->ui->actionMute_Colours->isChecked())
                  glColor4f(((float)grid_minor_red/3+170)/255.0, ((float)grid_minor_green/3+170)/255.0, ((float)grid_minor_blue/3+170)/255.0, 1.0);
              else
                  glColor4f(((float)grid_minor_red)/255.0, ((float)grid_minor_green)/255.0, ((float)grid_minor_blue)/255.0, 1.0);
              glBegin(GL_LINES);
              glVertex3f(i/10, lineedge, 0.0f); // origin of the line
              glVertex3f(i/10, 0-lineedge, 0.0f); // ending point of the line
              glEnd();
              glBegin(GL_LINES);
              glVertex3f(lineedge,i/10, 0.0f); // origin of the line
              glVertex3f(0-lineedge,i/10,0.0f); // ending point of the line
              glEnd();
          }

      }
     glEnable(GL_DEPTH_TEST);
      glEnable( GL_LIGHTING );

      if (MainWin->ui->actionSplit_Stereo->isChecked())
          gluPerspective(ClipAngle,(GLfloat)(xdim/2)/(GLfloat)ydim,ClipStart,ClipDepth);
      else
           gluPerspective(ClipAngle,(GLfloat)(xdim)/(GLfloat)ydim,ClipStart,ClipDepth);
  }


  glLoadIdentity();
  glLightfv(GL_LIGHT1, GL_POSITION, LightPosition);			// Position The Light
  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);

}

void SVGlWidget::SetView(bool half)
{
    //qDebug()<<"zoom"<<zoom<<"Clipangle"<<ClipAngle;
    //set up ortho or perspective view
    float asp=(float)ydim/(float)xdim;
    float fudge=(float)1300/(float)xdim;

    if (half)
    {
        // Reset The Projection Matrix
        if (MainWin->ui->actionOrthographic_View->isChecked())
            glOrtho((0-ClipAngle/(10*fudge))/2, ClipAngle/(10*fudge)/2, (0-ClipAngle/(10*fudge))*asp, asp*ClipAngle/(10*fudge), ClipStart, ClipDepth);
        else
            gluPerspective(ClipAngle,(GLfloat)(xdim/2)/(GLfloat)ydim,ClipStart,ClipDepth);
    }
    else
    {
        // Reset The Projection Matrix
        if (MainWin->ui->actionOrthographic_View->isChecked())
            glOrtho((0-ClipAngle/(10*fudge)), ClipAngle/(10*fudge), (0-ClipAngle/(10*fudge))*asp, asp*ClipAngle/(10*fudge), ClipStart, ClipDepth);
        else
            gluPerspective(ClipAngle,(GLfloat)xdim/(GLfloat)ydim,ClipStart,ClipDepth);
    }
}

void SVGlWidget::paintGL()
{


    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate model view transformation
    QMatrix4x4 matrix;
    QMatrix4x4 mMatrix;
    //matrix.translate(0.0, 0.0, -5.0);
    //matrix.rotate(rotation);


    simple_shader.bind();
    // Set modelview-projection matrix
    simple_shader.setUniformValue("mvpMatrix", projection * matrix * mMatrix);
    simple_shader.setUniformValue("color", QColor(Qt::white));

    vertices << QVector3D(1, 0, -2) << QVector3D(0, 1, -2) << QVector3D(-1, 0, -2);

    simple_shader.setAttributeArray("vertex", vertices.constData());
    simple_shader.enableAttributeArray("vertex");

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    simple_shader.disableAttributeArray("vertex");
    simple_shader.release();

    /*if (SVObjects.count())
    if (SVObjects[0]->VBOs.count())
    {
        SVObjects[0]->VBOs[0]->bind();
        glDrawArrays( GL_TRIANGLES, 0, SVObjects[0]->VBOVertexCounts[0]);
    }*/

/*
    glClearColor((float)back_red/(float)255, (float)back_green/(float)255, (float)back_blue/(float)255, 0.5f);				// bBackground

    //set OPENGL style global matrix
    for (int i=0; i<16; i++) GlobalMatrix[i] =
            (float)(globalmatrix.data()[i]);

    if (SVObjects.count()>0) //if need to record default and not some spurious early call with no data
    {
        for (int j=0; j<SVObjects.count(); j++)
        {
            if (SVObjects[j]->gotdefaultmatrix==false)
            {
                for (int i=0; i<16; i++) SVObjects[j]->defaultmatrix[i] = SVObjects[j]->matrix[i];
                SVObjects[j]->gotdefaultmatrix=true;
                DefaultClipAngle=ClipAngle; //yes, doing for each one, for convenience
                if (j==0)
                {
                    Xrot=0; Yrot=0; Zrot=0;
                    Xtrans=0; Ytrans=0; Ztrans=0;
                }
            }

        }

    }

  if (MainWin->ui->actionSplit_Stereo->isChecked())
  {
	  glDrawBuffer(GL_BACK);
      glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
      glViewport(0,0,xdim/2,ydim);
      glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
      glLoadIdentity();
      SetView(true);
      glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
  }
  else
  if (MainWin->ui->actionAnaglyph_Stereo->isChecked()) 
  {
	  glDrawBuffer(GL_BACK);
      glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	  glColorMask(GL_TRUE, GL_FALSE, GL_FALSE, GL_TRUE);
      glViewport(0,0,xdim,ydim);
      glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
      glLoadIdentity();									// Reset The Projection Matrix
      SetView(false);
      glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
 }
  else 
  if (MainWin->ui->actionNo_Stereo->isChecked() || MainWin->ui->actionOrthographic_View->isChecked())
  {
	  glDrawBuffer(GL_BACK);
      glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	  glViewport(0,0,xdim,ydim);
      glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
      glLoadIdentity();									// Reset The Projection Matrix
      SetView(false);
      glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
 }
  else
  if (MainWin->ui->actionQuadBuffer_Stereo->isChecked()) 
  {
	  glDrawBuffer(GL_BACK_LEFT);
      glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	  glViewport(0,0,xdim,ydim);
      glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
      glLoadIdentity();									// Reset The Projection Matrix
      SetView(false);
      glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	  LastDrawQuad=true;
  }

  //Do first view
  DrawObjects(false);

  if (MainWin->ui->actionAnaglyph_Stereo->isChecked()) 
  {
	    glDrawBuffer(GL_BACK);
        glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
        glClear(GL_DEPTH_BUFFER_BIT);
        glColorMask(GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE);
        glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
        glLoadIdentity();									// Reset The Projection Matrix
        glViewport(0,0,xdim,ydim);
        SetView(false);
        glMatrixMode(GL_MODELVIEW);
        DrawObjects(true);
  }
  else if (MainWin->ui->actionSplit_Stereo->isChecked())
  {
    	  glDrawBuffer(GL_BACK);
        glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
        glLoadIdentity();									// Reset The Projection Matrix
        glViewport(xdim/2,0,xdim/2,ydim);
        SetView(true);
        glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
        DrawObjects(true);
  }
  else if(MainWin->ui->actionQuadBuffer_Stereo->isChecked())
  {
	  glDrawBuffer(GL_BACK_RIGHT);
      glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	  glViewport(0,0,xdim,ydim);
      glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
      glLoadIdentity();									// Reset The Projection Matrix
      SetView(false);
      glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
      DrawObjects(true);
  }

  glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

  int pos=2;
  foreach (QString c, i_comments)
      if (c.left(3)=="(c)" || c.left(3)=="(C)") {
      renderText(10,10*pos++,c.replace("(c)","©",Qt::CaseInsensitive)); break;}
*/
}

 void SVGlWidget::SetZoom(int Zoom)
 {
     zoom=Zoom;
 }

 void SVGlWidget::SetClip(int Start, int Depth, int Angle)
 {
     ClipStart=(float)(Start+1)/50;
     ClipDepth=ClipStart + 0.005 + ((float)Depth)/100;
     ClipAngle=((float)Angle)/10;
 }

 void SVGlWidget::Rotate(double angle)
 {
   if (MainWin->ui->actionReposition_Scale_Ball->isChecked())
   {
       glLoadIdentity();
       glRotatef((float)angle,0.0f, 0.0f, 1.0f);
       glMultMatrixf(ScaleMatrix); //apply
       if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, ScaleMatrix); //retrieve
       FileDirty=true;


       return;
   }

   for (int i=0; i<SVObjects.count(); i++)
   {
       bool f=false;
       if (MainWin->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f=true;
       if (!MainWin->ui->actionReposition_Selected->isChecked()) f=true;
       if (f)
        {
             {
                 glLoadIdentity();
                 glRotatef((float)angle,0.0f, 0.0f, 1.0f);
                 glMultMatrixf(SVObjects[i]->matrix); //apply
                 if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, SVObjects[i]->matrix); //retrieve
                 FileDirty=true;
             }
           if (i==0) Zrot+=angle;
         }
    }

 }

 void SVGlWidget::MoveAway(double dist)
 {
   if (MainWin->ui->actionReposition_Scale_Ball->isChecked())
   {
       glLoadIdentity();
       glTranslatef(0.0f, 0.0f, -dist);
       glMultMatrixf(ScaleMatrix); //apply
       if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, ScaleMatrix); //retrieve
       FileDirty=true;
       return;
   }

   for (int i=0; i<SVObjects.count(); i++)
   {
       bool f=false;
       if (MainWin->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f=true;
       if (!MainWin->ui->actionReposition_Selected->isChecked()) f=true;
       if (f)
        {
             {
                 glLoadIdentity();
                 glTranslatef(0.0f, 0.0f, -dist);
                 glMultMatrixf(SVObjects[i]->matrix); //apply
                 if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, SVObjects[i]->matrix); //retrieve
                 FileDirty=true;
             }
           if (i==0) Ztrans -=dist;

         }
    }
 }

void SVGlWidget::Resize(float value)
{
    if (MainWin->ui->actionReposition_Scale_Ball->isChecked() || (voxml_mode && MainWin->ui->actionShow_Ball_2->isChecked()))
    {
        ScaleBallScale*=value;
        glLoadIdentity();
        glScalef(value,value,value);
        glMultMatrixf(ScaleMatrix); //apply
        if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, ScaleMatrix); //retrieve
        FileDirty=true;
        return;
    }

    for (int i=0; i<SVObjects.count(); i++)
   {
       bool f=false;
       if (MainWin->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f=true;
       if (!MainWin->ui->actionReposition_Selected->isChecked()) f=true;
       if (f)
        {
             {
                 SVObjects[i]->scale*=value;
                 glLoadIdentity();
                 glScalef(value,value,value);
                 glMultMatrixf(SVObjects[i]->matrix); //apply
                 if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, SVObjects[i]->matrix); //retrieve
                 FileDirty=true;
             }
         }
    }
}

void SVGlWidget::ResizeScaleBall(float newsize)
{
    return;
    float oldsize=(ScaleBallScale/globalrescale)*mm_per_unit;

    float value=newsize/oldsize;
    ScaleBallScale*=value;
    glLoadIdentity();
    glScalef(value,value,value);
    glMultMatrixf(ScaleMatrix); //apply
    if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, ScaleMatrix); //retrieve
    FileDirty=true;
    return;
}

void SVGlWidget::ResetSize()
{
    if (MainWin->ui->actionReposition_Scale_Ball->isChecked() || (voxml_mode && MainWin->ui->actionShow_Ball_2->isChecked()))
    {
        glLoadIdentity();
        glScalef(1.0/(ScaleBallScale/globalrescale),1.0/(ScaleBallScale/globalrescale),1.0/(ScaleBallScale/globalrescale));
        glMultMatrixf(ScaleMatrix); //apply
        if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, ScaleMatrix); //retrieve
        FileDirty=true;
        ScaleBallScale=1.0/globalrescale;
        return;
    }

   for (int i=0; i<SVObjects.count(); i++)
   {
       bool f=false;
       if (MainWin->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f=true;
       if (!MainWin->ui->actionReposition_Selected->isChecked()) f=true;
       if (f)
        {
             {
                 glLoadIdentity();
                 glScalef(1.0/SVObjects[i]->scale,1.0/SVObjects[i]->scale,1.0/SVObjects[i]->scale);
                 glMultMatrixf(SVObjects[i]->matrix); //apply
                 if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, SVObjects[i]->matrix); //retrieve
                 SVObjects[i]->scale=1.0;
                 FileDirty=true;
             }
         }
    }
}

void SVGlWidget::YRotate(float angle)
 {
    if (MainWin->ui->actionReposition_Scale_Ball->isChecked())
    {
        glLoadIdentity();
        glRotatef((float)angle,0.0f, 1.0f, 0.0f);
        glMultMatrixf(ScaleMatrix); //apply
        if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, ScaleMatrix); //retrieve
        FileDirty=true;
        return;
    }
   for (int i=0; i<SVObjects.count(); i++)
   {
       bool f=false;
       if (MainWin->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f=true;
       if (!MainWin->ui->actionReposition_Selected->isChecked()) f=true;
       if (f)
        {
             {
                 glLoadIdentity();
                 glRotatef((float)angle,0.0f, 1.0f, 0.0f);
                 glMultMatrixf(SVObjects[i]->matrix); //apply
                 if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, SVObjects[i]->matrix); //retrieve
                 FileDirty=true;
             }
            if (i==0) Yrot+=angle;
         }
    }
 }

void SVGlWidget::XRotate(float angle)
 {
    if (MainWin->ui->actionReposition_Scale_Ball->isChecked())
    {
        glLoadIdentity();
        glRotatef((float)angle,1.0f, 0.0f, 0.0f);
        glMultMatrixf(ScaleMatrix); //apply
        if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, ScaleMatrix); //retrieve
        FileDirty=true;
        return;
    }
   for (int i=0; i<SVObjects.count(); i++)
   {
       bool f=false;
       if (MainWin->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f=true;
       if (!MainWin->ui->actionReposition_Selected->isChecked()) f=true;
       if (f)
        {
             {
                 glLoadIdentity();
                 glRotatef((float)angle,1.0f, 0.0f, 0.0f);
                 glMultMatrixf(SVObjects[i]->matrix); //apply
                 if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, SVObjects[i]->matrix); //retrieve
                 FileDirty=true;
             }
            if (i==0) Xrot+=angle;
         }
    }
 }

void SVGlWidget::ZRotate(float angle)
 {
    if (MainWin->ui->actionReposition_Scale_Ball->isChecked())
    {
        glLoadIdentity();
        glRotatef((float)angle,0.0f, 0.0f, 1.0f);
        glMultMatrixf(ScaleMatrix); //apply
        if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, ScaleMatrix); //retrieve
        FileDirty=true;
        return;
    }
   for (int i=0; i<SVObjects.count(); i++)
   {
       bool f=false;
       if (MainWin->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f=true;
       if (!MainWin->ui->actionReposition_Selected->isChecked()) f=true;
       if (f)
        {
             {
                 glLoadIdentity();
                 glRotatef((float)angle,0.0f, 0.0f, 1.0f);
                 glMultMatrixf(SVObjects[i]->matrix); //apply
                 if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, SVObjects[i]->matrix); //retrieve
                 FileDirty=true;
             }
            if (i==0) Zrot+=angle;
         }
    }
 }

void SVGlWidget::Translate(float x, float y, float z)
{
   if (MainWin->ui->actionReposition_Scale_Ball->isChecked())
   {
       glLoadIdentity();
       glTranslatef(x/10.0,y/10.0,z/10.0);  //the 10.0 is a fudge factor!
       glMultMatrixf(ScaleMatrix); //apply
       if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, ScaleMatrix); //retrieve
       FileDirty=true;
       return;
   }
  for (int i=0; i<SVObjects.count(); i++)
  {
      bool f=false;
      if (MainWin->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f=true;
      if (!MainWin->ui->actionReposition_Selected->isChecked()) f=true;
      if (f)
       {
            {
                glLoadIdentity();
               glTranslatef(x/10.0,y/10.0,z/10.0); //the 10.0 is a fudge factor!
                glMultMatrixf(SVObjects[i]->matrix); //apply
                if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, SVObjects[i]->matrix); //retrieve
                FileDirty=true;
            }
           if (i==0) { Xtrans += x; Ytrans += y; Ztrans += z;}
        }
   }
}


 void SVGlWidget::mouseMoveEvent(QMouseEvent *event)
{
  return;
  bool donesomething=false;
  bool rotmode=false;
  if (event->buttons() & Qt::RightButton || ((event->buttons() & Qt::LeftButton) && event->modifiers()==Qt::CTRL) || ((event->buttons() & Qt::LeftButton) && MainWin->ui->actionRotate_Lock->isChecked()))
  rotmode=true;

      //If shift held - rotate round center
  if ((rotmode && event->modifiers()==Qt::SHIFT)  || (event->modifiers()==Qt::ALT && event->buttons() & Qt::LeftButton) )
      {
          //rotate amount is angular distance between prev and now
          double before=atan2((double)(LastMouseXpos-xdim/2),(double)(LastMouseYpos-ydim/2));
          double after=atan2((double)(event->x()-xdim/2),(double)(event->y()-ydim/2));
          double amount;
          //handle wrapping
          double amount1=after-before;
          double amount2=after+2*3.14159-before;
          if (abs(amount2)<abs(amount1)) amount=amount2; else amount=amount1;
          if (amount>.1) amount=.1;
          Rotate(amount*70);
          donesomething=true;
    }
    else if (rotmode)
    {
          float yangle = ((float) (event->x()-LastMouseXpos))/SENSITIVITY;
          float xangle = ((float) (event->y()-LastMouseYpos))/SENSITIVITY;

          //add 'em to matrix
          if (MainWin->ui->actionReposition_Scale_Ball->isChecked())
          {
              glLoadIdentity();
              glRotatef(xangle,1.0f, 0.0f, 0.0f);
              glRotatef(yangle,0.0f, 1.0f, 0.0f);
              glMultMatrixf(ScaleMatrix); //apply
              if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, ScaleMatrix); //retrieve
             donesomething=true;
             FileDirty=true;
          }
          else
           for (int i=0; i<SVObjects.count(); i++)
           {
               bool f=false;
               if (MainWin->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f=true;
               if (!MainWin->ui->actionReposition_Selected->isChecked()) f=true;
               if (f)
                {
                     {
                         glLoadIdentity();
                         glRotatef(xangle,1.0f, 0.0f, 0.0f);
                         glRotatef(yangle,0.0f, 1.0f, 0.0f);
                         if (i==0) { Xrot+=xangle; Yrot+=yangle; }
                         glMultMatrixf(SVObjects[i]->matrix); //apply
                         if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, SVObjects[i]->matrix); //retrieve
                         donesomething=true;
                         FileDirty=true;
                     }
                 }
            }
      }

   if (!rotmode && event->buttons() & Qt::LeftButton && donesomething==false)
   {
       float ObjXpos=((float)(event->x()-LastMouseXpos))/(SENSITIVITY*100);
       float ObjYpos=0-((float)(event->y()-LastMouseYpos))/(SENSITIVITY*100);

       if (MainWin->ui->actionReposition_Scale_Ball->isChecked())
       {
           glLoadIdentity();
           glTranslatef(ObjXpos, ObjYpos, 0.0f);
           glMultMatrixf(ScaleMatrix); //apply
           if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, ScaleMatrix); //retrieve
           FileDirty=true;
           donesomething=true;
      }
       else
       for (int i=0; i<SVObjects.count(); i++)
       {
           bool f=false;
           if (MainWin->ui->actionReposition_Selected->isChecked() && (SVObjects[i]->widgetitem->isSelected())) f=true;
           if (!MainWin->ui->actionReposition_Selected->isChecked()) f=true;
           if (f)
            {
                 {
                     glLoadIdentity();
                     glTranslatef(ObjXpos, ObjYpos, 0.0f);
                     if (i==0) { Xtrans+=ObjXpos; Ytrans+=ObjYpos;}
                     //glMultMatrixf(ViewMatrix); //apply
                     glMultMatrixf(SVObjects[i]->matrix); //apply
                     if (!SP2_lock) glGetFloatv(GL_MODELVIEW_MATRIX, SVObjects[i]->matrix); //retrieve
                     donesomething=true;
                     FileDirty=true;
                 }
             }
        }
   }
    LastMouseXpos=event->x();
    LastMouseYpos=event->y();

    if (donesomething) updateGL();
}


void SVGlWidget::SetStereoSeparation(float s)
{
    StereoSeparation=s;
}

void SVGlWidget::ResetToDefault()
{
    for (int j=0; j<SVObjects.count(); j++)
    {
        if (SVObjects[j]->gotdefaultmatrix)
            for (int i=0; i<16; i++) SVObjects[j]->matrix[i] = SVObjects[j]->defaultmatrix[i];
    }

    ClipAngle=DefaultClipAngle;
    Xrot=0; Yrot=0; Zrot=0;
    Xtrans=0; Ytrans=0; Ztrans=0;
    MainWin->ui->ClipAngle->setValue(ClipAngle*10); //should trigger a refresh? not if no change!
}

void SVGlWidget::NewDefault()
{
    if (SVObjects.count()>0) //if need to record default and not some spurious early call with no data
    {
        for (int j=0; j<SVObjects.count(); j++)
        {
            for (int i=0; i<16; i++) SVObjects[j]->defaultmatrix[i] = SVObjects[j]->matrix[i];
        }
        DefaultClipAngle=ClipAngle; //yes, doing for each one, for convenience
        Xrot=0; Yrot=0; Zrot=0;
        Xtrans=0; Ytrans=0; Ztrans=0;
    }
}
