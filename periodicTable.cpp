#include "periodicTable.h"
#include "mTools.h"

PeriodicTable::PeriodicTable(QWidget *parent) :  QFrame(parent)
{
    ui.setupUi(this);
    init();
}

PeriodicTable::~PeriodicTable()
{

}
void  PeriodicTable::OnClose()
{
    view3d->replaceSelelctedAtomWith(idx);
    close();
}

void PeriodicTable::init()
{
    QString s;
    vector3 color;


    for(unsigned int i=0;i<99;i++)
    {
        color=MakeGlColor(ElementColors[i])*255;
        s=tr("background-color: rgb(%1,%2,%3);").arg(color.x()).arg(color.y()).arg(color.z());

        switch (i) {
        case 0:
            ui.element_0->setStyleSheet(s);
            ui.element_00->setStyleSheet(s);
            break;

        case 1:  ui.element_1->setStyleSheet(s);break;
        case 2:  ui.element_2->setStyleSheet(s);break;
        case 3:  ui.element_3->setStyleSheet(s);break;
        case 4:  ui.element_4->setStyleSheet(s);break;
        case 5:  ui.element_5->setStyleSheet(s);break;
        case 6:  ui.element_6->setStyleSheet(s);break;
        case 7:  ui.element_7->setStyleSheet(s);break;
        case 8:  ui.element_8->setStyleSheet(s);break;
        case 9:  ui.element_9->setStyleSheet(s);break;

        case 10:  ui.element_10->setStyleSheet(s);break;
        case 11:  ui.element_11->setStyleSheet(s);break;
        case 12:  ui.element_12->setStyleSheet(s);break;
        case 13:  ui.element_13->setStyleSheet(s);break;
        case 14:  ui.element_14->setStyleSheet(s);break;
        case 15:  ui.element_15->setStyleSheet(s);break;
        case 16:  ui.element_16->setStyleSheet(s);break;
        case 17:  ui.element_17->setStyleSheet(s);break;
        case 18:  ui.element_18->setStyleSheet(s);break;
        case 19:  ui.element_19->setStyleSheet(s);break;
        case 20:  ui.element_20->setStyleSheet(s);break;

        case 21:  ui.element_21->setStyleSheet(s);break;
        case 22:  ui.element_22->setStyleSheet(s);break;
        case 23:  ui.element_23->setStyleSheet(s);break;
        case 24:  ui.element_24->setStyleSheet(s);break;
        case 25:  ui.element_25->setStyleSheet(s);break;
        case 26:  ui.element_26->setStyleSheet(s);break;
        case 27:  ui.element_27->setStyleSheet(s);break;
        case 28:  ui.element_28->setStyleSheet(s);break;
        case 29:  ui.element_29->setStyleSheet(s);break;
        case 30:  ui.element_30->setStyleSheet(s);break;

        case 31:  ui.element_31->setStyleSheet(s);break;
        case 32:  ui.element_32->setStyleSheet(s);break;
        case 33:  ui.element_33->setStyleSheet(s);break;
        case 34:  ui.element_34->setStyleSheet(s);break;
        case 35:  ui.element_35->setStyleSheet(s);break;
        case 36:  ui.element_36->setStyleSheet(s);break;
        case 37:  ui.element_37->setStyleSheet(s);break;
        case 38:  ui.element_38->setStyleSheet(s);break;
        case 39:  ui.element_39->setStyleSheet(s);break;
        case 40:  ui.element_40->setStyleSheet(s);break;

        case 41:  ui.element_41->setStyleSheet(s);break;
        case 42:  ui.element_42->setStyleSheet(s);break;
        case 43:  ui.element_43->setStyleSheet(s);break;
        case 44:  ui.element_44->setStyleSheet(s);break;
        case 45:  ui.element_45->setStyleSheet(s);break;
        case 46:  ui.element_46->setStyleSheet(s);break;
        case 47:  ui.element_47->setStyleSheet(s);break;
        case 48:  ui.element_48->setStyleSheet(s);break;
        case 49:  ui.element_49->setStyleSheet(s);break;
        case 50:  ui.element_50->setStyleSheet(s);break;

        case 51:  ui.element_51->setStyleSheet(s);break;
        case 52:  ui.element_52->setStyleSheet(s);break;
        case 53:  ui.element_53->setStyleSheet(s);break;
        case 54:  ui.element_54->setStyleSheet(s);break;
        case 55:  ui.element_55->setStyleSheet(s);break;
        case 56:  ui.element_56->setStyleSheet(s);break;
        case 57:  ui.element_57->setStyleSheet(s);break;
        case 58:  ui.element_58->setStyleSheet(s);break;
        case 59:  ui.element_59->setStyleSheet(s);break;
        case 60:  ui.element_60->setStyleSheet(s);break;

        case 61:  ui.element_61->setStyleSheet(s);break;
        case 62:  ui.element_62->setStyleSheet(s);break;
        case 63:  ui.element_63->setStyleSheet(s);break;
        case 64:  ui.element_64->setStyleSheet(s);break;
        case 65:  ui.element_65->setStyleSheet(s);break;
        case 66:  ui.element_66->setStyleSheet(s);break;
        case 67:  ui.element_67->setStyleSheet(s);break;
        case 68:  ui.element_68->setStyleSheet(s);break;
        case 69:  ui.element_69->setStyleSheet(s);break;

        case 70:  ui.element_70->setStyleSheet(s);break;

        case 71:  ui.element_71->setStyleSheet(s);break;
        case 72:  ui.element_72->setStyleSheet(s);break;
        case 73:  ui.element_73->setStyleSheet(s);break;
        case 74:  ui.element_74->setStyleSheet(s);break;
        case 75:  ui.element_75->setStyleSheet(s);break;
        case 76:  ui.element_76->setStyleSheet(s);break;
        case 77:  ui.element_77->setStyleSheet(s);break;
        case 78:  ui.element_78->setStyleSheet(s);break;
        case 79:  ui.element_79->setStyleSheet(s);break;

        case 80:  ui.element_80->setStyleSheet(s);break;


        case 81:  ui.element_81->setStyleSheet(s);break;
        case 82:  ui.element_82->setStyleSheet(s);break;
        case 83:  ui.element_83->setStyleSheet(s);break;
        case 84:  ui.element_84->setStyleSheet(s);break;
        case 85:  ui.element_85->setStyleSheet(s);break;
        case 86:  ui.element_86->setStyleSheet(s);break;
        case 87:  ui.element_87->setStyleSheet(s);break;
        case 88:  ui.element_88->setStyleSheet(s);break;
        case 89:  ui.element_89->setStyleSheet(s);break;

        case 90:  ui.element_90->setStyleSheet(s);break;
        case 91:  ui.element_91->setStyleSheet(s);break;
        case 92:  ui.element_92->setStyleSheet(s);break;
        case 93:  ui.element_93->setStyleSheet(s);break;
        case 94:  ui.element_94->setStyleSheet(s);break;
        case 95:  ui.element_95->setStyleSheet(s);break;
        case 96:  ui.element_96->setStyleSheet(s);break;
        case 97:  ui.element_97->setStyleSheet(s);break;
        case 98:  ui.element_98->setStyleSheet(s);break;
        case 99:  ui.element_99->setStyleSheet(s);break;

        case 100:  ui.element_100->setStyleSheet(s);break;
        case 101:  ui.element_101->setStyleSheet(s);break;
        case 102:  ui.element_102->setStyleSheet(s);break;
        case 103:  ui.element_103->setStyleSheet(s);break;
        }

    }

    //element_3
}

