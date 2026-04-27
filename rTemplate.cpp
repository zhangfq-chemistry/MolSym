#include "rTemplate.h"
#include "ui_rTemplate.h"

RTemplate::RTemplate(QWidget *parent) :
    QDialog(parent), ui(new Ui::RTemplate)
{
    ui->setupUi(this);
}




RTemplate::~RTemplate()
{
    delete ui;
}

void RTemplate::setTemplateName ()
{
    m_parent->setTemplateName(name);
    close();
}





void RTemplate::on_Frag1_clicked()
{
    name="ethyl";
    setTemplateName();
}

void RTemplate::on_Frag2_clicked()
{
    name="propyl";
    setTemplateName();

}

void RTemplate::on_Frag3_clicked()
{
    name="ipropyl";
    setTemplateName();
}

void RTemplate::on_Frag4_clicked()
{
    name="tbutyl";
    setTemplateName();
}

void RTemplate::on_Frag7_clicked()
{
    name="vinyl";
    setTemplateName();
}

void RTemplate::on_Frag8_clicked()
{
    name="acetylenyl";
    setTemplateName();
}

void RTemplate::on_Frag9_clicked()
{
    name="carbonyl";
    setTemplateName();
}

void RTemplate::on_Frag10_clicked()
{
    name="acid";
    setTemplateName();
}

void RTemplate::on_Frag11_clicked()
{
    name="amide";
    setTemplateName();

}

void RTemplate::on_Frag12_clicked()
{
    name="nitro";
    setTemplateName();
}

void RTemplate::on_Frag13_clicked()
{
    name="nitroso";
    setTemplateName();
}

void RTemplate::on_Frag14_clicked()
{
    name="cyano";
    setTemplateName();
}

void RTemplate::on_Frag15_clicked()
{
     name="l_co";
     setTemplateName();
}

void RTemplate::on_Frag16_clicked()
{
    name="sulfoxide";
    setTemplateName();
}

void RTemplate::on_Frag19_clicked()
{
    name="sulfonyl";
    setTemplateName();
}

void RTemplate::on_Frag20_clicked()
{
    name="phosphinyl";
    setTemplateName();
}

void RTemplate::on_Frag25_clicked()
{
    name="l_eth";
    setTemplateName();
}

void RTemplate::on_Frag26_clicked()
{
    name="l_cprop";
    setTemplateName();
}

void RTemplate::on_Frag27_clicked()
{
    name="l_cbd";
    setTemplateName();
}

void RTemplate::on_Frag28_clicked()
{
    name="l_cp";
    setTemplateName();
}

void RTemplate::on_Frag29_clicked()
{
    name="l_benz";
    setTemplateName();
}

void RTemplate::on_Frag31_clicked()
{
    name="l_trop";
    setTemplateName();
}

void RTemplate::on_Frag32_clicked()
{
    name="l_cot";
    setTemplateName();
}

void RTemplate::on_Frag33_clicked()
{
    name="porph";
    setTemplateName();
}

void RTemplate::on_Frag0_clicked()
{
    name="methane";
    setTemplateName();
}
