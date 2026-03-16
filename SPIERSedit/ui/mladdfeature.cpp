#include "mladdfeature.h"
#include "ui_mladdfeature.h"
#include "src/mlfeature.h"

#include <QList>
#include "src/mlfeaturecontrast.h"
#include "src/mlfeaturedifferenceofgaussians.h"
#include "src/mlfeatureintensity.h"
#include "src/mlfeaturegaussian.h"
#include "src/mlfeaturegradient.h"
#include "src/mlfeaturevariance.h"
#include "src/mlfeaturelog.h"
#include "src/mlfeaturehessian.h"

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
    int arg1 = ui->cmbArg1->currentData().toInt();
    int arg2 = ui->cmbArg2->currentData().toInt();
    bool is3D = ui->chk3D->isChecked();
    MLFeature::FeatureType type = dummy->GetType();

    return MLFeature::CreateFromData(
        type, channel, is3D, arg1, arg2);

}

void MLAddFeature::PopulateCombos()
{
    //Make dummy features, to get at their methods
    dummyFeatures.append(new MLFeatureIntensity(MLFeature::Channel::Intensity));
    dummyFeatures.append(new MLFeatureGaussian(MLFeature::Channel::Intensity, false, 1));
    dummyFeatures.append(new MLFeatureContrast(MLFeature::Channel::Intensity, false, 1));
    dummyFeatures.append(new MLFeatureDifferenceOfGaussians(MLFeature::Channel::Intensity,false, 2,1));
    dummyFeatures.append(new MLFeatureGradient(MLFeature::Channel::Intensity,false, 2));
    dummyFeatures.append(new MLFeatureVariance(MLFeature::Channel::Intensity,false, 2));
    dummyFeatures.append(new MLFeatureLoG(MLFeature::Channel::Intensity,false, 2));
    dummyFeatures.append(new MLFeatureHessian(MLFeature::Channel::Intensity,false, 2, MLFeatureHessian::HessianMode::Determinant));

    for (int i=0; i<dummyFeatures.count(); i++)
    {
        ui->cmbType->addItem(dummyFeatures[i]->GetPrettyName(),i);
    }

    for (int i=0; i<6; i++)
    {
        ui->cmbChannel->addItem(MLFeature::GetPrettyChannel((MLFeature::Channel)i),i);
    }


}



void MLAddFeature::Refresh() //changed type
{
    int dummyIndex = ui->cmbType->currentData().toInt();
    MLFeature *feature = dummyFeatures[dummyIndex];

    if (feature->GetPretty3D()=="") //means no 2D/3D
        ui->chk3D->hide();
    else
        ui->chk3D->show();

    int arg1min = feature->GetMinMaxForArgs(1,false);
    int arg2min = feature->GetMinMaxForArgs(2,false);
    int arg1max = feature->GetMinMaxForArgs(1,true);
    int arg2max = feature->GetMinMaxForArgs(2,true);

    ui->cmbArg1->clear();
    for (int i=arg1min; i<=arg1max; i++)
    {
        ui->cmbArg1->addItem(feature->GetArg1SetupString(i),i);
    }

    ui->cmbArg2->clear();
    for (int i=arg2min; i<=arg2max; i++)
    {
        ui->cmbArg2->addItem(feature->GetArg2SetupString(i),i);
    }

    if (arg1max==arg1min)
        ui->cmbArg1->hide();
    else
        ui->cmbArg1->show();

    if (arg2max==arg2min)
        ui->cmbArg2->hide();
    else
        ui->cmbArg2->show();

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

