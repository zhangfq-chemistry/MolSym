#ifndef ANGLEPROPERTY_H
#define ANGLEPROPERTY_H

#include <QDialog>

#include "ui_angle.h"

class MainWindow;
class View3D;
class HMol;

using namespace std;

namespace Ui {
    class AngleProperty;
}


class AngleProperty : public QDialog
{
    Q_OBJECT

public:
    explicit AngleProperty(QWidget *parent = nullptr);
    ~AngleProperty();

    void initial();
    void setParent(MainWindow *);
    void setAngle();
    void refreshSlider();
    void refreshEditor();

private slots:
    void on_horizontalSlider_valueChanged(int value);

    void on_pushButton_cancel_clicked();

    void on_pushButton_ok_clicked();

    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::Dialog_angle  ui;

    MainWindow * m_parent;
    View3D * view3d;
    HMol * mol;

    unsigned int   times;
    unsigned int  idx0,idx1,idx2;
    double bondAngle;
    vector <int> idList1,idList2;
};

#endif // ANGLEPROPERTY_H
