// MLUpdateBlockingDialog.cpp

//A ChatGPT special!

#include "MLUpdateBlockingDialog.h"

#include <QCoreApplication>
#include <QEventLoop>
#include <QFont>
#include <QLabel>
#include <QPoint>
#include <QRect>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>

MLUpdateBlockingDialog *MLUpdateBlockingDialog::s_instance = nullptr;
int MLUpdateBlockingDialog::s_animationState = 0;
QString MLUpdateBlockingDialog::s_highLevelBaseText;
QString MLUpdateBlockingDialog::s_detailText;

MLUpdateBlockingDialog::MLUpdateBlockingDialog(QWidget *parent)
    : QDialog(parent),
    m_highLevelLabel(new QLabel(this)),
    m_detailLabel(new QLabel(this))
{
    setWindowTitle("Working");

    setModal(true);
    setWindowModality(Qt::ApplicationModal);

    // Remove close/help buttons, and keep the dialog above its parent.
    setWindowFlags(windowFlags()
                       & ~Qt::WindowCloseButtonHint
                       & ~Qt::WindowContextHelpButtonHint
                   | Qt::WindowStaysOnTopHint);

    m_highLevelLabel->setWordWrap(true);
    m_highLevelLabel->setMinimumWidth(420);
    m_highLevelLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QFont highFont = m_highLevelLabel->font();
    highFont.setPointSize(highFont.pointSize() + 1);
    highFont.setBold(true);
    m_highLevelLabel->setFont(highFont);

    m_detailLabel->setWordWrap(true);
    m_detailLabel->setMinimumWidth(420);
    m_detailLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    QPushButton *cancelBtn = new QPushButton("Cancel", this);

    connect(cancelBtn, &QPushButton::clicked,
            this, &MLUpdateBlockingDialog::Cancelled);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_highLevelLabel);
    layout->addWidget(m_detailLabel);
    layout->addWidget(cancelBtn);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(10);

    setLayout(layout);

    m_highLevelLabel->setText("Working.");
    m_detailLabel->setText(QString());

    adjustSize();
    setFixedSize(sizeHint());
}

void MLUpdateBlockingDialog::setHighLevelStatusText(const QString &text)
{
    m_highLevelLabel->setText(text);
}

void MLUpdateBlockingDialog::setDetailStatusText(const QString &text)
{
    m_detailLabel->setText(text);
}

QString MLUpdateBlockingDialog::animatedText(const QString &baseText)
{
    QString dots;

    switch (s_animationState)
    {
    case 0: dots = ".";   break;
    case 1: dots = "..";  break;
    case 2: dots = "..."; break;
    default: dots = "";   break;
    }

    s_animationState = (s_animationState + 1) % 4;
    return baseText + dots;
}

void MLUpdateBlockingDialog::Cancelled()
{
    qDebug()<<"cancelled";
    cancelled = true;
}

void MLUpdateBlockingDialog::applyCurrentTexts()
{
    setHighLevelStatusText(animatedText(s_highLevelBaseText));
    setDetailStatusText(s_detailText);
}

void MLUpdateBlockingDialog::refreshAndProcessEvents()
{
    applyCurrentTexts();

    m_highLevelLabel->repaint();
    m_detailLabel->repaint();
    repaint();

    QCoreApplication::processEvents(); //QEventLoop::ExcludeUserInputEvents
}

void MLUpdateBlockingDialog::centerOverParent()
{
    QWidget *p = parentWidget();
    if (!p)
        return;

    QRect parentGeom = p->frameGeometry();
    QPoint center = parentGeom.center();

    move(center.x() - width() / 2,
         center.y() - height() / 2);
}

void MLUpdateBlockingDialog::showDialog(QWidget *parent,
                                        const QString &highLevelText,
                                        const QString &detailText,
                                        const QString &windowTitle)
{
    if (!s_instance)
        s_instance = new MLUpdateBlockingDialog(parent);

    s_instance->setWindowTitle(windowTitle);

    s_animationState = 0;
    s_highLevelBaseText = highLevelText;
    s_detailText = detailText;

    s_instance->adjustSize();
    s_instance->centerOverParent();

    s_instance->show();
    s_instance->raise();
    s_instance->activateWindow();

    s_instance->cancelled = false;
    s_instance->refreshAndProcessEvents();
}

void MLUpdateBlockingDialog::updateHighLevelText(const QString &text)
{
    if (!s_instance)
        return;

    s_highLevelBaseText = text;
    s_instance->refreshAndProcessEvents();
}

void MLUpdateBlockingDialog::updateDetailText(const QString &text)
{
    if (!s_instance)
        return;

    s_detailText = text;
    s_instance->refreshAndProcessEvents();
}

void MLUpdateBlockingDialog::hideDialog()
{
    if (!s_instance)
        return;

    s_instance->hide();
    delete s_instance;
    s_instance = nullptr;

    s_highLevelBaseText.clear();
    s_detailText.clear();
    s_animationState = 0;

    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

bool MLUpdateBlockingDialog::isCancelled()
{
    if (!s_instance)
        return false;

    return s_instance->cancelled;

}
