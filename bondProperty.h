#ifndef BONDPROPERTY_H
#define BONDPROPERTY_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include <vector>
#include "ui_bond.h"

class MainWindow;
class View3D;
class HMol;

using namespace std;

class bondProperty: public QDialog
{
    Q_OBJECT
public:
    Ui::Dialog_bond ui;
    explicit bondProperty(QWidget * parent = nullptr);


    MainWindow * m_parent;
    View3D * view3d;
    HMol * mol;

    void initial();
    void refreshView();
    void setParent(MainWindow *);

private slots:
    void on_radioButton1_clicked();

    void on_radioButton2_clicked();

    void on_radioButton3_clicked();

    void on_radioButton4_clicked();

    void on_radioButton5_clicked();

    void on_radioButton0_clicked();





    void on_horizontalSlider_valueChanged(int value);

    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();


    void refreshSlider();
    void refreshEditor();


    void on_lineEdit_textChanged(const QString &arg1);



    void setBondLength();

    void on_radioButton7_clicked(bool checked);

    void on_radioButton6_clicked(bool checked);

private:
    unsigned int   times;
    unsigned int idx0,idx1;
    unsigned int  bondOrder;
    double bondlength;
    vector <int> idList1,idList2;

};

#endif // BONDPROPERTY_H