void PeriodicTable::on_element_0_clicked()
{
    idx=0;
    OnClose();

}

void PeriodicTable::on_element_00_clicked()
{
    idx=0;OnClose();
}

void PeriodicTable::on_element_1_clicked(){ idx=1;OnClose();}

void PeriodicTable::on_element_2_clicked(){ idx=2;OnClose();}

void PeriodicTable::on_element_3_clicked(){ idx=3;OnClose();}
void PeriodicTable::on_element_4_clicked(){ idx=4;OnClose();}
void PeriodicTable::on_element_5_clicked(){ idx=5;OnClose();}
void PeriodicTable::on_element_6_clicked(){ idx=6;OnClose();}
void PeriodicTable::on_element_7_clicked(){ idx=7;OnClose();}
void PeriodicTable::on_element_8_clicked(){ idx=8;OnClose();}
void PeriodicTable::on_element_9_clicked(){ idx=9;OnClose();}

void PeriodicTable::on_element_10_clicked(){ idx=10;OnClose();}
void PeriodicTable::on_element_11_clicked(){ idx=11;OnClose();}
void PeriodicTable::on_element_12_clicked(){ idx=12;OnClose();}
void PeriodicTable::on_element_13_clicked(){ idx=13;OnClose();}
void PeriodicTable::on_element_14_clicked(){ idx=14;OnClose();}
void PeriodicTable::on_element_15_clicked(){ idx=15;OnClose();}
void PeriodicTable::on_element_16_clicked(){ idx=16;OnClose();}
void PeriodicTable::on_element_17_clicked(){ idx=17;OnClose();}
void PeriodicTable::on_element_18_clicked(){ idx=18;OnClose();}
void PeriodicTable::on_element_19_clicked(){ idx=19;OnClose();}
void PeriodicTable::on_element_20_clicked() {idx=20;OnClose();}


