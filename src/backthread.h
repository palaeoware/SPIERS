#ifndef __BACKTHREAD_H__
#define __BACKTHREAD_H__

#include <QThread>

 class MyThread : public QThread
 {
 private:
 Q_OBJECT
 public:
     void run();
 private slots:
	void TimerFired();
	bool TryCaching(int fnumber);
};
 
extern MyThread* BackThread;
#endif // __BACKTHREAD_H__
