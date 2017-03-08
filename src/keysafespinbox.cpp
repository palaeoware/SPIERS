#include "keysafespinbox.h"
#include <QEvent>
#include <QKeyEvent>

#include <QDebug>

KeysafeSpinBox::KeysafeSpinBox(QWidget *parent) :
    QSpinBox(parent)
{
}

//I don't know why the following works - but now all gets passed through properly!
bool KeysafeSpinBox::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QKeyEvent *ke = (QKeyEvent *)event;
        if (ke->key() == Qt::Key_Space)
        {
            return true;
        }
    }
    return QWidget::event(event);
}