void PeriodicTable::on_element_21_clicked(){ idx=21;OnClose();}
void PeriodicTable::on_element_22_clicked(){ idx=22;OnClose();}
void PeriodicTable::on_element_23_clicked(){ idx=23;OnClose();}
void PeriodicTable::on_element_24_clicked(){ idx=24;OnClose();}
void PeriodicTable::on_element_25_clicked(){ idx=25;OnClose();}
void PeriodicTable::on_element_26_clicked(){ idx=26;OnClose();}
void PeriodicTable::on_element_27_clicked(){ idx=27;OnClose();}
void PeriodicTable::on_element_28_clicked(){ idx=28;OnClose();}
void PeriodicTable::on_element_29_clicked(){ idx=29;OnClose();}
void PeriodicTable::on_element_30_clicked(){ idx=30;OnClose();}


void PeriodicTable::on_element_31_clicked(){ idx=31;OnClose();}
void PeriodicTable::on_element_32_clicked(){ idx=32;OnClose();}
void PeriodicTable::on_element_33_clicked(){ idx=33;OnClose();}
void PeriodicTable::on_element_34_clicked(){ idx=34;OnClose();}
void PeriodicTable::on_element_35_clicked(){ idx=35;OnClose();}
void PeriodicTable::on_element_36_clicked(){ idx=36;OnClose();}
void PeriodicTable::on_element_37_clicked(){ idx=37;OnClose();}
void PeriodicTable::on_element_38_clicked(){ idx=38;OnClose();}
void PeriodicTable::on_element_39_clicked(){ idx=39;OnClose();}
void PeriodicTable::on_element_40_clicked(){ idx=40;OnClose();}

void PeriodicTable::on_element_41_clicked(){ idx=41;OnClose();}
void PeriodicTable::on_element_42_clicked(){ idx=42;OnClose();}
void PeriodicTable::on_element_43_clicked(){ idx=43;OnClose();}
void PeriodicTable::on_element_44_clicked(){ idx=44;OnClose();}
void PeriodicTable::on_element_45_clicked(){ idx=45;OnClose();}
void PeriodicTable::on_element_46_clicked(){ idx=46;OnClose();}
void PeriodicTable::on_element_47_clicked(){ idx=47;OnClose();}
void PeriodicTable::on_element_48_clicked(){ idx=48;OnClose();}
void PeriodicTable::on_element_49_clicked(){ idx=49;OnClose();}
void PeriodicTable::on_element_50_clicked(){ idx=50;OnClose();}



