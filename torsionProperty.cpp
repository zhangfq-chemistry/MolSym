#include "torsionProperty.h"
#include <QMessageBox>
#include "mainwindow.h"

TorsionProperty::TorsionProperty(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);
}

TorsionProperty::~TorsionProperty()
{

}

void TorsionProperty::setParent(MainWindow * p)
{
    m_parent=p;
    mol=m_parent->getMol();
    mol->backupAtoms();
    view3d=m_parent->getView3D();
    initial();
}


void TorsionProperty::initial()
{
    times=0;

    ui.comboBox1->setCurrentIndex (2);
    ui.comboBox2->setCurrentIndex (2);

    ui.horizontalSlider->setTickInterval(10);
    ui.horizontalSlider->setRange(-180000,180000);

    idx0=view3d->getSelectedAtomId(0);
    idx1=view3d->getSelectedAtomId(1);
    idx2=view3d->getSelectedAtomId(2);
    idx3=view3d->getSelectedAtomId(3);

    torsionAngle=mol->Dihedral(idx0,idx1,idx2,idx3);


    ui.textEdit1->setFont(QFont("Courier", 10));
    ui.textEdit2->setFont(QFont("Courier", 10));
    ui.lineEdit->setAlignment(Qt::AlignCenter);
    ui.lineEdit->setText(QString::number(torsionAngle, 6, 4));



    mol->findLargestFragment_1(idx1,idx2,idList1); //find the idx0
    mol->findLargestFragment_1(idx2,idx1,idList2); //find the idx2


    vector <uint> :: iterator iter;
    for (iter=idList1.begin();iter!=idList1.end();iter++)
    {
        if ( (*iter)==idx0) {
            idList1.erase(iter); break;
        }
    }


    for (iter=idList2.begin();iter!=idList2.end();iter++)
    {
        if ( (*iter)==idx3) {
            idList2.erase(iter);
            break;
        }
    }


    if(idList1.size()>idList2.size()) {
        ui.comboBox1->setCurrentIndex (0);
        ui.comboBox2->setCurrentIndex (2);
    }
    else {
        ui.comboBox1->setCurrentIndex (2);
        ui.comboBox2->setCurrentIndex (0);
    }

    QString text1=QString::number(idx0+1)+":  ";
    for(size_t i=0;i<idList1.size();i++)
        text1+= QString::number(1+idList1[i])+"  ";
        ui.textEdit1->appendPlainText(text1);


    QString text2=QString::number(idx3+1)+":  ";
    for(size_t i=0;i<idList2.size();i++)
        text2+= QString::number(1+idList2[i])+"  ";
        ui.textEdit2->appendPlainText(text2);
}


void TorsionProperty::setTorsion()
{
    if( torsionAngle<-179.99 || fabs(torsionAngle-180.0)<0.01 ) return;

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
        mol->setTorsionAtoms(idx0, idx1, idx2, idx3, torsionAngle,
                             true, false);


        view3d->refresh();

        return;
    }



    if( selectText1.contains("atom")  && selectText2.contains("Fixed")  )
    {
        mol->setTorsionAtoms(idx0, idx1, idx2,  idx3, torsionAngle, false, true);

        view3d->refresh();

        return;
    }



    if( selectText1.contains("atom") &&  selectText2.contains("atom") )
    {
        mol->setTorsionAtoms(idx0, idx1, idx2,  idx3,  torsionAngle,
                              false, false);


        view3d->refresh();

        return;

    }




    //fixed group or not, using OpenBabel functions
    if( selectText1.contains("Fixed")  && selectText2.contains("group") )
    {
        mol->setTorsionFragments (idx0, idx1, idx2,idx3, torsionAngle,
                                   mlist1, mlist2, true, false);


        view3d->refresh();

        return;
    }


    if( selectText1.contains("group") && selectText2.contains("Fixed")  )
    {
        mol->setTorsionFragments (idx0, idx1, idx2, idx3, torsionAngle,
                                   mlist1, mlist2,
                                   false,true);

        view3d->refresh();
        return;
    }

    if( selectText1.contains("group")  && selectText2.contains("group")  )
    {
        mol->setTorsionFragments (idx0, idx1, idx2, idx3, torsionAngle,
                                   mlist1, mlist2,
                                   false,false);

        view3d->refresh();
        return;
    }


    //move group and one atom simutaneously
    if( selectText1.contains("group")  && selectText2.contains("atom")  )
    {
        vector <uint > list0;
        list0.push_back(idx3);

        mol->setTorsionFragments (idx0, idx1, idx2, idx3, torsionAngle,
                                   mlist1, list0,
                                   false,false);



        view3d->refresh();

        return;
    }

    //move group and one atom simutaneously
    if( selectText1.contains("atom")  && selectText2.contains("group")  )
    {
        vector <uint > list0; list0.push_back(idx0);
        mol->setTorsionFragments (idx0, idx1, idx2,idx3,  torsionAngle,
                                    list0, mlist2,
                                    false,false);

        view3d->refresh();
        return;
    }
}



void TorsionProperty::refreshEditor()
{
    torsionAngle = ui.horizontalSlider->value ()/1000.0;
    ui.lineEdit->setText(QString::number (torsionAngle));
    setTorsion();
}

void TorsionProperty::refreshSlider()
{
    torsionAngle = ui.lineEdit->text().toDouble();
    int scale = (int) (torsionAngle*1000.0);
    ui.horizontalSlider->setValue(scale);

    setTorsion();
}


void TorsionProperty::on_horizontalSlider_valueChanged(int value)
{
    refreshEditor();
}


void TorsionProperty::on_lineEdit_textChanged(const QString &arg1)
{
    refreshSlider();
}


void TorsionProperty::on_pushButton_cancel_clicked()
{
    view3d->releaseAllActors();
    mol->recoverAtomCoordinateFromBackup();
    mol->removeBackupAtoms();
    view3d->renderMol(mol);
    close();
}


void TorsionProperty::on_pushButton_ok_clicked()
{
    view3d->releaseAllActors();
    mol->removeBackupAtoms();
    view3d->renderMol(mol);
    view3d->push2Stack("ChangTorsion");
    close();
}
