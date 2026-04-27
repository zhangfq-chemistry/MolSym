#ifndef RINGPROPERTY_H
#define RINGPROPERTY_H


#include <QDialog>

#include "ui_dialogRing.h"
#include "math/vector3.h"

class MainWindow;
class View3D;
class HMol;


class ringProperty : public QDialog
{
    Q_OBJECT
public:
    explicit ringProperty(QWidget *parent = nullptr);

    void initial();
    void refreshView();
    void setParent(MainWindow *);

private:
    Ui::DialogRing ui;
    MainWindow * m_parent;
    View3D * view3d;
    HMol * mol;

    unsigned int  id;
    double radius_scale0,radius_scale;
    double size0,size;
    double opacity0,opacity;
    vector3 color0,color;


public slots:
private slots:
    void on_spinBox_red_valueChanged(int arg1);
    void on_spinBox_green_valueChanged(int arg1);
    void on_spinBox_blue_valueChanged(int arg1);
    void on_pushButton_cancel_clicked();
    void on_pushButton_ok_clicked();
    void on_dialRadius_valueChanged(int value);
    void on_dialSize_valueChanged(int value);
    void on_pushButton_color_clicked();
    void on_checkBox_type_stateChanged(int arg1);
    void on_spinBox_opacity_valueChanged(int arg1);
    void on_checkBox_aromatic_stateChanged(int arg1);
};

#endif // RINGPROPERTY_H
