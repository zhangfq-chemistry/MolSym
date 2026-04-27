#ifndef RINGTEMPLATE_H
#define RINGTEMPLATE_H

#include <QDialog>

class MainWindow;

namespace Ui {
class RingTemplate;
}

class RingTemplate : public QDialog
{
    Q_OBJECT

public:
    explicit RingTemplate(QWidget *parent = nullptr);
    ~RingTemplate();


    void  setTemplateName ();
    void  setParent (MainWindow * p) {m_parent=p;}


private slots:
    void on_Frag1_clicked();
    void on_Frag2_clicked();
    void on_Frag3_clicked();
    void on_Frag4_clicked();
    void on_Frag5_clicked();
    void on_Frag6_clicked();
    void on_Frag7_clicked();
    void on_Frag8_clicked();
    void on_Frag9_clicked();
    void on_Frag10_clicked();
    void on_Frag11_clicked();
    void on_Frag13_clicked();
    void on_Frag14_clicked();
    void on_Frag15_clicked();
    void on_Frag19_clicked();
    void on_Frag20_clicked();
    void on_Frag21_clicked();
    void on_Frag22_clicked();
    void on_Frag23_clicked();
    void on_Frag24_clicked();
    void on_Frag25_clicked();
    void on_Frag26_clicked();
    void on_Frag27_clicked();
    void on_Frag28_clicked();
    void on_Frag29_clicked();
    void on_Frag31_clicked();
    void on_Frag32_clicked();
    void on_Frag33_clicked();
    void on_Frag34_clicked();
    void on_Frag35_clicked();
    void on_Frag37_clicked();
    void on_Frag38_clicked();
    void on_Frag39_clicked();
    void on_Frag40_clicked();

    void on_Frag_C20_clicked();
    void on_Frag_B12_clicked();
    void on_Frag_C70_clicked();
    void on_Frag_C8H8_clicked();
    void on_Frag_C60_clicked();




private:
    Ui::RingTemplate *ui;
    MainWindow * m_parent;
    QString name;
};

#endif // RINGTEMPLATE_H
