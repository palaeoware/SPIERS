#ifndef __BRUSH_H__
#define __BRUSH_H__

#include <QList>
#include <QGraphicsLineItem>

class Brush_class
{
public:
	Brush_class();
	~Brush_class();
	void resize(int size, int shape, double o);
	bool draw(int x, int y); 
	void brighten(int x, int y, int segment, int effect);
	void lock(int x, int y, int effect); //effect is true or false in this case
	void mask(int x, int y, int mask); 
	void Brush_Flag_Restart();
	void segment(int x, int y, int effect);
	void recalc(int x, int y, int segment);
	//more to follow!
	int PixelCount; //applies to all three lists above
private:
	QList <int> Xpos;
	QList <int> Ypos;
	QList <double> Dist;
	QList <int> Soft;
	int Radius;
	QList <int> LeftLinesY;		
	QList <int> RightLinesY;		
	QList <int> TopLinesY;		
	QList <int> BottomLinesY;
	QList <int> LeftLinesX;		
	QList <int> RightLinesX;		
	QList <int> TopLinesX;		
	QList <int> BottomLinesX;
	int LeftLinesCount,RightLinesCount,TopLinesCount,BottomLinesCount;
	bool RestartFlag;
	QList <QGraphicsLineItem *> LineList;
	void ThreeDBrushPixels(QList <int> *X, QList <int> *Y, int *PointCount, double offset, int mode);
};

extern Brush_class Brush;

#endif // __BRUSH_H__
