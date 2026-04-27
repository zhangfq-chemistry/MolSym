#ifndef TORSIONPROPERTY_H
#define TORSIONPROPERTY_H

#include <QDialog>
#include "ui_torsion.h"

class MainWindow;
class View3D;
class HMol;

using namespace std;

namespace Ui {
class TorsionProperty;
}

class TorsionProperty : public QDialog
{
    Q_OBJECT

public:
    explicit TorsionProperty(QWidget *parent = nullptr);
    ~TorsionProperty();

    void initial();
    void setParent(MainWindow *);
    void setTorsion();
    void refreshSlider();
    void refreshEditor();


private slots:
    void on_pushButton_cancel_clicked();
    void on_pushButton_ok_clicked();
    void on_horizontalSlider_valueChanged(int value);
    void on_lineEdit_textChanged(const QString &arg1);

private:
    Ui::Dialog_torsion ui;

    MainWindow * m_parent;
    View3D * view3d;
    HMol * mol;

    unsigned int   times;
    unsigned int  idx0,idx1,idx2,idx3;
    double torsionAngle;
    vector <uint> idList1,idList2;
    vector <uint> _listModified;
};

#endif // TORSIONPROPERTY_H
