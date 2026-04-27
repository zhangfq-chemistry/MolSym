#include "bondProperty.h"
#include <QMessageBox>

#include "mainwindow.h"



bondProperty::bondProperty(QWidget * parent)  : QDialog(parent)
{
    ui.setupUi(this);
}

void bondProperty::setParent (MainWindow * p)
{
    m_parent=p;
    mol=m_parent->getMol();
    mol->backupAtoms();
    view3d=m_parent->getView3D();
    initial();
}

void bondProperty::initial()
{
    times=0;
    ui.comboBox_type1->setCurrentIndex (2);
    ui.comboBox_type2->setCurrentIndex (2);

    ui.textEdit_idList1->setFont(QFont("Courier", 12));
    ui.textEdit_idList2->setFont(QFont("Courier", 12));


    ui.horizontalSlider->setTickInterval(1);
    ui.horizontalSlider->setRange(5000,100000);

    if(view3d->numSelectedAtoms()==2)
    {
        idx0=view3d->getSelectedAtomId(0);
        idx1=view3d->getSelectedAtomId(1);
    }
    else if (view3d->numSelectedBonds()==1)
    {
        view3d->getSelectedBondIds(0,idx0,idx1);
    }


    bondlength=mol->Distance(idx0,idx1);
    bondOrder=mol->getBondOrderbyAtomIndex(idx0,idx1);

    ui.lineEdit->setText(QString::number(bondlength, 6, 4));


    switch (bondOrder){
    case 0:
        ui.radioButton0->setChecked(true);
        break;
    case 1:
        ui.radioButton1->setChecked(true);
        break;
    case 2:
        ui.radioButton2->setChecked(true);
        break;
    case 3:
        ui.radioButton3->setChecked(true);
        break;
    case 4:
        ui.radioButton4->setChecked(true);
        break;
    case 5:
        ui.radioButton5->setChecked(true);
        break;
    }
    refreshSlider();


    mol->findChildren(idList1,idx1,idx0);
    mol->findChildren(idList2,idx0,idx1);

    vector <int> :: iterator iter;
    for (iter=idList1.begin();iter!=idList1.end();iter++)
    {
        if ( (*iter)==idx1) {
            idList1.erase(iter); break;
        }
    }

    for (iter=idList2.begin();iter!=idList2.end();iter++)
    {
        if ( (*iter)==idx0) {
            idList2.erase(iter); break;
        }
    }




    std::stable_sort(idList1.begin() + 1, idList1.end());
    std::stable_sort(idList2.begin() + 1, idList2.end());



    QString text1=QString::number(idx0+1)+":  ";
    for(int i=0;i<idList1.size();i++)
        text1+= QString::number(1+idList1[i])+"  ";
        ui.textEdit_idList1->appendPlainText(text1);

    QString text2=QString::number(idx1+1)+":  ";
    for(int i=0;i<idList2.size();i++)
        text2+= QString::number(1+idList2[i])+"  ";
        ui.textEdit_idList2->appendPlainText(text2);
}




void bondProperty::refreshSlider()
{
    bondlength = ui.lineEdit->text().toDouble();

    int scale = (int) (bondlength*10000.0);
    ui.horizontalSlider->setValue(scale);
}

void bondProperty::refreshEditor()
{
    bondlength = double(ui.horizontalSlider->value ())/10000.0;
    ui.lineEdit->setText(QString::number (bondlength));
}


void bondProperty::on_radioButton1_clicked()
{
    unsigned int  bo=mol->getBondOrderbyAtomIndex(idx0,idx1);
    if (bo==1) return;

    if (bo==0)
        mol->addBond(idx0,idx1,1);
    else
        mol->getBondbyAtomIndex(idx0,idx1)->setBondOrder(1);

    mol->updateAllAtomLinker();
    mol->perceivePlaneRingforBonds();
    view3d->renderMol(mol);
}



void bondProperty::on_radioButton2_clicked()
{
    unsigned int bo=mol->getBondOrderbyAtomIndex(idx0,idx1);
    if (bo==2) return;

    if (bo==0)
        mol->addBond(idx0,idx1,2);
    else
        mol->getBondbyAtomIndex(idx0,idx1)->setBondOrder(2);

    mol->updateAllAtomLinker();
    mol->perceivePlaneRingforBonds();
    view3d->renderMol(mol);
}



void bondProperty::on_radioButton3_clicked()
{
    unsigned int bo=mol->getBondOrderbyAtomIndex(idx0,idx1);
    if (bo==3) return;

    if (bo==0)
        mol->addBond(idx0,idx1,3);
    else
        mol->getBondbyAtomIndex(idx0,idx1)->setBondOrder(3);

    mol->updateAllAtomLinker();
    mol->perceivePlaneRingforBonds();
    view3d->renderMol(mol);
}

void bondProperty::on_radioButton4_clicked()
{
    unsigned int bo=mol->getBondOrderbyAtomIndex(idx0,idx1);
    if (bo==4) return;

    if (bo==0)
        mol->addBond(idx0,idx1,4);
    else
        mol->getBondbyAtomIndex(idx0,idx1)->setBondOrder(4);

    mol->updateAllAtomLinker();
    mol->perceivePlaneRingforBonds();
    view3d->renderMol(mol);
}

