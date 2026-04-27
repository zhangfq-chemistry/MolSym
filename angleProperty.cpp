#include "angleProperty.h"
#include "mainwindow.h"
#include <QMessageBox>



AngleProperty::AngleProperty(QWidget *parent) :  QDialog(parent)
{
   ui.setupUi(this);
}

AngleProperty::~AngleProperty()
{

}

void AngleProperty::setParent (MainWindow * p)
{
    m_parent=p;
    mol=m_parent->getMol();
    mol->backupAtoms();
    view3d=m_parent->getView3D();
    initial();
}

#include <algorithm>
void AngleProperty::initial()
{
    times=0;

    ui.comboBox1->setCurrentIndex (2);
    ui.comboBox2->setCurrentIndex (2);

    ui.horizontalSlider->setTickInterval(10);
    ui.horizontalSlider->setRange(0,180000);

    idx0=view3d->getSelectedAtomId(0);
    idx1=view3d->getSelectedAtomId(1);
    idx2=view3d->getSelectedAtomId(2);

    bondAngle=mol->Angle(idx0,idx1,idx2);


    ui.textEdit1->setFont(QFont("Courier", 12));
    ui.textEdit2->setFont(QFont("Courier", 12));
    ui.lineEdit->setAlignment(Qt::AlignCenter);
    ui.lineEdit->setText(QString::number(bondAngle, 6, 4));


    mol->findChildren(idList1,idx1,idx0);
    mol->findChildren(idList2,idx1,idx2);

    vector <int> :: iterator iter;
    for (iter=idList1.begin();iter!=idList1.end();iter++)
    {
        if ( (*iter)==idx1) {
            idList1.erase(iter); break;
        }

    }

    for (iter=idList2.begin();iter!=idList2.end();iter++)
    {
        if ( (*iter)==idx1) {
            idList2.erase(iter); break;
        }
    }






    std::stable_sort(idList1.begin() + 1, idList1.end());
    std::stable_sort(idList2.begin() + 1, idList2.end());


    QString text1=QString::number(idx0+1)+":  ";
    for(int i=0;i<idList1.size();i++)
        text1+= QString::number(1+idList1[i])+"  ";
        ui.textEdit1->appendPlainText(text1);

    QString text2=QString::number(idx2+1)+":  ";
    for(int i=0;i<idList2.size();i++)
        text2+= QString::number(1+idList2[i])+"  ";
        ui.textEdit2->appendPlainText(text2);

}


