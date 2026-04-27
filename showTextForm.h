#ifndef SHOWTEXTFORM_H
#define SHOWTEXTFORM_H

#include <QDialog>
#include <QProcess>
#include "ui_showTextForm.h"



//using namespace GiNaC;



class MainWindow;
class View3D;
class HMol;

namespace Ui {
class ShowTextForm;
}

class ShowTextForm : public QDialog
{
    Q_OBJECT

public:
    explicit ShowTextForm(QString title="", QWidget *parent = nullptr);
    ~ShowTextForm() ;

    void initial();
    void initialXTB();
    void setParent(MainWindow *);

    void setText(QString s);
    void appendText(QString s);

    void clear();
    void setCalXTB (bool t) {isXTBCal=t;}
    void  runXTB();

private slots:
   void on_pushButton_Close_clicked();


   void on_pushButton_run_clicked();
   void readProcess();
   void finishedProcess();


   void on_spinBox_charge_valueChanged(const QString &arg1);

   void on_spinBox_numberElectrons_valueChanged(const QString &arg1);

private:
    Ui::ShowTextForm ui;
    MainWindow * m_parent;
    View3D * view3d;
    HMol * mol;
    bool isXTBCal;
    QProcess * runShellProcess;
    QString   log,commandXTB,executePath,execute_parameters;


    //GiNaC::matrix Huckel;
   // GiNaC::ex detHuckel,detHuckel1;
};

#endif // SHOWTEXTFORM_H
