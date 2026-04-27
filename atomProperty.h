#ifndef ATOMPROPERTY_H
#define ATOMPROPERTY_H

#include <QDialog>

#include "ui_dialogAtom.h"
#include "math/vector3.h"

class MainWindow;
class View3D;
class HMol;

class atomProperty :  public QDialog
{
    Q_OBJECT
public:

    explicit atomProperty(QWidget *parent = nullptr);

    void initial();
    void refreshView();
    void setParent(MainWindow *);

private:
    Ui::DialogAtom ui;
    MainWindow * m_parent;
    View3D * view3d;
    HMol * mol;

    unsigned int  idSelectedAtom;
    double radius0,radius;
    vector3 color0,color;



public slots:
    void on_dialAtomScale_valueChanged(int value);
    void on_pushButton_cancel_clicked();
    void on_pushButton_ok_clicked();
    void on_pushButton_color_clicked();

    void on_spinBox_red_valueChanged(int arg1);
    void on_spinBox_green_valueChanged(int arg1);
    void on_spinBox_blue_valueChanged(int arg1);
private slots:
    void on_doubleSpinBoxRadius_valueChanged(double arg1);
    void on_dialAtomRadius_valueChanged(int value);
};

#endif // ATOMPROPERTY_H
