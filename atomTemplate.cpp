#include "atomTemplate.h"
#include "ui_AtomTemplate.h"

AtomTemplate::AtomTemplate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AtomTemplate)
{
    ui->setupUi(this);
}

AtomTemplate::~AtomTemplate()
{
    delete ui;
}

void AtomTemplate::setTemplateName ()
{
    m_parent->setTemplateName(name);
    close();
}



void AtomTemplate::on_Frag1_clicked()
{
    name="tripla";
    setTemplateName();
}

void AtomTemplate::on_Frag2_clicked()
{
    name="tetsss";
    setTemplateName();
}

void AtomTemplate::on_Frag3_clicked()
{
    name="tet";
    setTemplateName();
}

void AtomTemplate::on_Frag4_clicked()
{
    name="seesaw";
    setTemplateName();
}

void AtomTemplate::on_Frag5_clicked()
{
    name="oct";
    setTemplateName();
}

void AtomTemplate::on_Frag6_clicked()
{
    name="captrig";
    setTemplateName();
}

void AtomTemplate::on_Frag7_clicked()
{
    name="pentbipyr";
    setTemplateName();
}

void AtomTemplate::on_Frag8_clicked()
{
    name="sqaprism";
    setTemplateName();
}
