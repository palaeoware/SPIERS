#include "mladdfeature.h"
#include "ui_mladdfeature.h"
#include "src/mlfeature.h"

#include <QList>
#include "src/mlfeaturecontrast.h"
#include "src/mlfeaturedifferenceofgaussians.h"
#include "src/mlfeatureintensity.h"
#include "src/mlfeaturegaussian.h"
#include "src/mlfeaturemean.h"
#include "src/mlfeaturegradient.h"

MLAddFeature::MLAddFeature(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MLAddFeature)
{
    ui->setupUi(this);

    PopulateCombos();
    connect(ui->cmbType,
            &QComboBox::currentIndexChanged,
            this,
            &MLAddFeature::Refresh);
    connect(ui->spinBoxArg1,
            &QSpinBox::valueChanged,
            this,
            &MLAddFeature::Refresh);
    connect(ui->spinBoxArg2,
            &QSpinBox::valueChanged,
            this,
            &MLAddFeature::Refresh);

    Refresh();
}

MLAddFeature::~MLAddFeature()
{
    delete ui;
}

void MLAddFeature::on_buttonBox_accepted()
{

    resultValid=true;
    hide();
}

MLFeature *MLAddFeature::CreateNewFeature()
{
    int dummyIndex = ui->cmbType->currentData().toInt();
    MLFeature *dummy = dummyFeatures[dummyIndex];

    MLFeature::Channel channel = (MLFeature::Channel)ui->cmbChannel->currentData().toInt();
    int arg1 = ui->spinBoxArg1->value();
    int arg2 = ui->spinBoxArg2->value();
    bool is3D = ui->chk3D->isChecked();
    MLFeature::FeatureType type = dummy->GetType();

    return MLFeature::CreateFromData(
        type, channel, is3D, arg1, arg2);

}

void MLAddFeature::PopulateCombos()
{
    //Make dummy features, to get at their methods
    dummyFeatures.append(new MLFeatureIntensity(MLFeature::Channel::Intensity));
    dummyFeatures.append(new MLFeatureMean(MLFeature::Channel::Intensity,false,1));
    dummyFeatures.append(new MLFeatureGaussian(MLFeature::Channel::Intensity, false, 1));
    dummyFeatures.append(new MLFeatureContrast(MLFeature::Channel::Intensity, false, 1));
    dummyFeatures.append(new MLFeatureDifferenceOfGaussians(MLFeature::Channel::Intensity,false, 2,1));
    dummyFeatures.append(new MLFeatureGradient(MLFeature::Channel::Intensity,false, 2));

    for (int i=0; i<dummyFeatures.count(); i++)
    {
        ui->cmbType->addItem(dummyFeatures[i]->GetPrettyName(),i);
    }

    for (int i=0; i<6; i++)
    {
        ui->cmbChannel->addItem(MLFeature::GetPrettyChannel((MLFeature::Channel)i),i);
    }


}



void MLAddFeature::Refresh()
{
    int dummyIndex = ui->cmbType->currentData().toInt();
    MLFeature *feature = dummyFeatures[dummyIndex];

    if (feature->GetPretty3D()=="") //means no 2D/3D
        ui->chk3D->hide();
    else
        ui->chk3D->show();

    QString arg1String = feature->GetArg1SetupString(ui->spinBoxArg1->value());
    QString arg2String = feature->GetArg2SetupString(ui->spinBoxArg2->value());

    if (arg1String=="")
    {
        ui->spinBoxArg1->hide();
        ui->lblArg1->hide();
    }
    else
    {
        ui->spinBoxArg1->show();
        ui->lblArg1->show();
        ui->lblArg1->setText(arg1String);
    }

    if (arg2String=="")
    {
        ui->spinBoxArg2->hide();
        ui->lblArg2->hide();
    }
    else
    {
        ui->spinBoxArg2->show();
        ui->lblArg2->show();
        ui->lblArg2->setText(arg2String);
    }

}


void MLAddFeature::Show()
{
    resultValid=false;
    exec();
}

MLFeature *MLAddFeature::GetResult()
{
    if (resultValid)
        return CreateNewFeature();
    else
        return nullptr;
}