//aromatic bond
void bondProperty::on_radioButton5_clicked()
{
    unsigned int bo=mol->getBondOrderbyAtomIndex(idx0,idx1);
    if (bo==6) return;

    if (bo==0)
        mol->addBond(idx0,idx1,6);
    else
        mol->getBondbyAtomIndex(idx0,idx1)->setBondOrder(6);

    mol->updateAllAtomLinker();
    mol->perceivePlaneRingforBonds();
    view3d->renderMol(mol);
}

void bondProperty::on_radioButton0_clicked()
{
    unsigned int bo=mol->getBondOrderbyAtomIndex(idx0,idx1);
    if (bo==0) return;

    mol->removeBondbyAtomIds(idx0,idx1);
    mol->updateAllAtomLinker();
    mol->perceivePlaneRingforBonds();
    view3d->renderMol(mol);
}

//weak bond
void bondProperty::on_radioButton7_clicked(bool checked)
{
    unsigned int bo=mol->getBondOrderbyAtomIndex(idx0,idx1);
    if (bo==7) return;

    if (bo==0)
        mol->addBond(idx0,idx1,7);
    else
        mol->getBondbyAtomIndex(idx0,idx1)->setBondOrder(7);

    mol->perceivePlaneRingforBonds();
    view3d->renderMol(mol);
}

//hydrogen-bond=6
void bondProperty::on_radioButton6_clicked(bool checked)
{

}



void bondProperty::on_pushButton_ok_clicked()
{
    mol->removeBackupAtoms();

    view3d->releaseAllActors();
    view3d->renderMol(mol);
    view3d->push2Stack("ChangBond");

    close();
}

void bondProperty::on_pushButton_cancel_clicked()
{
    view3d->releaseAllActors();
    mol->recoverAtomCoordinateFromBackup();
    view3d->renderMol(mol);
    mol->removeBackupAtoms();

    close();
}

void bondProperty::on_horizontalSlider_valueChanged(int value)
{
    refreshEditor();
    setBondLength();
}


void bondProperty::on_lineEdit_textChanged(const QString &arg1)
{
     refreshSlider();
     setBondLength();
}

void bondProperty::setBondLength()
{
    times++;
    if(times<3) return;

    QString selectText1=ui.comboBox_type1->currentText(),
            selectText2=ui.comboBox_type2->currentText();

    if(selectText1.contains("Fixed") && selectText2.contains("Fixed") )
    {
        QMessageBox::about(NULL, "Warning!","Both two atoms are fixed!");
        return;
    }

      //get text from index0 list
      QString list1_text=ui.textEdit_idList1->toPlainText ();
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
      QString list2_text=ui.textEdit_idList2->toPlainText ();
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


/*

      cout << "two list:"<<endl;
      for (int i=0; i<mlist1.size(); i++)
          cout << mlist1[i] +1<< " ";
      cout <<endl;

      for (int i=0; i<mlist2.size(); i++)
          cout << mlist2[i] +1<< " ";
      cout <<endl;

*/

      //check if list2 contains element of list1
      bool isfind=false;
      for (int i=0; i<mlist1.size(); i++) {
          for (int j=0; j<mlist2.size(); j++)
          {
              if(mlist2[j]==mlist1[i]) {
                  isfind=true;
                  break;
              }
          }
      }

      if(isfind){
          QMessageBox::about(NULL, "Warning!","The two fragment contains the same atoms");
          return;
      }


      //fix atom or not, do not using OpenBabel functions
      //---------------------------------------------------------------------------------
      if( selectText1.contains("Fixed") &&  selectText2.contains("atom") ) {
          mol->setBondLengthBetweenTwoAtoms(idx0, idx1, bondlength,   true , false );
          view3d->renderMol(mol); return;
      }


      if( selectText2.contains("Fixed") &&  selectText1.contains("atom") ) {
          mol->setBondLengthBetweenTwoAtoms(idx0, idx1, bondlength, false, true);
          view3d->renderMol(mol); return;
      }


      if( selectText1.contains("atom") &&  selectText2.contains("atom") ) {
          mol->setBondLengthBetweenTwoAtoms(idx0, idx1, bondlength, false, false);
          view3d->renderMol(mol); return;
      }

      //-------------------------------------------------------------------------------------
      if( selectText1.contains("Fixed")  && selectText2.contains("group") )
      {
          mol->setBondLengthBetweenTwoFragments(idx0, idx1, bondlength, mlist1, mlist2, true, false);
          view3d->renderMol(mol);
          return;
      }
      if( selectText1.contains("group") && selectText2.contains("Fixed")  )
      {
          mol->setBondLengthBetweenTwoFragments(idx0, idx1, bondlength, mlist1, mlist2, false, true);
          view3d->renderMol(mol);
          return;
      }


      if( selectText1.contains("group")  && selectText2.contains("group")  )
      {
          mol->setBondLengthBetweenTwoFragments(idx0, idx1, bondlength, mlist1, mlist2, false, false);
          view3d->renderMol(mol);
          return;
      }

      //move group and one atom simutaneously
      if( selectText1.contains("group")  && selectText2.contains("atom")  )
      {
          vector <unsigned int > list0; list0.push_back(idx1);
          mol->setBondLengthBetweenTwoFragments(idx0, idx1, bondlength, mlist1, list0, false, false);
          view3d->renderMol(mol);
          return;
      }

      //move group and one atom simutaneously
      if( selectText1.contains("atom")  && selectText2.contains("group")  )
      {
          vector <unsigned int > list0; list0.push_back(idx0);
          mol->setBondLengthBetweenTwoFragments(idx0, idx1, bondlength, list0, mlist2, false, false);
          view3d->renderMol(mol);
          return;
      }

}