void AngleProperty::setAngle()
{
    if( fabs(bondAngle)<0.1 || fabs(bondAngle-180.0)<0.1 ) return;

     times++;
     if(times<2) return;


     QString selectText1=ui.comboBox1->currentText(),
             selectText2=ui.comboBox2->currentText();

     if(selectText1.contains("Fixed") && selectText2.contains("Fixed") )
     {
         QMessageBox::about(NULL, "Warning!","Both two atoms are fixed!");
         return;
     }


     //get text from index0 list
     QString list1_text=ui.textEdit1->toPlainText ();
     list1_text.replace(',',' '); list1_text.replace(':',' ');
     vector <uint> mlist1;

     int m_index;
     QStringList lines=list1_text.split(QRegularExpression("\\s+"));
     for (int i=0; i<lines.count(); i++) {
         m_index=lines[i].toInt()-1;
         if(m_index>=0 && m_index<10000)  mlist1.push_back(m_index);
     }

     sort ( mlist1.begin(), mlist1.end());
     vector <uint> :: iterator iter = unique ( mlist1.begin(), mlist1.end());
     mlist1.erase(iter,mlist1.end());



     //get text from index1 list
     QString list2_text=ui.textEdit2->toPlainText ();
     list2_text.replace(',',' '); list2_text.replace(':',' ');

     vector <uint> mlist2;
     lines.clear();
     lines=list2_text.split(QRegularExpression("\\s+"));
     for (int i=0; i<lines.count(); i++) {
         m_index=lines[i].toInt()-1;
         if(m_index>=0 && m_index<10000)  mlist2.push_back(m_index);
     }
     sort ( mlist2.begin(), mlist2.end());
     iter = unique ( mlist2.begin(), mlist2.end());
     mlist1.erase(iter,mlist2.end());



     //fix atom or not
     //---------------------------------------------------------------------------------
     if( selectText1.contains("Fixed") &&  selectText2.contains("atom") )
     {
         mol->setBondAngleAtoms(idx0, idx1, idx2, bondAngle,
                               true, false);
         view3d->renderMol(mol); return;

     }

     if( selectText1.contains("atom")  && selectText2.contains("Fixed")  )
     {
         mol->setBondAngleAtoms(idx0, idx1, idx2, bondAngle,
                                           false, true);
         view3d->renderMol(mol); return;
         return;
     }

     if( selectText1.contains("atom") &&  selectText2.contains("atom") )
     {
         mol->setBondAngleAtoms(idx0, idx1, idx2, bondAngle,
                               false, false);
         view3d->renderMol(mol); return;
         return;
     }



     //fixed group or not, using OpenBabel functions
     if( selectText1.contains("Fixed")  && selectText2.contains("group") )
     {
         mol->setBondAngleFragments (idx0, idx1, idx2, bondAngle,
                                    mlist1, mlist2,
                                    true, false);
         view3d->renderMol(mol); return;
         return;
     }
     if( selectText1.contains("group") && selectText2.contains("Fixed")  )
     {
         mol->setBondAngleFragments (idx0, idx1, idx2, bondAngle,
                                    mlist1, mlist2,
                                    false,true);
         view3d->renderMol(mol); return;
         return;
     }
     if( selectText1.contains("group")  && selectText2.contains("group")  )
     {
         mol->setBondAngleFragments (idx0, idx1, idx2, bondAngle,
                                     mlist1, mlist2,
                                     false,false);
         view3d->renderMol(mol);
         return;
     }
     //move group and one atom simutaneously
     if( selectText1.contains("group")  && selectText2.contains("atom")  )
     {
         vector <unsigned int > list0; list0.push_back(idx2);
         mol->setBondAngleFragments (idx0, idx1, idx2, bondAngle,
                                     mlist1, list0,
                                     false,false);
         view3d->renderMol(mol);
         return;
     }

     //move group and one atom simutaneously
     if( selectText1.contains("atom")  && selectText2.contains("group")  )
     {
         vector <unsigned int > list0; list0.push_back(idx0);
         mol->setBondAngleFragments (idx0, idx1, idx2, bondAngle,
                                     list0, mlist2,
                                     false,false);
         view3d->renderMol(mol);
         return;
     }


 }


void AngleProperty::refreshEditor()
{
    bondAngle = ui.horizontalSlider->value ()/1000.0;
    ui.lineEdit->setText(QString::number (bondAngle));
    setAngle();
}

void AngleProperty::on_horizontalSlider_valueChanged(int value)
{
    refreshEditor();
}

void AngleProperty::refreshSlider()
{
    bondAngle = ui.lineEdit->text().toDouble();
    int scale = (int) (bondAngle*1000.0);
    ui.horizontalSlider->setValue(scale);
    setAngle();
}

void AngleProperty::on_lineEdit_textChanged(const QString &arg1)
{
    refreshSlider();
}




void AngleProperty::on_pushButton_cancel_clicked()
{
    view3d->releaseAllActors();
    mol->recoverAtomCoordinateFromBackup();
    mol->removeBackupAtoms();
    view3d->renderMol(mol);
    close();
}

void AngleProperty::on_pushButton_ok_clicked()
{
    mol->removeBackupAtoms();

    view3d->releaseAllActors();
    view3d->renderMol(mol);
    view3d->push2Stack("ChangBondAngle");
    close();
}

