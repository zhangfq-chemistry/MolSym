#ifndef RTEMPLATE_H
#define RTEMPLATE_H

#include <QDialog>
#include "mainwindow.h"


namespace Ui {
class RTemplate;
}

class RTemplate : public QDialog
{
    Q_OBJECT

public:
    explicit RTemplate(QWidget *parent = nullptr);
    ~RTemplate();

    void  setTemplateName ();
    void  setParent (MainWindow * p) {m_parent=p;}


private slots:
    void on_Frag1_clicked();
    void on_Frag2_clicked();
    void on_Frag3_clicked();
    void on_Frag4_clicked();

    void on_Frag7_clicked();
    void on_Frag8_clicked();
    void on_Frag9_clicked();
    void on_Frag10_clicked();
    void on_Frag11_clicked();
    void on_Frag12_clicked();

    void on_Frag13_clicked();

    void on_Frag14_clicked();

    void on_Frag15_clicked();

    void on_Frag16_clicked();

    void on_Frag19_clicked();

    void on_Frag20_clicked();

    void on_Frag25_clicked();

    void on_Frag26_clicked();

    void on_Frag27_clicked();

    void on_Frag28_clicked();

    void on_Frag29_clicked();

    void on_Frag31_clicked();

    void on_Frag32_clicked();

    void on_Frag33_clicked();

    void on_Frag0_clicked();

private:
    Ui::RTemplate *ui;
    MainWindow * m_parent;
    QString name;

};

#endif // RTEMPLATE_H
