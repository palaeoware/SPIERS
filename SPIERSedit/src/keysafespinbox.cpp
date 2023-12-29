/**
 * @file
 * Source: KeySafeSpinBox
 *
 * All SPIERSversion code is released under the GNU General Public License.
 * See LICENSE.md files in the programme directory.
 *
 * All SPIERSversion code is Copyright 2008-2023 by Mark D. Sutton, Russell J. Garwood,
 * and Alan R.T. Spencer.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY.
 */

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

        QKeyEvent *ke = static_cast<QKeyEvent *>(event);
        if (ke->key() == Qt::Key_Space)
        {
            return true;
        }
    }
    return QWidget::event(event);
}
