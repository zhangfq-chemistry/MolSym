#ifndef ATOMTEMPLATE_H
#define ATOMTEMPLATE_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class AtomTemplate;
}

class AtomTemplate : public QDialog
{
    Q_OBJECT

public:
    explicit AtomTemplate(QWidget *parent = nullptr);
    ~AtomTemplate();

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

private:
    Ui::AtomTemplate *ui;
    MainWindow * m_parent;
    QString name;
};

#endif // ATOMTEMPLATE_H
