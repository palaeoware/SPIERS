#ifndef MLADDFEATURE_H
#define MLADDFEATURE_H

#include <QDialog>

namespace Ui {
class MLAddFeature;
}

class MLFeature;

class MLAddFeature : public QDialog
{
    Q_OBJECT

public:
    explicit MLAddFeature(QWidget *parent = nullptr);
    ~MLAddFeature();

    void Show();
    MLFeature *GetResult();
private slots:
    void on_buttonBox_accepted();

    void Refresh();
private:
    Ui::MLAddFeature *ui;
    bool resultValid;
    void PopulateCombos();
    QList<MLFeature *> dummyFeatures;
    MLFeature * CreateNewFeature();
};

#endif // MLADDFEATURE_H
