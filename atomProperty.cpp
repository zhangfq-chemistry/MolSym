#include "atomProperty.h"

#include "mainwindow.h"
#include "mTools.h"

#include <QWidget>

#include <QColorDialog>
#include <QTimer>

atomProperty::atomProperty(QWidget * parent)  : QDialog(parent)
{
    ui.setupUi(this);
    ui.dialAtomRadius->setRange(1,600);
}

void atomProperty::setParent (MainWindow * p)
{
    m_parent=p;
    mol=m_parent->getMol();
    view3d=m_parent->getView3D();
    initial();
}

void atomProperty::initial()
{
    idSelectedAtom=view3d->getSelectedAtomId(0);
    radius0=mol->getAtomRadiusbyIndex(idSelectedAtom);

    color0=255.0*MakeGlColor(mol->atomList[idSelectedAtom]->Color());

    color=color0;
    radius=radius0;

    disconnect(ui.dialAtomRadius, SIGNAL(valueChanged(int)),
               this,SLOT(on_dialAtomRadius_valueChanged(int)));

    disconnect(ui.doubleSpinBoxRadius, SIGNAL(valueChanged(double)),
               this,SLOT(on_doubleSpinBoxRadius_valueChanged(double)));

    ui.dialAtomRadius->setValue(200*radius);
    ui.doubleSpinBoxRadius->setValue(radius);

    connect(ui.dialAtomRadius, SIGNAL(valueChanged(int)),
            this,SLOT(on_dialAtomRadius_valueChanged(int)));

    connect(ui.doubleSpinBoxRadius, SIGNAL(valueChanged(double)),
               this,SLOT(on_doubleSpinBoxRadius_valueChanged(double)));

    ui.spinBox_red->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");
    ui.spinBox_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");
    ui.spinBox_green->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");

    QString s=tr("background-color: rgb(%1,%2,%3);").arg(color.x()).arg(color.y()).arg(color.z());
    ui.pushButton_color->setStyleSheet(s);

    disconnect(ui.spinBox_red, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_red_valueChanged(int)));
    disconnect(ui.spinBox_green, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_green_valueChanged(int)));
    disconnect(ui.spinBox_blue, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_blue_valueChanged(int)));
    ui.spinBox_red->setValue(color.x());
    ui.spinBox_green->setValue(color.y());
    ui.spinBox_blue->setValue(color.z());

    connect(ui.spinBox_red, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_red_valueChanged(int)));
    connect(ui.spinBox_green, SIGNAL(valueChanged(int)),
                this,SLOT(on_spinBox_green_valueChanged(int)));
    connect(ui.spinBox_blue, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_blue_valueChanged(int)));
}


void atomProperty::on_dialAtomScale_valueChanged(int value)
{
    radius=value/200.0;
    mol->setAtomRadius(idSelectedAtom,radius);
    view3d->renderMol(mol);
}


void atomProperty::on_pushButton_cancel_clicked()
{
    mol->setAtomRadius(idSelectedAtom,radius0);
    mol->setAtomColor(idSelectedAtom, buildColor1(color0));

    close();
}


void atomProperty::on_pushButton_ok_clicked()
{
    color.SetX(ui.spinBox_red->value());
    color.SetY(ui.spinBox_green->value());
    color.SetZ(ui.spinBox_blue->value());

    radius=ui.dialAtomRadius->value()/200.0;

    mol->setAtomRadius(idSelectedAtom,radius);
    mol->setAtomColor(idSelectedAtom, buildColor1(color));

    view3d->renderMol(mol);

    close();
}




void atomProperty::on_pushButton_color_clicked()
{
    QColor c = QColorDialog::getColor(Qt::white,nullptr);

    //uint32_t dw=0;
    //dw+=(color.red()<<16);
    //dw+=(color.green()<<8);
    //dw+=(color.blue());

    color.Set(c.red(),c.green(),c.blue());


    QString s=tr("background-color: rgb(%1,%2,%3);").arg(color.x()).arg(color.y()).arg(color.z());
    ui.pushButton_color->setStyleSheet(s);


    disconnect(ui.spinBox_red, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_red_valueChanged(int)));
    disconnect(ui.spinBox_green, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_green_valueChanged(int)));
    disconnect(ui.spinBox_blue, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_blue_valueChanged(int)));


    ui.spinBox_red->setValue(color.x());
    ui.spinBox_green->setValue(color.y());
    ui.spinBox_blue->setValue(color.z());


    connect(ui.spinBox_red, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_red_valueChanged(int)));
    connect(ui.spinBox_green, SIGNAL(valueChanged((int))),
               this,SLOT(on_spinBox_green_valueChanged(int)));
    connect(ui.spinBox_blue, SIGNAL(valueChanged((int))),
               this,SLOT(on_spinBox_blue_valueChanged(int)));

}



void atomProperty::on_spinBox_red_valueChanged(int arg1)
{
    color.Set(ui.spinBox_red->value(),
              ui.spinBox_green->value(),
              ui.spinBox_blue->value());

    QString s=tr("background-color: rgb(%1,%2,%3);").arg(color.x()).arg(color.y()).arg(color.z());
    ui.pushButton_color->setStyleSheet(s);

    mol->setAtomColor(idSelectedAtom, buildColor1(color));
    view3d->renderMol(mol);
}

void atomProperty::on_spinBox_green_valueChanged(int arg1)
{
    color.Set(ui.spinBox_red->value(),
              ui.spinBox_green->value(),
              ui.spinBox_blue->value());

    QString s=tr("background-color: rgb(%1,%2,%3);").arg(color.x()).arg(color.y()).arg(color.z());
    ui.pushButton_color->setStyleSheet(s);

    mol->setAtomColor(idSelectedAtom, buildColor1(color));
    view3d->renderMol(mol);
}

void atomProperty::on_spinBox_blue_valueChanged(int arg1)
{
    color.Set(ui.spinBox_red->value(),
              ui.spinBox_green->value(),
              ui.spinBox_blue->value());

    QString s=tr("background-color: rgb(%1,%2,%3);").arg(color.x()).arg(color.y()).arg(color.z());
    ui.pushButton_color->setStyleSheet(s);

    mol->setAtomColor(idSelectedAtom, buildColor1(color));
    view3d->renderMol(mol);
}


void atomProperty::on_doubleSpinBoxRadius_valueChanged(double arg1)
{
    radius=arg1;

    disconnect(ui.dialAtomRadius, SIGNAL(valueChanged(int)),
               this,SLOT(on_dialAtomRadius_valueChanged(int)));

    ui.dialAtomRadius->setValue(round(200.0*radius));

    connect(ui.dialAtomRadius, SIGNAL(valueChanged(int)),
            this,SLOT(on_dialAtomRadius_valueChanged(int)));

    mol->setAtomRadius(idSelectedAtom,radius);
    view3d->renderMol(mol);
}


void atomProperty::on_dialAtomRadius_valueChanged(int value)
{
    radius=value/200.0;
    disconnect(ui.doubleSpinBoxRadius, SIGNAL(valueChanged(double)),
               this,SLOT(on_doubleSpinBoxRadius_valueChanged(double)));

    ui.doubleSpinBoxRadius->setValue(radius);

    disconnect(ui.doubleSpinBoxRadius, SIGNAL(valueChanged(double)),
               this,SLOT(on_doubleSpinBoxRadius_valueChanged(double)));

    mol->setAtomRadius(idSelectedAtom,radius);
    view3d->renderMol(mol);
}
