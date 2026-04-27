#include "ringTemplate.h"
#include "ui_ringTemplate.h"
#include "mainwindow.h"


RingTemplate::RingTemplate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RingTemplate)
{
    ui->setupUi(this);
}

RingTemplate::~RingTemplate()
{
    delete ui;
}


void RingTemplate::setTemplateName ()
{
    m_parent->setTemplateName(name);
    close();
}

void RingTemplate::on_Frag1_clicked()
{
    name="phenyl";
    setTemplateName ();
}

void RingTemplate::on_Frag2_clicked()
{
    name="naphth";
    setTemplateName ();
}

void RingTemplate::on_Frag3_clicked()
{
    name="asixfive";
    setTemplateName ();
}

void RingTemplate::on_Frag4_clicked()
{
    name="anthr";
    setTemplateName ();

}

void RingTemplate::on_Frag5_clicked()
{
    name="rings3";
    setTemplateName ();
}

void RingTemplate::on_Frag6_clicked()
{
    name="rings4";
    setTemplateName ();
}

void RingTemplate::on_Frag7_clicked()
{
    name="coronene";
    setTemplateName ();
}

void RingTemplate::on_Frag8_clicked()
{
    name="pyridinyl";
    setTemplateName ();
}

void RingTemplate::on_Frag9_clicked()
{
    name="pyrazine";
    setTemplateName ();
}

void RingTemplate::on_Frag10_clicked()
{
    name="pyridazine";
    setTemplateName ();
}

void RingTemplate::on_Frag11_clicked()
{
    name="pyrimidine";
    setTemplateName ();
}

void RingTemplate::on_Frag13_clicked()
{
    name="cprane";
    setTemplateName ();
}

void RingTemplate::on_Frag14_clicked()
{
    name="oxirane";
    setTemplateName ();
}

void RingTemplate::on_Frag15_clicked()
{
    name="cbane";
    setTemplateName ();
}

void RingTemplate::on_Frag19_clicked()
{
    name="cp";
    setTemplateName ();
}

void RingTemplate::on_Frag20_clicked()
{
    name="furanyl";
    setTemplateName ();
}

void RingTemplate::on_Frag21_clicked()
{
    name="pyrryl";
    setTemplateName ();
}

void RingTemplate::on_Frag22_clicked()
{
    name="thiophenyl";
    setTemplateName ();
}

void RingTemplate::on_Frag23_clicked()
{
    name="cpene";
    setTemplateName ();
}

void RingTemplate::on_Frag24_clicked()
{
    name="cpane";
    setTemplateName ();
}

void RingTemplate::on_Frag25_clicked()
{
    name="chexbt";
    setTemplateName ();
}

void RingTemplate::on_Frag26_clicked()
{
    name="chexch";
    setTemplateName ();
}

void RingTemplate::on_Frag27_clicked()
{
    name="tropylium";
    setTemplateName ();
}

void RingTemplate::on_Frag28_clicked()
{
    name="mr7";
    setTemplateName ();
}

void RingTemplate::on_Frag29_clicked()
{
    name="mr8";
    setTemplateName ();
}

void RingTemplate::on_Frag31_clicked()
{
    name="two5mr";
    setTemplateName ();
}

void RingTemplate::on_Frag32_clicked()
{
    name="asixfive";
    setTemplateName ();
}

void RingTemplate::on_Frag33_clicked()
{
    name="azu";
    setTemplateName ();
}

void RingTemplate::on_Frag34_clicked()
{
    name="656";
    setTemplateName ();
}

void RingTemplate::on_Frag35_clicked()
{
    name="bipyr";
    setTemplateName ();
}

void RingTemplate::on_Frag37_clicked()
{
    name="bucky";
    setTemplateName ();
}

void RingTemplate::on_Frag38_clicked()
{
    name="bicychept";
    setTemplateName ();
}

void RingTemplate::on_Frag39_clicked()
{
    name="bicycoct";
    setTemplateName ();
}

void RingTemplate::on_Frag40_clicked()
{
    name="adamantane";
    setTemplateName ();
}

void RingTemplate::on_Frag_C20_clicked()
{
    name="C20";
    setTemplateName ();
}

void RingTemplate::on_Frag_B12_clicked()
{
    name="B12";
    setTemplateName ();
}

void RingTemplate::on_Frag_C70_clicked()
{
    name="C70";
    setTemplateName ();
}

void RingTemplate::on_Frag_C8H8_clicked()
{
    name="C8H8";
    setTemplateName ();
}

void RingTemplate::on_Frag_C60_clicked()
{
    name="C60";
    setTemplateName ();
}

