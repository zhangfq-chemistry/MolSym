#include "ringProperty.h"
#include "mainwindow.h"
#include "mTools.h"
#include "QColorDialog"

ringProperty::ringProperty(QWidget * parent)  : QDialog(parent)
{
    ui.setupUi(this);
    ui.dialRadius->setRange(1,200);
    ui.dialSize->setRange(1,200);
}

void ringProperty::setParent (MainWindow * p)
{
    m_parent=p;
    mol=m_parent->getMol();
    view3d=m_parent->getView3D();
    initial();
}


void ringProperty::initial()
{
    id=view3d->getSelectedRingId(0);
    size0=mol->getRingbyId(id)->radius;
    radius_scale0=mol->getRingbyId(id)->scale;
    color0=255.0*mol->getRingbyId(id)->color;
    opacity0=255.0*mol->getRingbyId(id)->opacity;

    radius_scale=radius_scale0;
    color=color0;
    opacity=opacity0;


    disconnect(ui.spinBox_red, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_red_valueChanged(int)));
    disconnect(ui.spinBox_green, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_green_valueChanged(int)));
    disconnect(ui.spinBox_blue, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_blue_valueChanged(int)));

    disconnect(ui.dialSize, SIGNAL(valueChanged(int)),
               this,SLOT(on_dialSize_valueChanged(int)));

    disconnect(ui.dialRadius, SIGNAL(valueChanged(int)),
               this,SLOT(on_dialRadius_valueChanged(int)));

    ui.dialSize->setValue(size0*50);
    ui.dialRadius->setValue(radius_scale*50);
    ui.spinBox_red->setValue(color0.x());
    ui.spinBox_green->setValue(color0.y());
    ui.spinBox_blue->setValue(color0.z());

    connect(ui.spinBox_red, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_red_valueChanged(int)));
    connect(ui.spinBox_green, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_green_valueChanged(int)));
    connect(ui.spinBox_blue, SIGNAL(valueChanged(int)),
               this,SLOT(on_spinBox_blue_valueChanged(int)));

    connect(ui.dialSize, SIGNAL(valueChanged(int)),
               this,SLOT(on_dialSize_valueChanged(int)));

    connect(ui.dialRadius, SIGNAL(valueChanged(int)),
               this,SLOT(on_dialRadius_valueChanged(int)));

    QString s=tr("background-color: rgb(%1,%2,%3);").arg(color.x()).arg(color.y()).arg(color.z());
    ui.pushButton_color->setStyleSheet(s);
}


void ringProperty::on_spinBox_red_valueChanged(int arg1)
{
    color.Set(ui.spinBox_red->value(),
              ui.spinBox_green->value(),
              ui.spinBox_blue->value());

    QString s=tr("background-color: rgb(%1,%2,%3);").arg(color.x()).arg(color.y()).arg(color.z());
    ui.pushButton_color->setStyleSheet(s);
}


void ringProperty::on_spinBox_green_valueChanged(int arg1)
{
    color.Set(ui.spinBox_red->value(),
              ui.spinBox_green->value(),
              ui.spinBox_blue->value());

    QString s=tr("background-color: rgb(%1,%2,%3);").arg(color.x()).arg(color.y()).arg(color.z());
    ui.pushButton_color->setStyleSheet(s);
}

void ringProperty::on_spinBox_blue_valueChanged(int arg1)
{
    color.Set(ui.spinBox_red->value(),
              ui.spinBox_green->value(),
              ui.spinBox_blue->value());

    QString s=tr("background-color: rgb(%1,%2,%3);").arg(color.x()).arg(color.y()).arg(color.z());
    ui.pushButton_color->setStyleSheet(s);

    mol->getRingbyId(id)->color=color/255.0;
}

void ringProperty::on_pushButton_cancel_clicked()
{
    mol->getRingbyId(id)->color=color0*255.0;
    mol->getRingbyId(id)->scale=radius_scale0;
    mol->getRingbyId(id)->radius=size0;
    mol->getRingbyId(id)->color=color0/255.0;

    view3d->releaseAllActors();
    view3d->updateMol();
    close();
}

void ringProperty::on_pushButton_ok_clicked()
{
    view3d->releaseAllActors();
    view3d->updateMol();
    view3d->push2Stack("modify ring");
    close();
}

void ringProperty::on_dialRadius_valueChanged(int value)
{
    radius_scale=ui.dialRadius->value()/50.0;
    mol->getRingbyId(id)->scale=radius_scale;
    view3d->updateMol();
}

void ringProperty::on_dialSize_valueChanged(int value)
{
    size=ui.dialSize->value()/50.0;

    mol->getRingbyId(id)->radius=size;
    view3d->updateMol();
}

void ringProperty::on_pushButton_color_clicked()
{
    QColor c = QColorDialog::getColor(Qt::white,nullptr);


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

    mol->getRingbyId(id)->color=color/255.0;
    view3d->updateMol();
}


void ringProperty::on_checkBox_type_stateChanged(int arg1)
{
    if(arg1>0)
        mol->getRingbyId(id)->type=1;
    else
        mol->getRingbyId(id)->type=0;
}


void ringProperty::on_spinBox_opacity_valueChanged(int arg1)
{
    opacity=arg1/255.0;
    mol->getRingbyId(id)->opacity=opacity/255.0;
    view3d->updateMol();
}

void ringProperty::on_checkBox_aromatic_stateChanged(int arg1)
{
    if(arg1>0)
        mol->setRingAromatic(id,true);
    else
        mol->setRingAromatic(id,false);
}
