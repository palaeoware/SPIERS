#ifndef KEYSAFESPINBOX_H
#define KEYSAFESPINBOX_H

#include <QSpinBox>

class KeysafeSpinBox : public QSpinBox
{
Q_OBJECT
public:
    explicit KeysafeSpinBox(QWidget *parent = 0);

signals:

public slots:

protected:
        bool event(QEvent *event);
};

#endif // KEYSAFESPINBOX_H
