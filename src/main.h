#ifndef MAIN_H
#define MAIN_H
#include <QApplication>
#include <QString>
#include <QEvent>
#include <QKeyEvent>

class myapp : public QApplication
{
    Q_OBJECT
public:
    myapp(int &argc, char *argv[]);

    QString fn;
    bool namereceived;
    bool donthandlefileevent;


private:
    bool event(QEvent *);
};

#endif // MAIN_H