void PeriodicTable::on_element_51_clicked(){ idx=51;OnClose();}
void PeriodicTable::on_element_52_clicked(){ idx=52;OnClose();}
void PeriodicTable::on_element_53_clicked(){ idx=53;OnClose();}
void PeriodicTable::on_element_54_clicked(){ idx=54;OnClose();}
void PeriodicTable::on_element_55_clicked(){ idx=55;OnClose();}
void PeriodicTable::on_element_56_clicked(){ idx=56;OnClose();}
void PeriodicTable::on_element_57_clicked(){ idx=57;OnClose();}
void PeriodicTable::on_element_58_clicked(){ idx=58;OnClose();}
void PeriodicTable::on_element_59_clicked(){ idx=59;OnClose();}
void PeriodicTable::on_element_60_clicked(){ idx=60;OnClose();}




void PeriodicTable::on_element_61_clicked(){ idx=61;OnClose();}
void PeriodicTable::on_element_62_clicked(){ idx=62;OnClose();}
void PeriodicTable::on_element_63_clicked(){ idx=63;OnClose();}
void PeriodicTable::on_element_64_clicked(){ idx=64;OnClose();}
void PeriodicTable::on_element_65_clicked(){ idx=65;OnClose();}
void PeriodicTable::on_element_66_clicked(){ idx=66;OnClose();}
void PeriodicTable::on_element_67_clicked(){ idx=67;OnClose();}
void PeriodicTable::on_element_68_clicked(){ idx=68;OnClose();}
void PeriodicTable::on_element_69_clicked(){ idx=69;OnClose();}
void PeriodicTable::on_element_70_clicked(){ idx=70;OnClose();}



void PeriodicTable::on_element_71_clicked(){ idx=71;OnClose();}
void PeriodicTable::on_element_72_clicked(){ idx=72;OnClose();}
void PeriodicTable::on_element_73_clicked(){ idx=73;OnClose();}
void PeriodicTable::on_element_74_clicked(){ idx=74;OnClose();}
void PeriodicTable::on_element_75_clicked(){ idx=75;OnClose();}
void PeriodicTable::on_element_76_clicked(){ idx=76;OnClose();}
void PeriodicTable::on_element_77_clicked(){ idx=77;OnClose();}
void PeriodicTable::on_element_78_clicked(){ idx=78;OnClose();}
void PeriodicTable::on_element_79_clicked(){ idx=79;OnClose();}
void PeriodicTable::on_element_80_clicked(){ idx=80;OnClose();}



void PeriodicTable::on_element_81_clicked(){ idx=81;OnClose();}
void PeriodicTable::on_element_82_clicked(){ idx=82;OnClose();}
void PeriodicTable::on_element_83_clicked(){ idx=83;OnClose();}
void PeriodicTable::on_element_84_clicked(){ idx=84;OnClose();}
void PeriodicTable::on_element_85_clicked(){ idx=85;OnClose();}
void PeriodicTable::on_element_86_clicked(){ idx=86;OnClose();}
void PeriodicTable::on_element_87_clicked(){ idx=87;OnClose();}
void PeriodicTable::on_element_88_clicked(){ idx=88;OnClose();}
void PeriodicTable::on_element_89_clicked(){ idx=89;OnClose();}
void PeriodicTable::on_element_90_clicked(){ idx=90;OnClose();}



void PeriodicTable::on_element_91_clicked(){ idx=91;OnClose();}
void PeriodicTable::on_element_92_clicked(){ idx=92;OnClose();}
void PeriodicTable::on_element_93_clicked(){ idx=93;OnClose();}
void PeriodicTable::on_element_94_clicked(){ idx=94;OnClose();}
void PeriodicTable::on_element_95_clicked(){ idx=95;OnClose();}
void PeriodicTable::on_element_96_clicked(){ idx=96;OnClose();}
void PeriodicTable::on_element_97_clicked(){ idx=97;OnClose();}
void PeriodicTable::on_element_98_clicked(){ idx=98;OnClose();}
void PeriodicTable::on_element_99_clicked(){ idx=99;OnClose();}

void PeriodicTable::on_element_100_clicked(){ idx=100;OnClose();}
void PeriodicTable::on_element_101_clicked(){ idx=101;OnClose();}
void PeriodicTable::on_element_102_clicked(){ idx=102;OnClose();}
void PeriodicTable::on_element_103_clicked(){ idx=103;OnClose();}

