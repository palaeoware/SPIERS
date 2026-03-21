// MLUpdateBlockingDialog.h

#ifndef MLUPDATEBLOCKINGDIALOG_H
#define MLUPDATEBLOCKINGDIALOG_H

#include <QDialog>

class QLabel;

class MLUpdateBlockingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MLUpdateBlockingDialog(QWidget *parent = nullptr);

    static void showDialog(QWidget *parent,
                           const QString &highLevelText,
                           const QString &detailText = QString(),
                           const QString &windowTitle = "Working");

    static void updateHighLevelText(const QString &text);
    static void updateDetailText(const QString &text);
    static void hideDialog();
    static bool isCancelled();

private:
    QLabel *m_highLevelLabel;
    QLabel *m_detailLabel;
    bool cancelled;
    static MLUpdateBlockingDialog *s_instance;
    static int s_animationState;
    static QString s_highLevelBaseText;
    static QString s_detailText;

    void setHighLevelStatusText(const QString &text);
    void setDetailStatusText(const QString &text);

    void refreshAndProcessEvents();
    void applyCurrentTexts();
    void centerOverParent();
    
    static QString animatedText(const QString &baseText);

private slots:
    void Cancelled();
};

#endif // MLUPDATEBLOCKINGDIALOG_H
