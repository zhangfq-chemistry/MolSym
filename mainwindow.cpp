#include <QtWidgets>

#include "mainwindow.h"
#include "math/symmetry.h"
#include "math/vector3.h"

#include <QVTKOpenGLNativeWidget.h>
#include <vtkCamera.h>
#include <vtkDataSet.h>
#include <vtkDataSetMapper.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <vtkAnimationScene.h>
#include <vtkCMLMoleculeReader.h>
#include <vtkGaussianCubeReader2.h>
#include <vtkPDBReader.h>
#include <vtkXYZMolReader2.h>

#include <QColorDialog>
#include <QDialog>
#include <QIcon>

#include "bondProperty.h"

#include "aboutDialog.h"
#include "angleProperty.h"
#include "atomTemplate.h"
#include "rTemplate.h"
#include "ringProperty.h"
#include "ringTemplate.h"
#include "showTextForm.h"
#include "templateMol.h"
#include "templateView3d.h"
#include "torsionProperty.h"

#include "periodicTable.h"

#include <QColorDialog>
#include <QTimer>
#include <QWidget>
#include <QtColorWidgets/color_2d_slider.hpp>
#include <QtColorWidgets/color_delegate.hpp>
#include <QtColorWidgets/color_dialog.hpp>
#include <QtColorWidgets/color_line_edit.hpp>
#include <QtColorWidgets/color_list_widget.hpp>
#include <QtColorWidgets/color_palette_widget.hpp>
#include <QtColorWidgets/color_preview.hpp>
#include <QtColorWidgets/color_wheel.hpp>
#include <QtColorWidgets/hue_slider.hpp>

#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/generic.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>

#include "codeEditor.h"
#include <QRegularExpression>

static QRegularExpression re("^-?(100(\\.\\d{1,46})?|(\\d|[1-9]\\d)(\\.\\d{1,46})?)$");

MainWindow::MainWindow() : mdiArea(new QMdiArea) {
    ui.setupUi(this);

    ui.view3d->mainWindow = this;
    view3d = ui.view3d;

    ui.tabWidget->setCurrentWidget(ui.tab_3d);
    // ui.mdiArea->addSubWindow(view3d);
    ui.view3d->show();

    ui.view3d->setTemplateView3d(ui.openGLWidget_template);
    ui.openGLWidget_template->setMainWindow(this);

    ui.openGLWidget_template->setStyleSheet(
        "QFrame#frame{border:2px solid #014F84;background-color:#00d8ff}");


    mol = new HMol(this);
    ui.view3d->mol = mol;

    ui.view3d->showMaximized();

    ui.textEdit_dataMol->setParent(this);
    ui.textEdit_dataMol->setView3D(view3d);
    ui.textEdit_dataMol->setMol(mol);

    isMetalTemplate = false;

    ui.checkBox_PGElement->setCheckState(Qt::Unchecked);
    ui.checkBox_atomLabel->setCheckState(Qt::Unchecked);
    ui.checkBox_C2Hide->setCheckState(Qt::Unchecked);
    ui.checkBox_C3Hide->setCheckState(Qt::Unchecked);
    ui.checkBox_CnHide->setCheckState(Qt::Unchecked);
    ui.checkBox_SigmaD_Hide->setCheckState(Qt::Unchecked);
    //ui.checkBox_outline->setCheckState(Qt::Unchecked);

    isFirstTime = true;
    needFreshView = true;

    ui.doubleSpinBox_tolerance->setValue(0.1);

    FileName = "UnTitled";

    ui.dialAtomScale->setRange(1, 1000);
    ui.dialBondScale->setRange(1, 1000);

    QChar sigma = QChar(0xc3, 0x03);
    QString sigmaV = sigma + '-' + 'v';
    QString sigmaD = sigma + '-' + 'd';
    QString sigmaH = sigma + '-' + 'h';

    ui.pushButton_V->setText(QString(sigmaV));
    ui.pushButton_H->setText(QString(sigmaH));
    ui.pushButton_D->setText(QString(sigmaD));

    ui.treeWidget->setViewer(ui.view3d);
    ui.treeWidget->setMainWin(this);
    ui.spinBox_movieFrames->setValue(40);

    view3d->setOperatorTree(ui.treeWidget);

    ui.doubleSpinBox_thickness_C2->setValue(0.08);
    ui.checkBox_TypeC2_II->setCheckState(Qt::Checked);

    // init data
    ui.doubleSpinBox_movieOpacity->blockSignals(true);
    ui.doubleSpinBox_movieOpacity->setValue(0.8);
    ui.doubleSpinBox_movieOpacity->blockSignals(false);

    VersionLabel.setText("山西师范大学结构化学课题组");
    statusBar()->addPermanentWidget(&VersionLabel);
    statusBar()->addWidget(&ShowLabel);

    initSymmetry();

    //-100，100

    auto pReg = new QRegularExpressionValidator(re, this);
    ui.lineEdit_X->setValidator(pReg);
    ui.lineEdit_Y->setValidator(pReg);
    ui.lineEdit_Z->setValidator(pReg);

    ui.lineEdit_XX->setValidator(pReg);
    ui.lineEdit_YY->setValidator(pReg);
    ui.lineEdit_ZZ->setValidator(pReg);

#ifdef WIN32
    // openbabel
    QString curPath = getExecutePath();
    QString babel_DataDir =
        "BABEL_DATADIR=" + curPath + "\\openbabel\\share\\openbabel\\3.1.0";
    babel_DataDir.replace("\/", "\\");
    _putenv(babel_DataDir.toStdString().c_str());
    QString babel_LibDir =
        "BABEL_LIBDIR=" + curPath + "\\openbabel\\lib\\openbabel\\3.1.0";
    babel_LibDir.replace("\/", "\\");
    _putenv(babel_LibDir.toStdString().c_str());
    cout << babel_LibDir.toStdString().c_str() << " is set!" << endl;
    cout << babel_DataDir.toStdString().c_str() << " is set!" << endl;
#endif

    // ui.tabWidget_2->hide();
}

uint MainWindow::getMovieFrames() { return ui.spinBox_movieFrames->value(); }

void MainWindow::onTextEditPaste() {}

void MainWindow::actionShow3D() {
    view3d->show();
    view3d->updateView();
    view3d->setFocus();
}

void MainWindow::clearAllViews() {
    view3d->removeAllActors();
    ui.comboBox_symmetry->setCurrentIndex(0);
    view3d->setSymmetry("C1");
    view3d->updateView();
}

void MainWindow::on3DViewActive() {
    view3d->updateView();
    view3d->setFocus();
}

void MainWindow::onInformActive() {}

void MainWindow::closeEvent(QCloseEvent *event) {
    /*
  mdiArea->closeAllSubWindows();
  if (mdiArea->currentSubWindow()) {
      event->ignore();
  } else {
      writeSettings();
      event->accept();
  }
  */
}

void MainWindow::addMolecule(QString &s) {
    QApplication::setOverrideCursor(Qt::WaitCursor);
    // ui.treeWidget->AddMolecularOrbitals(s);
    QApplication::restoreOverrideCursor();

    // view3d->renderMol(ui.treeWidget->getActivedMol());
    view3d->updateView();
}

void MainWindow::on_actionFileNew_triggered() {
    isFirstTime = true;

    ui.comboBox_example->blockSignals(true);
    ui.comboBox_example->setCurrentIndex(0);
    ui.comboBox_example->blockSignals(false);

    undoList.clear();
    redoList.clear();

    mol->clearAll();
    ui.comboBox_symmetry->setCurrentIndex(0);
    view3d->setSymmetry("C1");

    view3d->setSymmetryElementVisible(false);
    view3d->setOutLineType(0);
    view3d->renderMol(mol);
    dataMol0 = "";
    ui.textEdit_dataMol->setText("");

    isMetalTemplate = false;

    ui.doubleSpinBox_tolerance->setValue(0.1);
    FileName = "UnTitle";
    ui.dialAtomScale->setRange(1, 1000);
    ui.dialBondScale->setRange(1, 1000);

    ui.treeWidget->stopTimer();

    initSymmetry();

    disable_C2();
    disable_C3();
    disable_Cn();

    disable_SigmaV();
    disable_SigmaD();
    disable_SigmaH();
}

void MainWindow::on_actionFileOpen_triggered() {
    QStringList filters;
    filters << tr("XYZ") + "(*.xyz)" << tr("MOL") + "(*.mol)"
            << tr("PDB") + "(*.pdb)" << tr("CML") + "(*.cml)"
            << tr("Cube") + "(*.cub)" << tr("All files") + "(* *.*)";

    FileName = QFileDialog::getOpenFileName(
        this, tr("Open File"), QDir::currentPath(), filters.join(";;"));
    if (FileName.isEmpty())
        return;

    loadFile(FileName);
}

void MainWindow::loadFile(QString filename) {
    if (filename.isEmpty())
        return;
    /*
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QMessageBox::information(0, "Warning","This File is not existed!");
      return;
  }
  */
    QFileInfo info_(filename);
    if (!info_.isFile()) {
        QMessageBox::information(0, "Warning", "This File is not existed!");
        return;
    }

    mol->clearAll();
    mol->setParent(this);
    mol->setView3D(view3d);
    mol->loadFile(filename);

    if (mol->NumAtoms() < 1) {
        displayMessage("Cannot open file");
        return;
    }

    updatePanelParameters();

    view3d->setSymmetry("C1");
    view3d->renderMol(mol);

    push2Stack("newFile");

    ui.comboBox_example->setCurrentIndex(0);

    isFirstTime = false;
}

void MainWindow::saveFile(QString filename) {}

void MainWindow::on_actionFileSave2PNG_triggered() {
    if (ui.tabWidget->currentIndex() > 0)
        ui.tabWidget->setCurrentIndex(0);

    QStringList filters;
    filters << tr("png") + "(*.png)";

    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save File"), QDir::currentPath(), filters.join(";;"));

    if (fileName.isEmpty()) {
        // QMessageBox::information(0, "Warning","Please input file name!");
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (!fileName.endsWith("png"))
        fileName += ".png";

    view3d->writePNG(fileName);
    view3d->refresh();

    QApplication::restoreOverrideCursor();
}

void MainWindow::displayMolCoordinates(HMol *mol) {
    /*
  unsigned int NAtoms=mol->NumAtoms();
  if (NAtoms<1) return;

  QString single="           Atom  Coordinates";
  sendMessage(single);
  single="--------------------------------------------------";
  sendMessage(single);

  double x,y,z;
  for (unsigned int i=0;i<NAtoms;i++) {
      QString a=mol->atomSymbol(i) + "    ";
      x=mol->atomPos(i).x();
      if(x<0.)   a+=QString::number(x,'f',4)+ " ";
      else       a+=QString::number(x,'f',4)+ " ";

      y=mol->atomPos(i).y();
      if(y<0.)   a+=QString::number(y,'f',4)+ " ";
      else       a+=QString::number(y,'f',4)+ " ";

      z=mol->atomPos(i).z();
      if(z<0.)   a+=QString::number(z,'f',4)+ " ";
      else       a+=QString::number(z,'f',4)+ " ";

      sendMessage(a);
  }
  single="-------------------------------------------------\n\n";
  sendMessage(single);
  */
}

void MainWindow::setTemplateName(QString name) { nameTMol = name; }

void MainWindow::loadTemplateMol() {
    QString fileName = nameTMol + ".frg";
    QString iconName = nameTMol + ".xbm";

    isMetalTemplate = false;
    if (nameTMol.contains("tripla") || nameTMol == "tetsss" ||
        nameTMol == "tet" || nameTMol == "seesaw" || nameTMol == "oct" ||
        nameTMol == "captrig" || nameTMol == "pentbipyr" ||
        nameTMol == "sqaprism") {
        iconName = nameTMol + ".svg";
        isMetalTemplate = true;
    }

    // cout << path.toStdString().c_str()<<endl;
    QString path =
        QCoreApplication::applicationDirPath() + "/template/" + fileName;

    if (isMetalTemplate)
        view3d->loadMetalTemplateMol(path);
    else
        view3d->loadTemplateMol(path);

    view3d->setLinkTemplateMode();

    // ui.mdiArea->tileSubWindows();
    isFirstTime = false;
}

QString MainWindow::getExecutePath() {
    return QCoreApplication::applicationDirPath();
}

void MainWindow::on_pushButton_clean_clicked() {
    on_actionMMFF94_triggered();
    return;
}

void MainWindow::on_pushButton_translate_clicked() {
    if (mol == nullptr)
        return;

    float x, y, z, err = 0.0000001;
    x = (ui.lineEdit_X->text()).toFloat();
    y = (ui.lineEdit_Y->text()).toFloat();
    z = (ui.lineEdit_Z->text()).toFloat();

    if (abs(x) < err && abs(y) < err && abs(z) < err) {
        QMessageBox::information(0, "Warning", "Please input rotation degree!");
        return;
    }

    if (abs(x) > err && abs(y) > err) {
        QMessageBox::information(0, "Warning", "Only one axis can be defined!");
        return;
    }

    if (abs(y) > err && abs(z) > err) {
        QMessageBox::information(0, "Warning", "Only one axis can be defined!");
        return;
    }

    if (abs(x) > err && abs(z) > err) {
        QMessageBox::information(0, "Warning", "Only one axis can be defined!");
        return;
    }

    mol->translate(x, y, z);

    view3d->renderMol(mol);

    push2Stack("translateMol");

    view3d->setDefaultOperationMode();
}

void MainWindow::on_pushButton_rotate_clicked() {
    if (mol == nullptr)
        return;

    float x, y, z, err = 0.01;
    x = (ui.lineEdit_X->text()).toFloat();
    y = (ui.lineEdit_Y->text()).toFloat();
    z = (ui.lineEdit_Z->text()).toFloat();

    if (abs(x) < err && abs(y) < err && abs(z) < err) {
        QMessageBox::information(0, "Warning", "Please input rotation degree!");
        return;
    }

    if (abs(x) > err && abs(y) > err) {
        QMessageBox::information(0, "Warning", "Only one axis can be defined!");
        return;
    }

    if (abs(y) > err && abs(z) > err) {
        QMessageBox::information(0, "Warning", "Only one axis can be defined!");
        return;
    }

    if (abs(x) > err && abs(z) > err) {
        QMessageBox::information(0, "Warning", "Only one axis can be defined!");
        return;
    }

    vector3 v(0.0, 0.0, 0.0);

    if (abs(x) > err) {
        v.SetX(1.0);
        mol->rotate(v, x);
    }

    if (abs(y) > err) {
        v.SetY(1.0);
        mol->rotate(v, y);
    }

    if (abs(z) > err) {

        v.SetZ(1.0);
        mol->rotate(v, z);
    }

    view3d->renderMol();
    push2Stack("rotateMol");

    view3d->setDefaultOperationMode();
}

void MainWindow::on_pushButton_cleardata_clicked() {
    ui.lineEdit_X->clear();
    ui.lineEdit_Y->clear();
    ui.lineEdit_Z->clear();
}

// store and push
void MainWindow::push2Stack(QString command) {
    displayMessage(command);

    buildMolData();
    dataMol = command + "\n" + dataMol;
    undoList.push_back(dataMol);
    // cout << undoList.size()<< "     sfdsdsssd"<<endl;

    ui.textEdit_dataMol->setText(dataMol0);
    isFirstTime = false;
}

void MainWindow::push2Stack_selection(QString command) {
    updateLastStackOne();
    push2Stack(command);
}

void MainWindow::updateLastStackOne() {
    dataMol = undoList.pop();
    dataMol += buildSelectedAtomBondInfo();
    undoList.push(dataMol);
}

void MainWindow::buildMolData() {
    // commands=     newFile     cleanMol     runXTB     centralizeMol addHydrogen
    //               translateMol     rotateMol     linkTemplate     newTemplate
    //               removeSelectedAtomBond removeSelectedBonds
    //               removeSelectedAtoms     pastefromClipboard

    if (mol == nullptr)
        return;
    // cout << mol->NumAtoms() << endl;
    if (mol->NumAtoms() < 1)
        return;

    // if(!redoList.isEmpty()) redoList.clear();

    // atoms ---------------------------------------
    // Symbol x     y    z  radius  color linked-atoms
    // C   0.0  0.0  0.0  0.3  uint32_t  1 2 3 4
    dataMol = "";
    dataMol += "Atoms  " + QString::number(mol->NumAtoms()) + "\n";

    dataMol0 = "";
    QString d = "", s;

    for (unsigned int i = 0; i < mol->NumAtoms(); i++) {
        s = QString("%1     %2     %3     %4\n")
        .arg(mol->atomSymbol(i), 3)
            .arg(mol->atomPos(i).x(), 10, 'f', 6)
            .arg(mol->atomPos(i).y(), 10, 'f', 6)
            .arg(mol->atomPos(i).z(), 10, 'f', 6);

        dataMol0 += s;

        QString a = mol->atomSymbol(i) + "    " +
                    QString::number(mol->atomPos(i).x(), 'f', 6) + "    " +
                    QString::number(mol->atomPos(i).y(), 'f', 6) + "    " +
                    QString::number(mol->atomPos(i).z(), 'f', 6) + "    "

                    + QString::number(mol->getAtombyIndex(i)->radius(), 'f', 2) +
                    "    " + QString::number(mol->atomList[i]->Color()) + "    ";

        QString b = "";
        for (unsigned int j = 0; j < mol->getAtombyIndex(i)->numLinkdedAtoms(); j++)
            b += QString::number(mol->getAtombyIndex(i)->getLinkdedAtomId(j)) + " ";
        b += "\n";
        d += a;
        d += b;
    }
    dataMol += d;

    // bonds
    QString title = "Bonds  " + QString::number(mol->NumBonds()) + "\n";
    dataMol += title;
    d = "";

    // id0 id1 order   plane <x  y  z>=VZero  <idRing>=100001
    for (unsigned int i = 0; i < mol->NumBonds(); i++) {
        QString a = "", b = "", c = "";
        a = QString::number(mol->getBondbyIndex(i)->atomIndex0()) + " " +
            QString::number(mol->getBondbyIndex(i)->atomIndex1()) + " " +
            QString::number(mol->getBondbyIndex(i)->getBondOrder()) + " ";

        vector3 norm = VZero;
        if (mol->getBondbyIndex(i)->getPlane().length() > 0.1)
            norm = mol->getBondbyIndex(i)->getPlane();

        b = QString::number(mol->getBondbyIndex(i)->getPlane().x(), 'f', 2) + " " +
            QString::number(mol->getBondbyIndex(i)->getPlane().y(), 'f', 2) + " " +
            QString::number(mol->getBondbyIndex(i)->getPlane().z(), 'f', 2) + " ";

        c = a + b;
        d += c;

        // id ring?
        QString idRing = "";
        if (mol->getBondbyIndex(i)->isInsideRing()) {
            for (auto j : mol->getBondbyIndex(i)->getRingId())
                idRing += " " + QString::number(j);
        }
        d += idRing + "\n";
    }
    dataMol += d;

    // rings data stored:
    // size  < id of atoms> , <center>     < normal >,       <radius> ,  scale,
    // color  ,  isAromatic 6 :  1 0 5 4 3 2 , -1.06 -0.62 0.00 , 0.00
    // -0.00 1.00, 1.40 ,  2342342342, 0.5,       1
    title = "Rings  " + QString::number(mol->numRings()) + "\n";
    dataMol += title;
    d = "";
    for (unsigned int i = 0; i < mol->numRings(); i++) {
        QString a = "", b = "";
        unsigned int size = mol->getRingbyId(i)->atomIdList.size();
        a = QString::number(size) + " :  ";

        for (unsigned int j = 0; j < size; j++)
            b += QString::number(mol->getRingbyId(i)->atomIdList[j]) + " ";
        a += b;

        // center
        vector3 v = mol->getRingbyId(i)->center;
        QString center = QString::number(v.x(), 'f', 2) + " " +
                         QString::number(v.y(), 'f', 2) + " " +
                         QString::number(v.z(), 'f', 2) + ", ";
        a += center;

        // normal
        v = mol->getRingbyId(i)->norm;
        QString norm = QString::number(v.x(), 'f', 2) + " " +
                       QString::number(v.y(), 'f', 2) + " " +
                       QString::number(v.z(), 'f', 2) + ", ";
        a += norm;

        // radius
        QString radius =
            QString::number(mol->getRingbyId(i)->radius, 'f', 2) + ", ";
        a += radius;

        // scale
        QString scale = QString::number(mol->getRingbyId(i)->scale, 'f', 2) + ", ";
        a += scale;

        // color
        QString color =
            QString::number(mol->getRingbyId(i)->color.x(), 'f', 2) + " " +
            QString::number(mol->getRingbyId(i)->color.y(), 'f', 2) + " " +
            QString::number(mol->getRingbyId(i)->color.z(), 'f', 2) + ", ";
        a += color;

        // aromatic?
        if (mol->getRingbyId(i)->isAromatic)
            a += QString::number(1) + "\n";
        else
            a += QString::number(0) + "\n";

        d += a;
    }
    dataMol += d;

    // cout << "\n\nsaved data:"<<endl;
    // cout << dataMol.toStdString().c_str()<<endl;
    // cout << "\n\n";
}

// help function
QString MainWindow::buildSelectedAtomBondInfo() {
    QString All = "";

    // selected Atoms
    unsigned int nAtoms = view3d->numSelectedAtoms();
    if (nAtoms > 1) {
        QString title = "SelectedAtoms  " + QString::number(nAtoms) + "\n";
        QString atomsList = "";
        for (unsigned int i = 0; i < view3d->numSelectedAtoms(); i++)
            atomsList += QString::number(view3d->getSelectedAtomId(i)) + " ";
        atomsList = title + atomsList + " \n";
        All += atomsList;
    }

    // selected Bonds
    unsigned int nBonds = view3d->numSelectedBonds();
    if (nBonds > 1) {
        QString bondsList = "";
        QString title = "SelectedBonds  " + QString::number(nBonds) + "\n";
        for (unsigned int i = 0; i < view3d->numSelectedBonds(); i++) {
            unsigned int ibeg = 0, iend = 0;
            view3d->getSelectedBondIds(i, ibeg, iend);

            // only the index is stored
            bondsList += QString::number(i) + "  ";
        }
        bondsList = title + bondsList + " \n";
        All += bondsList;
    }

    return All;
}

void MainWindow::recoverStackData(QString &All) {
    view3d->releaseAllActors();
    mol->clearAll();

    QStringList Lines = All.split("\n");
    if (Lines.size() < 1)
        return;

    QString single;
    QStringList ls;
    unsigned int nAtoms = 0, nBonds = 0, nRings = 0;

    QString command = Lines.at(0);

    cout << "recover :  " << endl;
    cout << All.toStdString().c_str() << endl;

    vector<unsigned int> selAtoms, selBonds;
    for (unsigned int i = 1; i < Lines.size(); i++) {
        single = Lines.at(i);

        unsigned int nAtoms0 = 0;
        // atoms ---------------------------------------
        // Symbol x     y    z   radius  color neighbors;
        // C   0.0  0.0  0.0  0.3  uint32_t  1 2 3 4
        if (single.contains("Atoms")) {
            ls = single.trimmed().split(QRegularExpression("\\s+"));
            nAtoms = ls[1].toInt();

            if (nAtoms < 1)
                return;
            nAtoms0 = nAtoms;

            dataMol0 = "";
            QString s;
            double x, y, z;

            while (nAtoms--) {
                i = i + 1;
                single = Lines.at(i);
                // single.replace(":", " ");
                // cout <<single .toStdString().c_str()<< "    sssssssssssss"<< endl;
                ls = single.trimmed().split(QRegularExpression("\\s+"));

                x = ls[1].toDouble();
                y = ls[2].toDouble();
                z = ls[3].toDouble();

                s = QString("%1     %2     %3    %4\n")
                        .arg(ls[0], -3)
                        .arg(x, 10, 'f', 6)
                        .arg(y, 10, 'f', 6)
                        .arg(z, 10, 'f', 6);
                dataMol0 += s;

                mol->addAtom(ls[0], x, y, z);

                // radius and color
                mol->getLastAtom()->setRadius(ls[4].toDouble());
                mol->getLastAtom()->setColor(ls[5].toULong());

                // cout << ls[0].toStdString().c_str() <<" "<<ls[1].toDouble() <<" "<<
                // ls[2].toDouble()<<" "<<ls[3].toDouble() <<" "; cout <<
                // ls[4].toDouble() << " "<<ls[5].toULong() ;

                if (ls.size() > 6)
                    for (unsigned int k = 6; k < ls.size(); k++)
                        mol->getLastAtom()->appendNeighbor(ls[k].toInt());
            }

            if (nAtoms0 < 2) {
                view3d->renderMol(mol);
                return;
            }

            continue;
        }

        // beg end order plane ringId
        unsigned int nBonds0 = 0;
        if (single.contains("Bonds")) {
            ls = single.trimmed().split(QRegularExpression("\\s+"));
            nBonds = ls[1].toInt();
            if (nBonds < 1)
                return;

            nBonds0 = nBonds;

            // bond
            // cout << "recovered bond infor:"<<endl;
            while (nBonds--) {
                i = i + 1;
                single = Lines.at(i);
                // cout <<single .toStdString().c_str()<<endl;
                ls = single.trimmed().split(QRegularExpression("\\s+"));
                mol->addBond(ls[0].toInt(), ls[1].toInt(), ls[2].toInt());
                mol->getLastBond()->setPlane(ls[3].toDouble(), ls[4].toDouble(),
                                             ls[5].toDouble());

                if (ls.size() > 6)
                    mol->getLastBond()->appendRingId(ls[6].toInt());
                if (ls.size() > 7)
                    mol->getLastBond()->appendRingId(ls[7].toInt());
            }

            if (nBonds0 < 2) {
                view3d->renderMol(mol);
                return;
            }

            continue;
        }

        // rings data stored:
        // size  < id of atoms> , <center>     < normal >,       <radius> ,  scale,
        // color  ,  isAromatic 6 :  1 0 5 4 3 2 , -1.06 -0.62 0.00 , 0.00
        // -0.00 1.00, 1.40 ,   0.00 -0.00 1.00 , 0.5,       1
        if (single.contains("Rings")) {
            ls = single.trimmed().split(QRegularExpression("\\s+"));
            nRings = ls[1].toInt();

            vector<unsigned int> idList;
            while (nRings--) {
                i = i + 1;
                single = Lines.at(i);
                single.replace(",", " ");
                single.replace(":", " ");

                ls = single.trimmed().split(QRegularExpression("\\s+"));

                idList.clear();
                unsigned int size = ls[0].toInt();
                for (unsigned int j = 0; j < size; j++)
                    idList.push_back(ls[j + 1].toInt());

                vector3 ringCenter(ls[size + 1].toDouble(), ls[size + 2].toDouble(),
                                   ls[size + 3].toDouble());

                vector3 ringNormal(ls[size + 4].toDouble(), ls[size + 5].toDouble(),
                                   ls[size + 6].toDouble());

                double ringRadius = ls[size + 7].toDouble();
                double ringScale = ls[size + 8].toDouble();

                vector3 color(ls[size + 9].toDouble(), ls[size + 10].toDouble(),
                              ls[size + 11].toDouble());

                unsigned int ringAromatic = ls[size + 12].toInt();

                mol->addRing(idList, ringCenter, ringNormal, ringRadius, ringScale,
                             color, ringAromatic);
            }
        }
    }

    view3d->renderMol(mol);
}

void MainWindow::on_checkBox_kekule_stateChanged(int arg1) {
    view3d->setKekuleStyle(arg1);
    view3d->updateMol();
}


void MainWindow::symmetryPercept()
{
    on_pushButton_Symmetrypercept_PG_clicked();
}

void MainWindow::on_pushButton_Symmetrypercept_PG_clicked()
{
    initSymmetry();

    if (mol == nullptr)
        return;
    if (mol->NumAtoms() < 2) {
        QMessageBox::information(0, "Warning", "Empty molecule or single Atoms");
        return;
    }



    view3d->setDefaultOperationMode();
    view3d->releaseAllActors();

    double tol = ui.doubleSpinBox_tolerance->value() / 2.0;

    mol->centralize();
    PGSymmetry *symmetry = new PGSymmetry(mol, tol);
    symmetry->PerceptSymmetry(true);

    string pg;
    symmetry->report(pg);

    QString PG = QString::fromStdString(pg);
    int idx = ui.comboBox_symmetry->findText(PG);

    mol->setPointGroup(PG);

    view3d->setSymmetry(PG);
    view3d->updateMolLength();

    view3d->setSymmetryElementVisible(true);
    view3d->setDefaultOperationMode();
    view3d->onViewXY();
    //view3d->setOutLineType(1);

    if (!PG.contains("C1")) {
        _isC1 = true;
        mol->rotate(symmetry->getstandOrientMatrix());
        push2Stack("Rotate");
    }

    if (idx > -1)
        ui.comboBox_symmetry->setCurrentIndex(idx);
    else {
        ui.comboBox_symmetry->insertItem(0, PG);
        ui.comboBox_symmetry->setCurrentIndex(0);
    }

    updateBasedOnSymmetry(PG);

    isFirstTime = true;
    ui.checkBox_PGElement->setCheckState(Qt::Checked);
    ui.checkBox_CnHide->setCheckState(Qt::Unchecked);
    isFirstTime = false;

    view3d->renderMol();

    delete symmetry;
}

void MainWindow::initSymmetry() {
    _isCnv = _isCn = _isCnh = false;
    _isDnd = _isDnh = _isDn = false;
    _isT = _isTd = _isTh = false;
    _isO = _isOh = false;
    _isI = _isIh = false;
    _isC00v = _isD00h = false;
    _isCs = _isCi = _isCs = _isSn = false;
}

void MainWindow::disable_Cn() {
    ui.checkBox_CnHide->setCheckState(Qt::Unchecked);
    ui.pushButton_Cn->setEnabled(false);
    ui.checkBox_CnHide->setEnabled(false);
    ui.doubleSpinBox_Cn_length->setEnabled(false);
    ui.doubleSpinBox_diskSize_Cn->setEnabled(false);
    ui.doubleSpinBox_radius_Cn->setEnabled(false);
    ui.doubleSpinBox_thickness_Cn->setEnabled(false);
    ui.spinBox_Cn_red->setEnabled(false);
    ui.spinBox_Cn_blue->setEnabled(false);
    ui.spinBox_Cn_green->setEnabled(false);
}

void MainWindow::disable_C3() {
    ui.checkBox_C3Hide->setCheckState(Qt::Unchecked);
    ui.pushButton_C3->setEnabled(false);
    ui.checkBox_C3Hide->setEnabled(false);
    ui.doubleSpinBox_C3_length->setEnabled(false);
    ui.doubleSpinBox_diskSize_C3->setEnabled(false);
    ui.doubleSpinBox_radius_C3->setEnabled(false);
    ui.doubleSpinBox_thickness_C3->setEnabled(false);
    ui.spinBox_C3_red->setEnabled(false);
    ui.spinBox_C3_blue->setEnabled(false);
    ui.spinBox_C3_green->setEnabled(false);
}

void MainWindow::disable_C2() {
    ui.checkBox_C2Hide->setCheckState(Qt::Unchecked);
    ui.pushButton_C2->setEnabled(false);
    ui.checkBox_C2Hide->setEnabled(false);
    ui.doubleSpinBox_C2_length->setEnabled(false);
    ui.doubleSpinBox_diskSize_C2->setEnabled(false);

    ui.doubleSpinBox_radius_C2->setEnabled(false);
    ui.doubleSpinBox_thickness_C2->setEnabled(false);

    ui.spinBox_C2_red->setEnabled(false);
    ui.spinBox_C2_blue->setEnabled(false);
    ui.spinBox_C2_green->setEnabled(false);
}

void MainWindow::disable_SigmaV() {
    ui.checkBox_SigmaV_Hide->setCheckState(Qt::Unchecked);
    ui.checkBox_SigmaV_Hide->setEnabled(false);
    ui.pushButton_V->setEnabled(false);
    ui.spinBox_V_type->setEnabled(false);
    ui.doubleSpinBox_V_length->setEnabled(false);
    ui.doubleSpinBox_V_thickness->setEnabled(false);
    ui.spinBox_V_red->setEnabled(false);
    ui.spinBox_V_blue->setEnabled(false);
    ui.spinBox_V_green->setEnabled(false);
    ui.spinBox_V_opacity->setEnabled(false);
}

void MainWindow::disable_SigmaH() {
    ui.checkBox_SigmaH_Hide->setCheckState(Qt::Unchecked);

    ui.checkBox_SigmaH_Hide->setEnabled(false);
    ui.pushButton_H->setEnabled(false);
    ui.spinBox_H_type->setEnabled(false);
    ui.doubleSpinBox_H_length->setEnabled(false);
    ui.doubleSpinBox_H_thickness->setEnabled(false);
    ui.spinBox_H_red->setEnabled(false);
    ui.spinBox_H_blue->setEnabled(false);
    ui.spinBox_H_green->setEnabled(false);
    ui.spinBox_H_opacity->setEnabled(false);
}

void MainWindow::disable_SigmaD() {
    ui.checkBox_SigmaD_Hide->setEnabled(false);
    ui.pushButton_D->setEnabled(false);
    ui.spinBox_D_type->setEnabled(false);
    ui.doubleSpinBox_D_length->setEnabled(false);
    ui.doubleSpinBox_D_thickness->setEnabled(false);
    ui.spinBox_D_red->setEnabled(false);
    ui.spinBox_D_blue->setEnabled(false);
    ui.spinBox_D_green->setEnabled(false);
    ui.spinBox_D_opacity->setEnabled(false);
}

void MainWindow::on_pushButton_SymmetryRefine_PG_clicked() {
    if (mol == nullptr)
        return;
    if (mol->NumAtoms() < 2)
        return;

    double tol = ui.doubleSpinBox_tolerance->value();

    PGSymmetry *symmetry = new PGSymmetry(mol, tol);
    QString SymmetrySymbol = ui.comboBox_symmetry->currentText();

    if (tol > 1.0) {
        QMessageBox::information(0, "Warning",
                                 "Wrong results may obtaied when tolerance > 1.0");
    }

    symmetry->ClearAll();
    symmetry->setSymmetry(SymmetrySymbol.toStdString());

    if (!symmetry->refine()) {
        QMessageBox::information(
            0, "Warning",
            "Sorry! Cannot refined based on " + SymmetrySymbol +
                " symmetry!\n Please reduce or enhance the tolerance and try!");
        return;
    }

    symmetry->updateMolData();

    push2Stack("Refine Molecule Data");

    view3d->refresh();

    // QString single="\n\n     refined data ";
    displayMolCoordinates(mol);
}

// update panel from view3d
//-----------------------------------------------
void MainWindow::setMolVisible() {
    if (!ui.checkBox_HideMol->isChecked()) {
        ui.checkBox_HideMol->setCheckState(Qt::Checked);
    } else {
        ui.checkBox_HideMol->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::setSymmetryElementVisible() {
    if (!ui.checkBox_PGElement->isChecked()) {
        ui.checkBox_PGElement->setCheckState(Qt::Checked);
    } else {
        ui.checkBox_PGElement->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::setXYZAxisVisible() {
    if (!ui.checkBox_xyzAxis->isChecked()) {
        ui.checkBox_xyzAxis->setCheckState(Qt::Checked);
    } else {
        ui.checkBox_xyzAxis->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::setOutLineVisible() {
    if (!ui.checkBox_outline->isChecked()) {
        ui.checkBox_outline->setCheckState(Qt::Checked);
    } else {
        ui.checkBox_outline->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::setAtomLabelVisible() {
    if (!ui.checkBox_atomLabel->isChecked()) {
        ui.checkBox_atomLabel->setCheckState(Qt::Checked);
    } else {
        ui.checkBox_atomLabel->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::setSigmaV_Visible() {
    if (ui.pushButton_V->isEnabled()) {
        if (!ui.checkBox_SigmaV_Hide->isChecked())
            ui.checkBox_SigmaV_Hide->setCheckState(Qt::Checked);
        else
            ui.checkBox_SigmaV_Hide->setCheckState(Qt::Unchecked);
    } else {
        if (!ui.pushButton_D->text().endsWith("v"))
            return;

        if (!ui.checkBox_SigmaD_Hide->isChecked())
            ui.checkBox_SigmaD_Hide->setCheckState(Qt::Checked);
        else
            ui.checkBox_SigmaD_Hide->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::setSigmaD_Visible() {
    if (!ui.pushButton_D->isEnabled())
        return;

    if (!ui.pushButton_V->isEnabled()) {
        if (ui.pushButton_D->text().endsWith("v"))
            return;
    }

    if (!ui.checkBox_SigmaD_Hide->isChecked()) {
        ui.checkBox_SigmaD_Hide->setCheckState(Qt::Checked);
    } else {
        ui.checkBox_SigmaD_Hide->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::setSigmaH_Visible() {
    if (!ui.checkBox_SigmaH_Hide->isChecked()) {
        ui.checkBox_SigmaH_Hide->setCheckState(Qt::Checked);
    } else {
        ui.checkBox_SigmaH_Hide->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::modifycheckBox_SigmaH_Hide(bool t) {
    if (t)
        ui.checkBox_SigmaH_Hide->setCheckState(Qt::Checked);
    else
        ui.checkBox_SigmaH_Hide->setCheckState(Qt::Unchecked);
}

void MainWindow::modifycheckBox_SigmaV_Hide(bool t) {
    if (t)
        ui.checkBox_SigmaV_Hide->setCheckState(Qt::Checked);
    else
        ui.checkBox_SigmaV_Hide->setCheckState(Qt::Unchecked);
}

void MainWindow::modifycheckBox_SigmaD_Hide(bool t) {
    if (t)
        ui.checkBox_SigmaD_Hide->setCheckState(Qt::Checked);
    else
        ui.checkBox_SigmaD_Hide->setCheckState(Qt::Unchecked);
}

void MainWindow::setSigmaD_Visible(uint type) {
    // 0
    switch (type) {
    case 0:
        ui.checkBox_SigmaD_Hide->blockSignals(true);
        ui.checkBox_SigmaD_Hide->setCheckState(Qt::Unchecked);
        ui.checkBox_SigmaD_Hide->blockSignals(false);

        if (ui.pushButton_V->text().endsWith("d")) {
            ui.checkBox_SigmaD_Hide->blockSignals(true);
            ui.checkBox_SigmaD_Hide->setCheckState(Qt::Unchecked);
            ui.checkBox_SigmaD_Hide->blockSignals(false);
        }
        break;

    case 1:
        ui.checkBox_SigmaD_Hide->blockSignals(true);
        ui.checkBox_SigmaD_Hide->setCheckState(Qt::Unchecked);
        ui.checkBox_SigmaD_Hide->blockSignals(false);
        break;

    case 2:
        if (ui.pushButton_V->text().endsWith("d")) {
            ui.checkBox_SigmaD_Hide->blockSignals(true);
            ui.checkBox_SigmaD_Hide->setCheckState(Qt::Unchecked);
            ui.checkBox_SigmaD_Hide->blockSignals(false);
        }
        break;
    }
}

void MainWindow::setSigmaV_Visible(uint type) {
    switch (type) {
    case 0:
        ui.checkBox_SigmaV_Hide->blockSignals(true);
        ui.checkBox_SigmaV_Hide->setCheckState(Qt::Unchecked);
        ui.checkBox_SigmaV_Hide->blockSignals(false);

        if (ui.pushButton_D->text().endsWith("v")) {
            ui.checkBox_SigmaD_Hide->blockSignals(true);
            ui.checkBox_SigmaD_Hide->setCheckState(Qt::Unchecked);
            ui.checkBox_SigmaD_Hide->blockSignals(false);
        }
        break;

    case 1:
        ui.checkBox_SigmaV_Hide->blockSignals(true);
        ui.checkBox_SigmaV_Hide->setCheckState(Qt::Unchecked);
        ui.checkBox_SigmaV_Hide->blockSignals(false);
        break;

    case 2:
        if (ui.pushButton_D->text().endsWith("v")) {
            ui.checkBox_SigmaD_Hide->blockSignals(true);
            ui.checkBox_SigmaD_Hide->setCheckState(Qt::Unchecked);
            ui.checkBox_SigmaD_Hide->blockSignals(false);
        }
        break;
    }
}

void MainWindow::setSigmaV_Invisible(uint type) {
    // 0
    switch (type) {
    case 0:
        ui.checkBox_SigmaV_Hide->blockSignals(true);
        ui.checkBox_SigmaV_Hide->setCheckState(Qt::Checked);
        ui.checkBox_SigmaV_Hide->blockSignals(false);

        if (ui.pushButton_D->text().endsWith("v")) {
            ui.checkBox_SigmaD_Hide->blockSignals(true);
            ui.checkBox_SigmaD_Hide->setCheckState(Qt::Checked);
            ui.checkBox_SigmaD_Hide->blockSignals(false);
        }
        break;

    case 1:
        ui.checkBox_SigmaV_Hide->blockSignals(true);
        ui.checkBox_SigmaV_Hide->setCheckState(Qt::Checked);
        ui.checkBox_SigmaV_Hide->blockSignals(false);
        break;

    case 2:
        if (ui.pushButton_D->text().endsWith("v")) {
            ui.checkBox_SigmaD_Hide->blockSignals(true);
            ui.checkBox_SigmaD_Hide->setCheckState(Qt::Checked);
            ui.checkBox_SigmaD_Hide->blockSignals(false);
        }
        break;
    }
}

void MainWindow::setSigmaD_Invisible(uint type) {
    // 0
    switch (type) {
    case 0:
        ui.checkBox_SigmaD_Hide->blockSignals(true);
        ui.checkBox_SigmaD_Hide->setCheckState(Qt::Checked);
        ui.checkBox_SigmaD_Hide->blockSignals(false);

        if (ui.pushButton_V->text().endsWith("d")) {
            ui.checkBox_SigmaV_Hide->blockSignals(true);
            ui.checkBox_SigmaV_Hide->setCheckState(Qt::Checked);
            ui.checkBox_SigmaV_Hide->blockSignals(false);
        }
        break;

    case 1:
        ui.checkBox_SigmaD_Hide->blockSignals(true);
        ui.checkBox_SigmaD_Hide->setCheckState(Qt::Checked);
        ui.checkBox_SigmaD_Hide->blockSignals(false);
        break;

    case 2:
        if (ui.pushButton_V->text().endsWith("d")) {
            ui.checkBox_SigmaV_Hide->blockSignals(true);
            ui.checkBox_SigmaV_Hide->setCheckState(Qt::Checked);
            ui.checkBox_SigmaV_Hide->blockSignals(false);
        }
        break;
    }
}

void MainWindow::setSigmaH_Visible(uint type) {
    // 0
    switch (type) {
    case 0:
        ui.checkBox_SigmaH_Hide->blockSignals(true);
        ui.checkBox_SigmaH_Hide->setCheckState(Qt::Unchecked);
        ui.checkBox_SigmaH_Hide->blockSignals(false);

        break;

    case 1:
        ui.checkBox_SigmaH_Hide->blockSignals(true);
        ui.checkBox_SigmaH_Hide->setCheckState(Qt::Unchecked);
        ui.checkBox_SigmaH_Hide->blockSignals(false);
        break;

    case 2:
        ui.checkBox_SigmaH_Hide->blockSignals(true);
        ui.checkBox_SigmaH_Hide->setCheckState(Qt::Unchecked);
        ui.checkBox_SigmaH_Hide->blockSignals(false);
        break;
    }
}

void MainWindow::setSigmaH_Invisible(uint type) {
    // 0
    switch (type) {
    case 0:
        ui.checkBox_SigmaH_Hide->blockSignals(false);
        ui.checkBox_SigmaH_Hide->setCheckState(Qt::Checked);
        ui.checkBox_SigmaH_Hide->blockSignals(false);
        break;

    case 1:
        ui.checkBox_SigmaH_Hide->blockSignals(false);
        ui.checkBox_SigmaH_Hide->setCheckState(Qt::Checked);
        ui.checkBox_SigmaH_Hide->blockSignals(false);
        break;

    case 2:
        ui.checkBox_SigmaH_Hide->blockSignals(false);
        ui.checkBox_SigmaH_Hide->setCheckState(Qt::Checked);
        ui.checkBox_SigmaH_Hide->blockSignals(false);
        break;
    }
}

void MainWindow::setAllC2_Visible(bool t) {
    ui.checkBox_C2Hide->blockSignals(false);
    ui.checkBox_C3Hide->blockSignals(false);

    if (t) {
        ui.checkBox_C2Hide->setCheckState(Qt::Unchecked);
        if (isC3toC2)
            ui.checkBox_C3Hide->setCheckState(Qt::Unchecked);
    } else {
        ui.checkBox_C2Hide->setCheckState(Qt::Checked);
        if (isC3toC2)
            ui.checkBox_C3Hide->setCheckState(Qt::Checked);
    }
}

void MainWindow::setAllC3_Visible(bool t) {
    ui.checkBox_C3Hide->blockSignals(false);

    if (t) {
        ui.checkBox_C3Hide->setCheckState(Qt::Unchecked);
    } else {
        ui.checkBox_C3Hide->setCheckState(Qt::Checked);
    }
}

void MainWindow::setC2_Visible(uint type) {
    // 0
    switch (type) {
    case 0:
        ui.checkBox_C2Hide->blockSignals(true);
        ui.checkBox_C2Hide->setCheckState(Qt::Unchecked);
        ui.checkBox_C2Hide->blockSignals(false);

        if (isC3toC2) {
            ui.checkBox_C3Hide->blockSignals(true);
            ui.checkBox_C3Hide->setCheckState(Qt::Unchecked);
            ui.checkBox_C3Hide->blockSignals(false);
        }
        break;

    case 1:
        ui.checkBox_C2Hide->blockSignals(true);
        ui.checkBox_C2Hide->setCheckState(Qt::Unchecked);
        ui.checkBox_C2Hide->blockSignals(false);
        break;

    case 2:
        ui.checkBox_C3Hide->blockSignals(true);
        ui.checkBox_C3Hide->setCheckState(Qt::Unchecked);
        ui.checkBox_C3Hide->blockSignals(false);
        break;
    }
}

void MainWindow::setC2_Invisible(uint type) {
    // 0
    switch (type) {
    case 0:
        ui.checkBox_C2Hide->blockSignals(true);
        ui.checkBox_C2Hide->setCheckState(Qt::Checked);
        ui.checkBox_C2Hide->blockSignals(false);

        if (isC3toC2) {
            ui.checkBox_C3Hide->blockSignals(true);
            ui.checkBox_C3Hide->setCheckState(Qt::Checked);
            ui.checkBox_C3Hide->blockSignals(false);
        }
        break;

    case 1:
        ui.checkBox_C2Hide->blockSignals(true);
        ui.checkBox_C2Hide->setCheckState(Qt::Checked);
        ui.checkBox_C2Hide->blockSignals(false);
        break;

    case 2:
        ui.checkBox_C3Hide->blockSignals(true);
        ui.checkBox_C3Hide->setCheckState(Qt::Checked);
        ui.checkBox_C3Hide->blockSignals(false);
        break;
    }
}

void MainWindow::setC3_Visible() {
    if (!ui.checkBox_C3Hide->isEnabled())
        return;
    if (isC3toC2)
        return;

    ui.checkBox_C3Hide->blockSignals(true);

    if (ui.checkBox_C3Hide->isChecked()) {
        ui.checkBox_C3Hide->setCheckState(Qt::Unchecked);
        if (isC3toC2)
            ui.checkBox_C3Hide->setCheckState(Qt::Unchecked);
    } else {
        ui.checkBox_C3Hide->setCheckState(Qt::Checked);
        if (isC3toC2)
            ui.checkBox_C3Hide->setCheckState(Qt::Checked);
    }

    ui.checkBox_C3Hide->blockSignals(false);
}

void MainWindow::setCn_Visible() {
    if (ui.checkBox_CnHide->isChecked()) {
        ui.checkBox_CnHide->setCheckState(Qt::Unchecked);
    } else {
        ui.checkBox_CnHide->setCheckState(Qt::Checked);
    }
}

void MainWindow::setKekuleStyle() {
    if (ui.checkBox_kekule->isChecked())
        ui.checkBox_kekule->setCheckState(Qt::Unchecked);
    else
        ui.checkBox_kekule->setCheckState(Qt::Checked);
}

void MainWindow::setHydrogenVisible() {
    if (ui.checkBox_HideHydrogen->isChecked())
        ui.checkBox_HideHydrogen->setCheckState(Qt::Unchecked);
    else
        ui.checkBox_HideHydrogen->setCheckState(Qt::Checked);
}
//--------------------------------

void MainWindow::on_comboBox_symmetry_currentIndexChanged(int index) {
    if (index < 0)
        return;
    const QString arg1 = ui.comboBox_symmetry->itemText(index);
    view3d->setSymmetry(arg1);
    if (isFirstTime)
        return;

    updateBasedOnSymmetry(arg1);

    view3d->refresh();
}

void MainWindow::updateBasedOnSymmetry(QString arg1) {
    QString PG = arg1;
    ui.treeWidget->setSymmetry(PG);

    if (arg1 == "C1")
        return;

    ui.checkBox_CnHide->setCheckState(Qt::Checked);
    ui.pushButton_Cn->setEnabled(true);
    ui.checkBox_CnHide->setEnabled(true);
    ui.doubleSpinBox_Cn_length->setEnabled(true);
    ui.doubleSpinBox_diskSize_Cn->setEnabled(true);
    ui.doubleSpinBox_radius_Cn->setEnabled(true);
    ui.doubleSpinBox_thickness_Cn->setEnabled(true);
    ui.spinBox_Cn_red->setEnabled(true);
    ui.spinBox_Cn_blue->setEnabled(true);
    ui.spinBox_Cn_green->setEnabled(true);

    QChar sigma = QChar(0xc3, 0x03);
    QString sigmaV = sigma + '-' + 'v';
    QString sigmaD = sigma + '-' + 'd';
    QString sigmaH = sigma + '-' + 'h';

    ui.pushButton_H->setEnabled(true);
    ui.checkBox_SigmaH_Hide->setEnabled(true);

    ui.pushButton_V->setText(QString(sigmaV));
    ui.pushButton_H->setText(QString(sigmaH));
    ui.pushButton_D->setText(QString(sigmaD));

    ui.checkBox_SigmaV_Hide->setEnabled(true);
    ui.pushButton_V->setEnabled(true);
    ui.spinBox_V_type->setEnabled(true);
    ui.doubleSpinBox_V_length->setEnabled(true);
    ui.doubleSpinBox_V_thickness->setEnabled(true);
    ui.spinBox_V_red->setEnabled(true);
    ui.spinBox_V_blue->setEnabled(true);
    ui.spinBox_V_green->setEnabled(true);
    ui.spinBox_V_opacity->setEnabled(true);

    ui.checkBox_SigmaH_Hide->setEnabled(true);
    ui.pushButton_H->setEnabled(true);
    ui.spinBox_H_type->setEnabled(true);
    ui.doubleSpinBox_H_length->setEnabled(true);
    ui.doubleSpinBox_H_thickness->setEnabled(true);
    ui.spinBox_H_red->setEnabled(true);
    ui.spinBox_H_blue->setEnabled(true);
    ui.spinBox_H_green->setEnabled(true);
    ui.spinBox_H_opacity->setEnabled(true);

    ui.checkBox_SigmaD_Hide->setEnabled(true);
    ui.pushButton_D->setEnabled(true);
    ui.spinBox_D_type->setEnabled(true);
    ui.doubleSpinBox_D_length->setEnabled(true);
    ui.doubleSpinBox_D_thickness->setEnabled(true);
    ui.spinBox_D_red->setEnabled(true);
    ui.spinBox_D_blue->setEnabled(true);
    ui.spinBox_D_green->setEnabled(true);
    ui.spinBox_D_opacity->setEnabled(true);

    ui.checkBox_SigmaH_Hide->setEnabled(true);
    ui.pushButton_C2->setEnabled(true);
    ui.pushButton_C3->setEnabled(true);
    ui.checkBox_C2Hide->setEnabled(true);
    ui.checkBox_C3Hide->setEnabled(true);

    ui.doubleSpinBox_C2_length->setEnabled(true);
    ui.doubleSpinBox_C3_length->setEnabled(true);
    ui.doubleSpinBox_diskSize_C2->setEnabled(true);
    ui.doubleSpinBox_diskSize_C3->setEnabled(true);

    ui.doubleSpinBox_radius_C2->setEnabled(true);
    ui.doubleSpinBox_radius_C3->setEnabled(true);

    ui.doubleSpinBox_thickness_C2->setEnabled(true);
    ui.doubleSpinBox_thickness_C3->setEnabled(true);

    ui.spinBox_C2_red->setEnabled(true);
    ui.spinBox_C2_blue->setEnabled(true);
    ui.spinBox_C2_green->setEnabled(true);

    ui.spinBox_C3_red->setEnabled(true);
    ui.spinBox_C3_blue->setEnabled(true);
    ui.spinBox_C3_green->setEnabled(true);

    // QString s=tr("background-color:
    // rgb(%1,%2,%3);").arg(Color_C2.x()).arg(Color_C2.y()).arg(Color_C2.z());
    // ui.pushButton_C2->setStyleSheet(s);

    QString tmp = PG;
    uint order = 1;

    // cout << "\n\n\nPG.toStdString()-------------------"<<endl;
    // cout << PG.toStdString()<<endl;

    if (PG.contains("C00v") || PG.contains("Coov")) {
        _isC00v = true;
        disable_C2();
        disable_C3();

        disable_SigmaD();
        disable_SigmaH();
        ui.doubleSpinBox_H_length->setEnabled(true);

        isFirstTime = true;
        ui.checkBox_PGElement->setCheckState(Qt::Checked);
        isFirstTime = false;
        return;
    }

    if (PG.contains("D00h") || PG.contains("Dooh")) {
        _isD00h = true;

        disable_C2();
        disable_C3();

        disable_SigmaD();

        isFirstTime = true;
        ui.checkBox_PGElement->setCheckState(Qt::Checked);
        isFirstTime = false;
    }

    if (PG.startsWith("Ci"))
        _isCi = true;

    if (PG.endsWith("C1")) {
        _isC1 = true;
        disable_C2();
        disable_C3();
        disable_SigmaV();
        disable_SigmaD();
        disable_SigmaH();
    }

    if (PG.startsWith("C")) {
        if (PG.endsWith("s")) {
            _isCs = true;

            disable_Cn();
            disable_C2();
            disable_C3();
            disable_SigmaD();
            disable_SigmaH();

            ui.doubleSpinBox_H_length->setEnabled(true);
            mol->perceiveMolLength();
            ui.doubleSpinBox_H_length->setValue(mol->getHeight());
        }

        else if (PG.endsWith("v")) {
            _isCnv = true;
            ui.pushButton_D->setText(QString(sigmaV));

            disable_C2();
            disable_C3();
            disable_SigmaH();

            ui.doubleSpinBox_H_length->setEnabled(true);

            tmp.remove(0, 1);
            tmp.chop(1);
            order = tmp.toInt();
            if (order % 2 == 1) {
                ui.pushButton_D->setText(QString(sigmaD));
                disable_SigmaD();
            } else {
                ui.pushButton_D->setText(QString(sigmaV));
                ui.pushButton_D->setEnabled(true);
                ui.spinBox_D_type->setEnabled(true);
                ui.doubleSpinBox_D_length->setEnabled(true);
                ui.doubleSpinBox_D_thickness->setEnabled(true);
                ui.spinBox_D_red->setEnabled(true);
                ui.spinBox_D_blue->setEnabled(true);
                ui.spinBox_D_green->setEnabled(true);
                ui.spinBox_D_opacity->setEnabled(true);
            }
        }

        else if (PG.endsWith("h")) {
            _isCnh = true;
            ui.pushButton_D->setText(QString(sigmaV));

            disable_C2();
            disable_C3();

            disable_SigmaV();
            disable_SigmaD();

            ui.doubleSpinBox_H_length->blockSignals(true);
            ui.doubleSpinBox_H_length->setValue(view3d->getHorizontalLengh());
            ui.doubleSpinBox_H_length->blockSignals(false);
        }

        else {
            _isCn = true;
            disable_C2();
            disable_C3();
            disable_SigmaV();
            disable_SigmaD();
            disable_SigmaH();
        }
    }

    if (PG.startsWith("D")) {
        if (PG.endsWith("d")) {
            _isDnd = true;

            disable_SigmaH();
            tmp.remove(0, 1);
            tmp.chop(1);
            order = tmp.toInt();

            ui.doubleSpinBox_H_length->setEnabled(true);

            if (order % 2 > 0) {
                disable_C3();
                disable_SigmaV();
                ui.pushButton_V->setText(QString(sigmaV));
            } else {
                ui.pushButton_V->setText(QString(sigmaD));
                isC3toC2 = true;
                ui.pushButton_C2->setText("C2-I");
                ui.pushButton_C3->setText("C2-II");
            }
        }

        else if (PG.endsWith("h")) {
            _isDnh = true;

            tmp.remove(0, 1);
            tmp.chop(1);
            order = tmp.toInt();

            ui.pushButton_D->setText(QString(sigmaV));

            if (order % 2 > 0) {
                disable_SigmaV();
                disable_C3();
            } else {
                isC3toC2 = true;
                ui.pushButton_C2->setText("C2-I");
                ui.pushButton_C3->setText("C2-II");
            }
        }

        // Dn
        else {
            _isDn = true;
            tmp.remove(0, 1);
            order = tmp.toInt();

            if (order % 2 > 0)
                disable_C3();
            else {
                isC3toC2 = true;
                ui.pushButton_C2->setText("C2-I");
                ui.pushButton_C3->setText("C2-II");
            }

            disable_SigmaV();
            disable_SigmaD();
            disable_SigmaH();
        }
    }

    if (PG.startsWith("T")) {
        QString s = tr("background-color: rgb(%1,%2,%3);")
        .arg(ColorCn.x())
            .arg(ColorCn.y())
            .arg(ColorCn.z());
        ui.pushButton_C2->setStyleSheet(s);

        ui.spinBox_C2_red->blockSignals(true);
        ui.spinBox_C2_blue->blockSignals(true);
        ui.spinBox_C2_green->blockSignals(true);

        ui.spinBox_C2_red->setValue(Color_Cn.x());
        ui.spinBox_C2_green->setValue(Color_Cn.y());
        ui.spinBox_C2_blue->setValue(Color_Cn.z());

        ui.spinBox_C2_red->blockSignals(false);
        ui.spinBox_C2_blue->blockSignals(false);
        ui.spinBox_C2_green->blockSignals(false);

        if (PG.endsWith("d")) {
            _isTd = true;
            ui.pushButton_V->setText(QString(sigmaD));
            ui.pushButton_H->setText(QString(sigmaD));

            ui.checkBox_TypeC2_II->setCheckState(Qt::Unchecked);
        }

        else if (PG.endsWith("h")) {
            _isTh = true;
            ui.pushButton_V->setText(QString(sigmaH));
            ui.pushButton_H->setText(QString(sigmaH));
            ui.pushButton_D->setText(QString(sigmaH));
        }

        else {
            _isT = true;
            disable_SigmaV();
            disable_SigmaD();
            disable_SigmaH();
        }
    }

    if (PG.startsWith("O")) {
        if (!PG.endsWith("h")) {
            _isOh = true;
            disable_SigmaV();
            disable_SigmaD();
            disable_SigmaH();
        } else {
            _isO = true;
            ui.pushButton_D->setText(QString(sigmaV));
        }
    }

    if (PG.startsWith("I")) {
        if (!PG.endsWith("h")) {
            _isIh = true;

            disable_SigmaV();
            disable_SigmaD();
            disable_SigmaH();
        } else {
            ui.pushButton_V->setText(QString(sigmaD));
            ui.pushButton_H->setText(QString(sigmaD));
        }
    }

    if (PG.startsWith("S")) {
        _isSn = true;

        disable_C2();
        disable_C3();
        disable_SigmaD();
        disable_SigmaH();
        disable_SigmaV();
    }

    isFirstTime = true;
    ui.checkBox_PGElement->setCheckState(Qt::Checked);
    isFirstTime = false;

    // ui.checkBox_Circle->blockSignals(false);
    // ui.checkBox_Circle->blockSignals(true);
}

void MainWindow::on_checkBox_xyzAxis_stateChanged(int arg1) {
    view3d->setXYZAxisVisible(arg1);
    view3d->renderMol(mol);
}

void MainWindow::on_checkBox_HideMol_stateChanged(int arg1) {
    if (arg1 > 0)
        view3d->setMolVisible(false);
    else
        view3d->setMolVisible(true);

    if (isFirstTime)
        return;
    view3d->refresh();
}

void MainWindow::on_checkBox_HideHydrogen_stateChanged(int arg1) {
    if (arg1 > 0)
        view3d->setHydrogenVisible(false);
    else
        view3d->setHydrogenVisible(true);

    if (isFirstTime)
        return;
    view3d->refresh();
}

void MainWindow::on_pushButton_tmp_Draw_clicked() {
    view3d->setPencilDrawMode();
    view3d->removeDynamicLineActor();
    view3d->releaseAllActors();
}

void MainWindow::on_pushButton_tmp_Metal_clicked() {
    auto form = new AtomTemplate(this);
    if (form != nullptr) {
        form->setParent(this);
        form->setAttribute(Qt::WA_DeleteOnClose, true);
        form->exec();
    }
    loadTemplateMol();
}

void MainWindow::on_pushButton_tmp_R_clicked() {
    auto form = new RTemplate(this);
    form->setParent(this);

    if (form != nullptr) {
        form->setAttribute(Qt::WA_DeleteOnClose, true);
        form->setParent(this);
        form->exec();
    }

    loadTemplateMol();
}

void MainWindow::on_pushButton_tmp_Ring_clicked() {
    auto form = new RingTemplate(this);
    form->setParent(this);

    if (form != nullptr) {
        form->setAttribute(Qt::WA_DeleteOnClose, true);
        form->setParent(this);
        form->exec();
    }

    loadTemplateMol();
}

void MainWindow::on_checkBox_outline_stateChanged(int arg1) {
    view3d->setOutLineType(arg1);
    view3d->renderMol();
    if (isFirstTime)
        return;
}

void MainWindow::on_checkBox_atomLabel_stateChanged(int arg1) {
    if (isFirstTime)
        return;
    if (ui.checkBox_atomLabel->isChecked())
        view3d->setAtomLabelVisible(true);
    else
        view3d->setAtomLabelVisible(false);

    view3d->renderMol();
}

void MainWindow::on_doubleSpinBox_diskSize_Cn_valueChanged(double arg1) {
    view3d->setDiscSize_Cn(arg1);

    if (view3d->getSymmetry() == "T" || view3d->getSymmetry() == "Td" ||
        view3d->getSymmetry() == "Th") {
        disconnect(ui.doubleSpinBox_diskSize_C2, SIGNAL(valueChanged(double)), this,
                   SLOT(on_doubleSpinBox_diskSize_C2_valueChanged(double)));

        ui.doubleSpinBox_diskSize_C2->setValue(arg1);
        view3d->setDiscSize_C2(arg1);

        connect(ui.doubleSpinBox_diskSize_C2, SIGNAL(valueChanged(double)), this,
                SLOT(on_doubleSpinBox_diskSize_C2_valueChanged(double)));
    }

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_doubleSpinBox_diskSize_C2_valueChanged(double arg1) {
    view3d->setDiscSize_C2(arg1);

    if (isC3toC2) {
        disconnect(ui.doubleSpinBox_diskSize_C3, SIGNAL(valueChanged(double)), this,
                   SLOT(on_doubleSpinBox_diskSize_C3_valueChanged(double)));

        ui.doubleSpinBox_diskSize_C3->setValue(arg1);

        connect(ui.doubleSpinBox_diskSize_C3, SIGNAL(valueChanged(double)), this,
                SLOT(on_doubleSpinBox_diskSize_C3_valueChanged(double)));
    }

    if (view3d->getSymmetry() == "T" || view3d->getSymmetry() == "Td" ||
        view3d->getSymmetry() == "Th") {
        disconnect(ui.doubleSpinBox_diskSize_Cn, SIGNAL(valueChanged(double)), this,
                   SLOT(on_doubleSpinBox_diskSize_Cn_valueChanged(double)));

        ui.doubleSpinBox_diskSize_Cn->setValue(arg1);
        view3d->setDiscSize_Cn(arg1);

        connect(ui.doubleSpinBox_diskSize_Cn, SIGNAL(valueChanged(double)), this,
                SLOT(on_doubleSpinBox_diskSize_Cn_valueChanged(double)));
    }

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_doubleSpinBox_diskSize_C3_valueChanged(double arg1) {
    view3d->setDiscSize_C3(arg1);

    if (isC3toC2) {
        disconnect(ui.doubleSpinBox_diskSize_C2, SIGNAL(valueChanged(double)), this,
                   SLOT(on_doubleSpinBox_diskSize_C2_valueChanged(double)));
        ui.doubleSpinBox_diskSize_C2->setValue(arg1);

        view3d->setDiscSize_C2(arg1);

        connect(ui.doubleSpinBox_diskSize_C2, SIGNAL(valueChanged(double)), this,
                SLOT(on_doubleSpinBox_diskSize_C2_valueChanged(double)));
    }

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_doubleSpinBox_radius_Cn_valueChanged(double arg1) {
    view3d->setRadiusCn_Scale(arg1);

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_doubleSpinBox_radius_C2_valueChanged(double arg1) {
    view3d->setRadiusC2_Scale(arg1);

    if (isC3toC2) {
        disconnect(ui.doubleSpinBox_radius_C3, SIGNAL(valueChanged(double)), this,
                   SLOT(on_doubleSpinBox_radius_C3_valueChanged(double)));
        ui.doubleSpinBox_radius_C3->setValue(arg1);

        connect(ui.doubleSpinBox_radius_C3, SIGNAL(valueChanged(double)), this,
                SLOT(on_doubleSpinBox_radius_C3_valueChanged(double)));
    }

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_doubleSpinBox_radius_C3_valueChanged(double arg1) {
    view3d->setRadiusC3_Scale(arg1);

    if (view3d->getSymmetry().startsWith("D")) {
        QString o = view3d->getSymmetry().at(1);
        unsigned int ord = o.toInt();
        if (ord % 2 == 0)
            isC3toC2 = true;
        else
            isC3toC2 = false;
    }

    if (isC3toC2) {
        disconnect(ui.doubleSpinBox_radius_C2, SIGNAL(valueChanged(double)), this,
                   SLOT(on_doubleSpinBox_radius_C2_valueChanged));
        ui.doubleSpinBox_radius_C2->setValue(arg1);

        connect(ui.doubleSpinBox_radius_C2, SIGNAL(valueChanged(double)), this,
                SLOT(on_doubleSpinBox_radius_C2_valueChanged));
    }

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_doubleSpinBox_thickness_Cn_valueChanged(double arg1) {
    disconnect(ui.doubleSpinBox_thickness_C2, SIGNAL(valueChanged(double)), this,
               SLOT(on_doubleSpinBox_thickness_C2_valueChanged(double)));

    disconnect(ui.doubleSpinBox_thickness_C3, SIGNAL(valueChanged(double)), this,
               SLOT(on_doubleSpinBox_thickness_C3_valueChanged(double)));

    ui.doubleSpinBox_thickness_C2->setValue(arg1);
    ui.doubleSpinBox_thickness_C3->setValue(arg1);

    connect(ui.doubleSpinBox_thickness_C2, SIGNAL(valueChanged(double)), this,
            SLOT(on_doubleSpinBox_thickness_C2_valueChanged(double)));

    connect(ui.doubleSpinBox_thickness_C3, SIGNAL(valueChanged(double)), this,
            SLOT(on_doubleSpinBox_thickness_C3_valueChanged(double)));

    view3d->setDiscThickness(arg1);

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_doubleSpinBox_thickness_C2_valueChanged(double arg1) {
    disconnect(ui.doubleSpinBox_thickness_Cn, SIGNAL(valueChanged(double)), this,
               SLOT(on_doubleSpinBox_thickness_C2_valueChanged(double)));

    disconnect(ui.doubleSpinBox_thickness_C3, SIGNAL(valueChanged(double)), this,
               SLOT(on_doubleSpinBox_thickness_C3_valueChanged(double)));

    ui.doubleSpinBox_thickness_Cn->setValue(arg1);
    ui.doubleSpinBox_thickness_C3->setValue(arg1);

    connect(ui.doubleSpinBox_thickness_Cn, SIGNAL(valueChanged(double)), this,
            SLOT(on_doubleSpinBox_thickness_C2_valueChanged(double)));

    connect(ui.doubleSpinBox_thickness_C3, SIGNAL(valueChanged(double)), this,
            SLOT(on_doubleSpinBox_thickness_C3_valueChanged(double)));

    view3d->setDiscThickness(arg1);

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_doubleSpinBox_thickness_C3_valueChanged(double arg1) {
    disconnect(ui.doubleSpinBox_thickness_Cn, SIGNAL(valueChanged(double)), this,
               SLOT(on_doubleSpinBox_thickness_C2_valueChanged(double)));

    disconnect(ui.doubleSpinBox_thickness_C2, SIGNAL(valueChanged(double)), this,
               SLOT(on_doubleSpinBox_thickness_C2_valueChanged(double)));

    ui.doubleSpinBox_thickness_Cn->setValue(arg1);
    ui.doubleSpinBox_thickness_C2->setValue(arg1);

    connect(ui.doubleSpinBox_thickness_Cn, SIGNAL(valueChanged(double)), this,
            SLOT(on_doubleSpinBox_thickness_C2_valueChanged(double)));

    connect(ui.doubleSpinBox_thickness_C2, SIGNAL(valueChanged(double)), this,
            SLOT(on_doubleSpinBox_thickness_C2_valueChanged(double)));

    view3d->setDiscThickness(arg1);

    if (isFirstTime)
        return;

    view3d->renderMol();
}

// read from View3d
void MainWindow::updatePanelParameters() {
    isFirstTime = true;

    needFreshView = false;
    isC3toC2 = false;

    ui.doubleSpinBox_V_length->setValue(view3d->getHorizontalLengh());
    ui.doubleSpinBox_D_length->setValue(view3d->getHorizontalLengh());
    ui.doubleSpinBox_H_length->setValue(view3d->getVerticalLength());

    ui.doubleSpinBox_thickness_Cn->setValue(view3d->getDiscThickness());
    ui.doubleSpinBox_thickness_C2->setValue(view3d->getDiscThickness());
    ui.doubleSpinBox_thickness_C3->setValue(view3d->getDiscThickness());

    ui.doubleSpinBox_diskSize_Cn->setValue(view3d->getDiscSize_Cn());
    ui.doubleSpinBox_diskSize_C2->setValue(view3d->getDiscSize_C2());
    ui.doubleSpinBox_diskSize_C3->setValue(view3d->getDiscSize_C3());

    needFreshView = false;

    QString s;

    // color
    Color_bk = 255.0 * view3d->getBackgroundColor();
    Color_V = 255.0 * view3d->getSigmaVColor();
    Color_D = 255.0 * view3d->getSigmaDColor();
    Color_H = 255.0 * view3d->getSigmaHColor();
    Color_Cn = 255.0 * view3d->getColorCn();

    ui.spinBox_V_red->setValue(Color_V.x());
    ui.spinBox_V_green->setValue(Color_V.y());
    ui.spinBox_V_blue->setValue(Color_V.z());

    ui.spinBox_D_red->setValue(Color_D.x());
    ui.spinBox_D_green->setValue(Color_D.y());
    ui.spinBox_D_blue->setValue(Color_D.z());

    ui.spinBox_H_red->setValue(Color_H.x());
    ui.spinBox_H_green->setValue(Color_H.y());
    ui.spinBox_H_blue->setValue(Color_H.z());

    // ui.spinBox_bk_red->setValue(Color_bk.x());
    // ui.spinBox_bk_green->setValue(Color_bk.y());
    // ui.spinBox_bk_blue->setValue(Color_bk.z());
    // ui.spinBox_bk_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");

    ui.spinBox_Cn_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");
    ui.spinBox_C2_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");
    ui.spinBox_C3_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");

    ui.spinBox_D_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");
    ui.spinBox_H_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");
    ui.spinBox_V_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");

    ui.spinBox_Cn_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");
    ui.spinBox_C2_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");
    ui.spinBox_C3_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");

    ui.spinBox_D_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");
    ui.spinBox_H_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");
    ui.spinBox_V_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");
    // ui.spinBox_bk_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");

    ui.spinBox_Cn_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");
    ui.spinBox_C2_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");
    ui.spinBox_C3_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");

    ui.spinBox_D_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");
    ui.spinBox_H_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");
    ui.spinBox_V_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");
    // ui.spinBox_bk_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_V.x())
            .arg(Color_V.y())
            .arg(Color_V.z());
    ui.pushButton_V->setStyleSheet(s);

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_D.x())
            .arg(Color_D.y())
            .arg(Color_D.z());
    ui.pushButton_D->setStyleSheet(s);

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_H.x())
            .arg(Color_H.y())
            .arg(Color_H.z());
    ui.pushButton_H->setStyleSheet(s);

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_bk.x())
            .arg(Color_bk.y())
            .arg(Color_bk.z());
    // ui.pushButton_bk->setStyleSheet(s);

    ui.spinBox_V_red->setValue(Color_V.x());
    ui.spinBox_V_green->setValue(Color_V.y());
    ui.spinBox_V_blue->setValue(Color_V.z());

    Opacity_V = 255 * view3d->getOpacityV();
    Opacity_D = 255 * view3d->getOpacityD();
    Opacity_H = 255 * view3d->getOpacityH();

    // backup
    OpacityV = Opacity_V;
    OpacityD = Opacity_D;
    OpacityH = Opacity_H;
    ColorV = Color_V;
    ColorD = Color_D;
    ColorH = Color_H;
    Colorbk = Color_bk;
    ColorCn = Color_Cn;

    ui.spinBox_V_opacity->setValue(Opacity_V);
    ui.spinBox_D_opacity->setValue(Opacity_D);
    ui.spinBox_H_opacity->setValue(Opacity_H);

    scaleThicknessSigma_V = view3d->getThicknessSigmaV_Scale();
    scaleThicknessSigma_D = view3d->getThicknessSigmaD_Scale();
    scaleThicknessSigma_H = view3d->getThicknessSigmaH_Scale();

    scaleThicknessSigmaV = scaleThicknessSigma_V;
    scaleThicknessSigmaD = scaleThicknessSigma_D;
    scaleThicknessSigmaH = scaleThicknessSigma_H;

    ui.doubleSpinBox_V_thickness->setValue(scaleThicknessSigma_V);
    ui.doubleSpinBox_D_thickness->setValue(scaleThicknessSigma_D);
    ui.doubleSpinBox_H_thickness->setValue(scaleThicknessSigma_H);

    ui.doubleSpinBox_radius_Cn->setValue(view3d->getRadiusCn_Scale());
    ui.doubleSpinBox_radius_C2->setValue(view3d->getRadiusC2_Scale());
    ui.doubleSpinBox_radius_C3->setValue(view3d->getRadiusC3_Scale());

    ui.doubleSpinBox_Cn_length->setValue(view3d->getCn_Length());
    ui.doubleSpinBox_C2_length->setValue(view3d->getC2_Length());
    ui.doubleSpinBox_C3_length->setValue(view3d->getC3_Length());

    Color_C2 = 255.0 * view3d->getColorC2_1();
    Color_C3 = 255.0 * view3d->getColorC3();

    ui.spinBox_Cn_red->setValue(Color_Cn.x());
    ui.spinBox_Cn_green->setValue(Color_Cn.y());
    ui.spinBox_Cn_blue->setValue(Color_Cn.z());

    ui.spinBox_C2_red->setValue(Color_C2.x());
    ui.spinBox_C2_green->setValue(Color_C2.y());
    ui.spinBox_C2_blue->setValue(Color_C2.z());

    ui.spinBox_C3_red->setValue(Color_C3.x());
    ui.spinBox_C3_green->setValue(Color_C3.y());
    ui.spinBox_C3_blue->setValue(Color_C3.z());

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_Cn.x())
            .arg(Color_Cn.y())
            .arg(Color_Cn.z());
    ui.pushButton_Cn->setStyleSheet(s);

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_C2.x())
            .arg(Color_C2.y())
            .arg(Color_C2.z());
    ui.pushButton_C2->setStyleSheet(s);

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_C3.x())
            .arg(Color_C3.y())
            .arg(Color_C3.z());
    ui.pushButton_C3->setStyleSheet(s);

    ui.dialAtomScale->setValue(view3d->getAtomScale() * 200);
    ui.dialBondScale->setValue(view3d->getAtomScale() * 200);

    // ui.dialRingScale_1->setValue(view3d->getRingSizeScale()*100);
    //  ui.dialRingScale_2->setValue(view3d->getRingSizeScale()*50);
    isFirstTime = false;
}

void MainWindow::getDataFromPanel() {
    Color_V.SetX(ui.spinBox_V_red->value());
    Color_V.SetY(ui.spinBox_V_green->value());
    Color_V.SetZ(ui.spinBox_V_blue->value());

    Color_D.SetX(ui.spinBox_D_red->value());
    Color_D.SetY(ui.spinBox_D_green->value());
    Color_D.SetZ(ui.spinBox_D_blue->value());

    Color_H.SetX(ui.spinBox_H_red->value());
    Color_H.SetY(ui.spinBox_H_green->value());
    Color_H.SetZ(ui.spinBox_H_blue->value());

    Opacity_V = ui.spinBox_V_opacity->value();
    Opacity_D = ui.spinBox_D_opacity->value();
    // Opacity_H=ui.spinBox_H_opacity->value();

    scaleThicknessSigma_V = ui.doubleSpinBox_V_thickness->value();
    scaleThicknessSigma_D = ui.doubleSpinBox_D_thickness->value();
    scaleThicknessSigma_H = ui.doubleSpinBox_H_thickness->value();
    scaleThickness_Cn = ui.doubleSpinBox_thickness_Cn->value();

    // Color_bk.Set(ui.spinBox_bk_red->value(),
    //              ui.spinBox_bk_green->value(),
    //              ui.spinBox_bk_blue->value());
}


void MainWindow::on_doubleSpinBox_V_length_valueChanged(double arg1) {
    if (isFirstTime)
        return;

    view3d->setHorizontalLengh(arg1);



    ui.doubleSpinBox_D_length->blockSignals(true);
    ui.doubleSpinBox_D_length->setValue(arg1);
    ui.doubleSpinBox_D_length->blockSignals(false);



    //modify accordingly
    if(ui.doubleSpinBox_C3_length->isEnabled())
    {
        auto value=ui.doubleSpinBox_C3_length->value();

        if( value_V_length>arg1)
            ui.doubleSpinBox_C3_length->setValue(value-0.1);
        else
            ui.doubleSpinBox_C3_length->setValue(value+0.1);
    }

    if (_isTd || _isTh) {
        ui.doubleSpinBox_H_length->blockSignals(true);
        ui.doubleSpinBox_H_length->setValue(arg1);
        ui.doubleSpinBox_H_length->blockSignals(false);
    }

    value_V_length=arg1;

    view3d->renderMol();
}

void MainWindow::on_doubleSpinBox_D_length_valueChanged(double arg1) {
    if (isFirstTime)
        return;

    view3d->setHorizontalLengh(arg1);

    if(ui.doubleSpinBox_V_length->isEnabled())
    {
        ui.doubleSpinBox_V_length->blockSignals(true);
        ui.doubleSpinBox_V_length->setValue(arg1);
        ui.doubleSpinBox_V_length->blockSignals(false);
    }

    if(ui.doubleSpinBox_C2_length->isEnabled())
    {
        auto value=ui.doubleSpinBox_C2_length->value();

        if( value_D_length>arg1)
            ui.doubleSpinBox_C2_length->setValue(value-0.1);
        else
            ui.doubleSpinBox_C2_length->setValue(value+0.1);

    }

    if (_isTd || _isTh) {
        ui.doubleSpinBox_H_length->blockSignals(true);
        ui.doubleSpinBox_H_length->setValue(arg1);
        ui.doubleSpinBox_H_length->blockSignals(false);
    }

    value_D_length=arg1;
    view3d->renderMol();
}


void MainWindow::on_doubleSpinBox_H_length_valueChanged(double arg1)
{
    if (isFirstTime)
        return;


    if(ui.doubleSpinBox_Cn_length->isEnabled())
    {
        auto value=ui.doubleSpinBox_Cn_length->value();

        if( value_H_length>arg1)
            ui.doubleSpinBox_Cn_length->setValue(value-0.1);
        else
            ui.doubleSpinBox_Cn_length->setValue(value+0.1);
    }


    if (_isTd) {
        ui.doubleSpinBox_V_length->blockSignals(true);
        ui.doubleSpinBox_V_length->setValue(arg1);
        ui.doubleSpinBox_V_length->blockSignals(false);

        ui.doubleSpinBox_D_length->blockSignals(true);
        ui.doubleSpinBox_D_length->setValue(arg1);
        ui.doubleSpinBox_D_length->blockSignals(false);
    }

    else if (_isTh) {
        view3d->setHorizontalLengh(arg1);

        ui.doubleSpinBox_V_length->blockSignals(true);
        ui.doubleSpinBox_V_length->setValue(arg1);
        ui.doubleSpinBox_V_length->blockSignals(false);

        ui.doubleSpinBox_D_length->blockSignals(true);
        ui.doubleSpinBox_D_length->setValue(arg1);
        ui.doubleSpinBox_D_length->blockSignals(false);
    }

    else if (_isCnh) {
        view3d->setHorizontalLengh(arg1);
    }

    else  if (_isDnh){
        view3d->setVerticalLength(arg1);
    }

    value_H_length=arg1;
    view3d->renderMol();
}


void MainWindow::on_doubleSpinBox_V_thickness_valueChanged(double arg1) {
    view3d->setThicknessSigmaV_Scale(arg1);

    ui.doubleSpinBox_D_thickness->blockSignals(true);
    ui.doubleSpinBox_D_thickness->setValue(arg1);
    ui.doubleSpinBox_D_thickness->blockSignals(false);

    if (_isTh || _isOh) {
        ui.doubleSpinBox_H_thickness->blockSignals(true);
        ui.doubleSpinBox_H_thickness->setValue(arg1);
        ui.doubleSpinBox_H_thickness->blockSignals(false);
    }

    if (isFirstTime)
        return;
    view3d->renderMol();
}


void MainWindow::on_doubleSpinBox_D_thickness_valueChanged(double arg1) {
    view3d->setThicknessSigmaD_Scale(arg1);

    ui.doubleSpinBox_V_thickness->setValue(arg1);
    if (_isTh || _isOh) {
        ui.doubleSpinBox_H_thickness->blockSignals(true);
        ui.doubleSpinBox_H_thickness->setValue(arg1);
        ui.doubleSpinBox_H_thickness->blockSignals(false);
    }

    if (isFirstTime)
        return;

    view3d->renderMol();
}

void MainWindow::on_doubleSpinBox_H_thickness_valueChanged(double arg1) {
    view3d->setThicknessSigmaH_Scale(arg1);

    if (_isTh || _isOh) {
        ui.doubleSpinBox_V_thickness->blockSignals(true);
        ui.doubleSpinBox_V_thickness->setValue(arg1);
        ui.doubleSpinBox_V_thickness->blockSignals(false);

        ui.doubleSpinBox_D_thickness->blockSignals(true);
        ui.doubleSpinBox_D_thickness->setValue(arg1);
        ui.doubleSpinBox_D_thickness->blockSignals(false);
    }

    if (isFirstTime)
        return;

    view3d->renderMol();
}

void MainWindow::on_checkBox_PGElement_stateChanged(int arg1) {
    if (arg1 > 0) {
        view3d->setSymmetryElementVisible(true);

        if (ui.checkBox_CnHide->isEnabled())
            ui.checkBox_CnHide->setCheckState(Qt::Unchecked);

        if (ui.checkBox_C2Hide->isEnabled())
            ui.checkBox_C2Hide->setCheckState(Qt::Unchecked);

        if (ui.checkBox_C3Hide->isEnabled())
            ui.checkBox_C3Hide->setCheckState(Qt::Unchecked);

        if (ui.checkBox_SigmaV_Hide->isEnabled())
            ui.checkBox_SigmaV_Hide->setCheckState(Qt::Unchecked);

        if (ui.checkBox_SigmaD_Hide->isEnabled())
            ui.checkBox_SigmaD_Hide->setCheckState(Qt::Unchecked);

        if (ui.checkBox_SigmaH_Hide->isEnabled())
            ui.checkBox_SigmaH_Hide->setCheckState(Qt::Unchecked);

        ui.treeWidget->setAllChecked();
    } else {
        view3d->setSymmetryElementVisible(false);

        if (ui.checkBox_CnHide->isEnabled())
            ui.checkBox_CnHide->setCheckState(Qt::Checked);
        if (ui.checkBox_C2Hide->isEnabled())
            ui.checkBox_C2Hide->setCheckState(Qt::Checked);

        if (ui.checkBox_C3Hide->isEnabled())
            ui.checkBox_C3Hide->setCheckState(Qt::Checked);

        if (ui.checkBox_SigmaV_Hide->isEnabled())
            ui.checkBox_SigmaV_Hide->setCheckState(Qt::Checked);

        if (ui.checkBox_SigmaD_Hide->isEnabled())
            ui.checkBox_SigmaD_Hide->setCheckState(Qt::Checked);

        if (ui.checkBox_SigmaH_Hide->isEnabled())
            ui.checkBox_SigmaH_Hide->setCheckState(Qt::Checked);

        ui.treeWidget->setAllUnchecked();
    }

    if (isFirstTime)
        return;

    view3d->refresh();
}

void MainWindow::on_checkBox_CnHide_stateChanged(int arg1) {
    if (arg1 < 1)
        setPGElement_Visible(true);

    if (arg1 > 0) {
        view3d->setCnVisible(false);
        ui.treeWidget->set_Cn_Checked(false);

        if (_isO || _isOh)
            view3d->setAllC4_Visible(false);

        if (_isI || _isIh)
            view3d->setAllC5_Visible(false);
    } else {
        if (_isO || _isOh)
            view3d->setAllC4_Visible(true);

        if (_isI || _isIh) {
            view3d->setAllC5_Visible(true);
            view3d->setC5_Visible(true);
        }

        ui.treeWidget->set_Cn_Checked(true);

        view3d->setSymmetryElementVisible(true);
        view3d->setCnVisible(true);

        if (!ui.checkBox_PGElement->isChecked()) {
            ui.checkBox_PGElement->blockSignals(true);
            ui.checkBox_PGElement->setCheckState(Qt::Checked);
            ui.checkBox_PGElement->blockSignals(false);
        }
    }

    if (isFirstTime)
        return;
    view3d->refresh();
}

void MainWindow::modifyCheckBox_CnHide(bool t, bool blockSignal) {
    if (blockSignal)
        ui.checkBox_CnHide->blockSignals(true);

    if (t)
        ui.checkBox_CnHide->setCheckState(Qt::Checked);
    else
        ui.checkBox_CnHide->setCheckState(Qt::Unchecked);

    if (blockSignal)
        ui.checkBox_CnHide->blockSignals(false);
}

void MainWindow::on_checkBox_C2Hide_stateChanged(int arg1) {
    if (arg1 < 1)
        view3d->setSymmetryElementVisible(true);

    if (ui.treeWidget->isTd() || ui.treeWidget->isTh() || ui.treeWidget->isT()) {
        if (arg1 > 0) {
            view3d->setAllS4_Visible(false);
        } else {
            view3d->setAllS4_Visible(true);
        }

        if (isFirstTime)
            return;
        view3d->refresh();
    }

    if (arg1 > 0) {
        view3d->setC2_1_Visible(false);
        if (ui.checkBox_C3Hide->isEnabled() && isC3toC2)
            ui.treeWidget->set_C2_Checked(1, false);
        else
            ui.treeWidget->set_C2_Checked(0, false);
    } else {
        if (ui.checkBox_C3Hide->isEnabled() && isC3toC2)
            ui.treeWidget->set_C2_Checked(1);
        else
            ui.treeWidget->set_C2_Checked(0);

        view3d->setC2_1_Visible(true);

        if (!ui.checkBox_PGElement->isChecked()) {
            ui.checkBox_PGElement->blockSignals(true);
            ui.checkBox_PGElement->setCheckState(Qt::Checked);
            ui.checkBox_PGElement->blockSignals(false);
        }
    }

    if (isFirstTime)
        return;

    view3d->refresh();
}

void MainWindow::on_checkBox_C3Hide_stateChanged(int arg1) {
    if (arg1 < 1)
        setPGElement_Visible(true);

    if (arg1 > 0) {
        if (isC3toC2) {
            view3d->setC2_2_Visible(false);

            if (ui.checkBox_C3Hide->isEnabled())
                ui.treeWidget->set_C2_Checked(2, false);
            else
                ui.treeWidget->set_C2_Checked(0, false);
        } else {
            view3d->setC3_Visible(false);
            ui.treeWidget->set_C3_Checked(0);
        }

    } else {
        view3d->setSymmetryElementVisible(true);

        if (isC3toC2) {
            view3d->setC2_2_Visible(true);
            if (ui.checkBox_C3Hide->isEnabled())
                ui.treeWidget->set_C2_Checked(2);
            else
                ui.treeWidget->set_C2_Checked(0);
        } else {
            view3d->setC3_Visible(true);
            ui.treeWidget->set_C3_Checked(1);
        }

        if (!ui.checkBox_PGElement->isChecked()) {
            ui.checkBox_PGElement->blockSignals(true);
            ui.checkBox_PGElement->setCheckState(Qt::Checked);
            ui.checkBox_PGElement->blockSignals(false);
        }
    }

    if (isFirstTime)
        return;
    view3d->refresh();
}

void MainWindow::checkBox_SigmaV_Hide_stateChanged_Td(int arg1) {
    if (arg1 > 0) {

    } else {
    }
}

void MainWindow::checkBox_SigmaV_Hide_stateChanged_Th(int arg1) {}

void MainWindow::checkBox_SigmaV_Hide_stateChanged_Oh(int arg1) {
    if (arg1 > 0) {

    } else {
    }
}

void MainWindow::checkBox_SigmaV_Hide_stateChanged_Ih(int arg1) {
    if (arg1 > 0) {

    } else {
    }
}

void MainWindow::on_checkBox_SigmaV_Hide_stateChanged(int arg1) {
    if (arg1 < 1)
        setPGElement_Visible(true);

    if (ui.treeWidget->isIh()) {
        if (arg1 > 0) {
            view3d->setSigmaVVisible(false);
            for (uint i = 0; i < 5; i++) {
                view3d->setSigmaV_Visible(i, false);
                ui.treeWidget->set_SigmaD1_Checked_index(i, false);
            }

        } else {
            view3d->setSymmetryElementVisible(true);
            view3d->setSigmaVVisible(true);

            for (uint i = 0; i < 5; i++) {
                view3d->setSigmaV_Visible(i, true);
                ui.treeWidget->set_SigmaD1_Checked_index(i, true);
            }
        }
        if (isFirstTime)
            return;
        view3d->refresh();
        return;
    }

    if (ui.treeWidget->isOh()) {
        if (arg1 > 0) {
            view3d->setSigmaVVisible(false);

            ui.treeWidget->set_SigmaD_Checked_index(0, false);
            ui.treeWidget->set_SigmaD_Checked_index(1, false);

            view3d->setSigmaD_Visible(0, false);
            view3d->setSigmaD_Visible(1, false);

            // view3d->setAllSigmaD_Visible(1,false);

            // view3d->setAllSigmaV_Visible(false);
        } else {
            view3d->setSymmetryElementVisible(true);
            view3d->setSigmaVVisible(true);

            ui.treeWidget->set_SigmaD_Checked_index(0, true);
            ui.treeWidget->set_SigmaD_Checked_index(1, true);

            view3d->setSigmaD_Visible(0, true);
            view3d->setSigmaD_Visible(1, true);

            // view3d->setAllSigmaV_Visible(true);
        }

        if (isFirstTime)
            return;
        view3d->refresh();
        return;
    }

    // Th ------------------------------
    if (ui.treeWidget->isTh()) {
        if (arg1 > 0) {
            view3d->setSigmaVVisible(false);

            ui.treeWidget->set_SigmaH_Checked_index(2, false);
        } else {
            view3d->setSymmetryElementVisible(true);
            view3d->setSigmaVVisible(true);

            ui.treeWidget->set_SigmaH_Checked_index(2, true);
        }

        if (isFirstTime)
            return;
        view3d->refresh();
        return;
    }

    // Td ------------------------------
    if (ui.treeWidget->isTd()) {
        if (arg1 > 0) {
            view3d->setSigmaVVisible(false);

            ui.treeWidget->set_SigmaD_Checked_index(0, false);
            ui.treeWidget->set_SigmaD_Checked_index(1, false);

            view3d->setAllSigmaV_Visible(false);

        } else {
            view3d->setSymmetryElementVisible(true);

            view3d->setSigmaVVisible(true);
            ui.treeWidget->set_SigmaD_Checked_index(0, true);
            ui.treeWidget->set_SigmaD_Checked_index(1, true);

            view3d->setAllSigmaV_Visible(true);
        }

        if (isFirstTime)
            return;
        view3d->refresh();
        return;
    }

    if (ui.treeWidget->isOh()) {
        checkBox_SigmaV_Hide_stateChanged_Oh(arg1);
        return;
    }

    if (ui.treeWidget->isIh()) {
        checkBox_SigmaV_Hide_stateChanged_Ih(arg1);
        return;
    }

    //----------------------------------
    if (arg1 > 0) {
        if (ui.pushButton_V->text().endsWith("v")) {
            view3d->setAllSigmaV_Visible(false);

            if (ui.pushButton_D->text().endsWith("v"))
                ui.treeWidget->set_SigmaV_Checked(1, false);
            else
                ui.treeWidget->set_SigmaV_Checked(0, false);
        }

        if (ui.pushButton_V->text().endsWith("d")) {
            ui.treeWidget->set_SigmaD_Checked(1, false);
            view3d->setAllSigmaV_Visible(false);
        }

    } else {
        view3d->setSymmetryElementVisible(true);

        if (ui.pushButton_V->text().endsWith("v")) {
            view3d->setAllSigmaV_Visible(true);

            if (ui.pushButton_D->text().endsWith("v")) {
                ui.treeWidget->set_SigmaV_Checked(1, true);
            } else {
                ui.treeWidget->set_SigmaV_Checked(0, true);
            }
        }

        if (ui.pushButton_V->text().endsWith("d")) {
            view3d->setAllSigmaV_Visible(true);
            ui.treeWidget->set_SigmaD_Checked(1, true);
        }

        if (!ui.checkBox_PGElement->isChecked()) {
            ui.checkBox_PGElement->blockSignals(true);
            ui.checkBox_PGElement->setCheckState(Qt::Checked);
            ui.checkBox_PGElement->blockSignals(false);
        }
    }

    if (isFirstTime)
        return;

    view3d->refresh();
}

void MainWindow::on_checkBox_SigmaD_Hide_stateChanged(int arg1) {
    if (arg1 < 1)
        setPGElement_Visible(true);

    if (ui.treeWidget->isIh()) {
        if (arg1 > 0) {
            view3d->setSigmaDVisible(false);
            for (uint i = 0; i < 3; i++) {
                view3d->setSigmaD_Visible(i, false);
                ui.treeWidget->set_SigmaD2_Checked_index(i, false);
            }
        } else {
            view3d->setSymmetryElementVisible(true);
            view3d->setSigmaDVisible(true);

            for (uint i = 0; i < 3; i++) {
                view3d->setSigmaD_Visible(i, true);
                ui.treeWidget->set_SigmaD2_Checked_index(i, true);
            }
        }
        if (isFirstTime)
            return;
        view3d->refresh();
        return;
    }

    if (ui.treeWidget->isOh()) {
        if (arg1 > 0) {
            view3d->setSigmaDVisible(false);

            ui.treeWidget->set_SigmaD_Checked_index(2, false);
            ui.treeWidget->set_SigmaD_Checked_index(3, false);
            ui.treeWidget->set_SigmaD_Checked_index(4, false);
            ui.treeWidget->set_SigmaD_Checked_index(5, false);

            // view3d->setAllSigmaD_Visible(false);

            view3d->setSigmaD_Visible(2, false);
            view3d->setSigmaD_Visible(3, false);
            view3d->setSigmaD_Visible(4, false);
            view3d->setSigmaD_Visible(5, false);
        } else {
            view3d->setSymmetryElementVisible(true);
            view3d->setSigmaDVisible(true);

            ui.treeWidget->set_SigmaD_Checked_index(2, true);
            ui.treeWidget->set_SigmaD_Checked_index(3, true);
            ui.treeWidget->set_SigmaD_Checked_index(4, true);
            ui.treeWidget->set_SigmaD_Checked_index(5, true);

            view3d->setSigmaD_Visible(2, true);
            view3d->setSigmaD_Visible(3, true);
            view3d->setSigmaD_Visible(4, true);
            view3d->setSigmaD_Visible(5, true);

            // view3d->setAllSigmaD_Visible(true);
        }

        if (isFirstTime)
            return;
        view3d->refresh();
        return;
    }

    if (ui.treeWidget->isTh()) {
        if (arg1 > 0) {
            view3d->setSigmaDVisible(false);
            ui.treeWidget->set_SigmaH_Checked_index(1, false);
        } else {
            view3d->setSymmetryElementVisible(true);
            view3d->setSigmaDVisible(true);
            ui.treeWidget->set_SigmaH_Checked_index(1, true);
        }

        if (isFirstTime)
            return;
        view3d->refresh();
        return;
    }

    // Td ------------------------------
    if (ui.treeWidget->isTd()) {
        if (arg1 > 0) {
            view3d->setSigmaDVisible(false);

            ui.treeWidget->set_SigmaD_Checked_index(2, false);
            ui.treeWidget->set_SigmaD_Checked_index(3, false);

            view3d->setAllSigmaD_Visible(false);
        } else {
            view3d->setSymmetryElementVisible(true);

            view3d->setSigmaDVisible(true);
            ui.treeWidget->set_SigmaD_Checked_index(2, true);
            ui.treeWidget->set_SigmaD_Checked_index(3, true);

            view3d->setAllSigmaD_Visible(true);
        }

        if (isFirstTime)
            return;
        view3d->refresh();
        return;
    }

    if (arg1 > 0) {
        if (!ui.pushButton_V->isEnabled())
            ui.treeWidget->set_SigmaD_Checked(0, false);

        view3d->setAllSigmaD_Visible(false);

        if (ui.pushButton_D->text().endsWith("v")) {
            if (ui.pushButton_V->isEnabled())
                ui.treeWidget->set_SigmaV_Checked(2, false);
            else
                ui.treeWidget->set_SigmaV_Checked(0, false);
        }

        if (!ui.pushButton_V->isEnabled()) {
            if (ui.pushButton_D->text().endsWith("v")) {
                ui.treeWidget->set_SigmaV_Checked(0, true);

                view3d->setAllSigmaV_Visible(false);
            }

            // Dnd n=even
            if (ui.pushButton_D->text().endsWith("d")) {
                view3d->setAllSigmaD_Visible(false);
                view3d->setAllSigmaV_Visible(false);
            }
        }

        if (ui.pushButton_V->text().endsWith("d")) {
            ui.treeWidget->set_SigmaD_Checked(2, false);
        }
    }

    else {
        view3d->setSymmetryElementVisible(true);

        view3d->setAllSigmaD_Visible(true);

        if (!ui.pushButton_V->isEnabled()) {
            if (ui.pushButton_D->text().endsWith("v"))
                ui.treeWidget->set_SigmaV_Checked(0, true);

            if (ui.pushButton_D->text().endsWith("d")) {
                ui.treeWidget->set_SigmaD_Checked(0, true);

                view3d->setAllSigmaV_Visible(true);
            }
        }

        if (ui.pushButton_D->text().endsWith("v")) {
            if (!ui.pushButton_V->isEnabled()) {
                view3d->setAllSigmaV_Visible(true);
                ui.treeWidget->set_SigmaV_Checked(0, true);
            } else {
                ui.treeWidget->set_SigmaV_Checked(2, true);
            }
        }

        if (ui.pushButton_V->text().endsWith("d")) {
            ui.treeWidget->set_SigmaD_Checked(2, true);
        }

        if (!ui.checkBox_PGElement->isChecked()) {
            ui.checkBox_PGElement->blockSignals(true);
            ui.checkBox_PGElement->setCheckState(Qt::Checked);
            ui.checkBox_PGElement->blockSignals(false);
        }
    }

    if (isFirstTime)
        return;

    view3d->refresh();
}

void MainWindow::on_checkBox_SigmaH_Hide_stateChanged(int arg1) {
    if (arg1 < 1)
        setPGElement_Visible(true);

    if (ui.treeWidget->isIh()) {
        if (arg1 > 0) {
            view3d->setSigmaHVisible(false);
            for (uint i = 0; i < 2; i++) {
                view3d->setSigmaH_Visible(i, false);
                ui.treeWidget->set_SigmaD3_Checked_index(i, false);
            }
        } else {
            view3d->setSymmetryElementVisible(true);
            view3d->setSigmaHVisible(true);

            for (uint i = 0; i < 2; i++) {
                view3d->setSigmaH_Visible(i, true);
                ui.treeWidget->set_SigmaD3_Checked_index(i, true);
            }
        }

        if (isFirstTime)
            return;
        view3d->refresh();
        return;
    }

    if (ui.treeWidget->isOh()) {
        if (arg1 > 0) {
            view3d->setSigmaHVisible(false);

            ui.treeWidget->set_SigmaH_Checked_index(0, false);
            ui.treeWidget->set_SigmaH_Checked_index(1, false);
            ui.treeWidget->set_SigmaH_Checked_index(2, false);

            view3d->setAllSigmaH_Visible(false);
        } else {
            view3d->setSymmetryElementVisible(true);
            view3d->setSigmaHVisible(true);

            ui.treeWidget->set_SigmaH_Checked_index(0, true);
            ui.treeWidget->set_SigmaH_Checked_index(1, true);
            ui.treeWidget->set_SigmaH_Checked_index(2, true);

            view3d->setAllSigmaH_Visible(true);
        }

        if (isFirstTime)
            return;
        view3d->refresh();
        return;
    }

    // Th ------------------------------
    if (ui.treeWidget->isTh()) {
        if (arg1 > 0) {
            // view3d->setSigmaV_Visible(0,false);
            view3d->setSigmaHVisible(false);

            ui.treeWidget->set_SigmaH_Checked_index(0, false);
        } else {
            view3d->setSymmetryElementVisible(true);
            // view3d->setSigmaV_Visible(0,true);
            view3d->setSigmaHVisible(true);

            ui.treeWidget->set_SigmaH_Checked_index(0, true);
        }

        if (isFirstTime)
            return;
        view3d->refresh();
        return;
    }

    // Td ------------------------------
    if (ui.treeWidget->isTd()) {
        if (arg1 > 0) {
            view3d->setSigmaHVisible(false);

            ui.treeWidget->set_SigmaD_Checked_index(4, false);
            ui.treeWidget->set_SigmaD_Checked_index(5, false);

            view3d->setAllSigmaH_Visible(false);
        } else {
            view3d->setSymmetryElementVisible(true);

            view3d->setSigmaHVisible(true);
            ui.treeWidget->set_SigmaD_Checked_index(4, true);
            ui.treeWidget->set_SigmaD_Checked_index(5, true);

            view3d->setAllSigmaH_Visible(true);
        }

        if (isFirstTime)
            return;
        view3d->refresh();
        return;
    }

    if (arg1 > 0)
        view3d->setSigmaHVisible(false);
    else {
        view3d->setSigmaHVisible(true);
        view3d->setSymmetryElementVisible(true);

        if (!ui.checkBox_PGElement->isChecked()) {
            ui.checkBox_PGElement->blockSignals(true);
            ui.checkBox_PGElement->setCheckState(Qt::Checked);

            ui.checkBox_PGElement->blockSignals(false);
        }
    }

    if (isFirstTime)
        return;

    view3d->refresh();
}

void MainWindow::on_doubleSpinBox_Cn_length_valueChanged(double arg1) {
    if (isFirstTime)
        return;

    view3d->setCn_Length(arg1);

    if (view3d->getSymmetry() == "T" || view3d->getSymmetry() == "Td" ||
        view3d->getSymmetry() == "Th") {
        ui.doubleSpinBox_C2_length->blockSignals(true);
        ui.doubleSpinBox_C2_length->setValue(arg1);
        view3d->setC2_Length(arg1);
        ui.doubleSpinBox_C2_length->blockSignals(false);
    }

    view3d->refresh();
}

void MainWindow::on_doubleSpinBox_C2_length_valueChanged(double arg1) {
    if (isFirstTime)
        return;

    view3d->setC2_Length(arg1);

    if (_isDn)
        view3d->setHorizontalLengh(arg1);

    if (view3d->getSymmetry().startsWith("D")) {
        QString o = view3d->getSymmetry().at(1);
        if (o.toInt() % 2 == 0)
            isC3toC2 = true;
        else
            isC3toC2 = false;
    }

    if (isC3toC2) {
        ui.doubleSpinBox_C3_length->blockSignals(true);

        ui.doubleSpinBox_C3_length->setValue(arg1);

        ui.doubleSpinBox_C3_length->blockSignals(false);
    }

    if (view3d->getSymmetry() == "T" || view3d->getSymmetry() == "Td" ||
        view3d->getSymmetry() == "Th") {
        ui.doubleSpinBox_Cn_length->blockSignals(true);
        ui.doubleSpinBox_Cn_length->setValue(arg1);
        view3d->setCn_Length(arg1);
        ui.doubleSpinBox_Cn_length->blockSignals(false);
    }

    view3d->refresh();
}

void MainWindow::on_doubleSpinBox_C3_length_valueChanged(double arg1) {
    if (isFirstTime)
        return;

    view3d->setC3_Length(arg1);

    if (_isDn)
        view3d->setHorizontalLengh(arg1);

    if (view3d->getSymmetry().startsWith("D")) {
        QString o = view3d->getSymmetry().at(1);
        unsigned int ord = o.toInt();
        if (ord % 2 == 0)
            isC3toC2 = true;
        else
            isC3toC2 = false;
    }

    if (isC3toC2) {
        ui.doubleSpinBox_C2_length->blockSignals(true);

        ui.doubleSpinBox_C2_length->setValue(arg1);
        view3d->setC2_Length(arg1);

        ui.doubleSpinBox_C2_length->blockSignals(false);
    }

    view3d->refresh();
}

void MainWindow::on_checkBox_TypeC2_I_stateChanged(int arg1) {
    if (ui.checkBox_TypeC2_I->isChecked()) {
        view3d->setHorizontalC2_type(1);
    } else {
        view3d->setHorizontalC2_type(2);
    }

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_checkBox_TypeC2_II_stateChanged(int arg1) {
    if (ui.checkBox_TypeC2_II->isChecked()) {
        view3d->setHorizontalC2_type(3);
    } else {
        view3d->setHorizontalC2_type(4);
    }

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_Cn_red_valueChanged(int arg1) {
    double a = 255.0;
    view3d->setColorCn(ui.spinBox_Cn_red->value() / a,
                       ui.spinBox_Cn_green->value() / a,
                       ui.spinBox_Cn_blue->value() / a);
    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_Cn.x())
                    .arg(Color_Cn.y())
                    .arg(Color_Cn.z());
    ui.pushButton_Cn->setStyleSheet(s);

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_Cn_green_valueChanged(int arg1) {
    double a = 255.0;
    view3d->setColorCn(ui.spinBox_Cn_red->value() / a,
                       ui.spinBox_Cn_green->value() / a,
                       ui.spinBox_Cn_blue->value() / a);
    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_Cn.x())
                    .arg(Color_Cn.y())
                    .arg(Color_Cn.z());
    ui.pushButton_Cn->setStyleSheet(s);
    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_Cn_blue_valueChanged(int arg1) {
    double a = 255.0;

    view3d->setColorCn(ui.spinBox_Cn_red->value() / a,
                       ui.spinBox_Cn_green->value() / a,
                       ui.spinBox_Cn_blue->value() / a);
    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_Cn.x())
                    .arg(Color_Cn.y())
                    .arg(Color_Cn.z());
    ui.pushButton_Cn->setStyleSheet(s);

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_pushButton_Cn_clicked() {
    if (isFirstTime)
        return;

    QColor color(Color_Cn.x(), Color_Cn.y(), Color_Cn.z(), 1);

    color_widgets::ColorDialog dialog;
    dialog.showNormal();
    dialog.setColor(color);
    dialog.exec();
    dialog.showNormal();

    QColor c = dialog.color();
    Color_Cn.Set(c.red(), c.green(), c.blue());

    // Color_Cn=c.alpha();

    ui.spinBox_Cn_red->blockSignals(true);
    ui.spinBox_Cn_blue->blockSignals(true);
    ui.spinBox_Cn_green->blockSignals(true);

    ui.spinBox_Cn_red->setValue(Color_Cn.x());
    ui.spinBox_Cn_green->setValue(Color_Cn.y());
    ui.spinBox_Cn_blue->setValue(Color_Cn.z());

    ui.spinBox_Cn_red->blockSignals(false);
    ui.spinBox_Cn_blue->blockSignals(false);
    ui.spinBox_Cn_green->blockSignals(false);

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_Cn.x())
                    .arg(Color_Cn.y())
                    .arg(Color_Cn.z());
    ui.pushButton_Cn->setStyleSheet(s);

    double a = 255.0;
    view3d->setColorCn(ui.spinBox_Cn_red->value() / a,
                       ui.spinBox_Cn_green->value() / a,
                       ui.spinBox_Cn_blue->value() / a);

    if (ui.treeWidget->isT() || ui.treeWidget->isTd() || ui.treeWidget->isTh()) {
        view3d->setColorC2_1(ui.spinBox_Cn_red->value() / a,
                             ui.spinBox_Cn_green->value() / a,
                             ui.spinBox_Cn_blue->value() / a);

        Color_C2.Set(c.red(), c.green(), c.blue());

        ui.pushButton_C2->setStyleSheet(s);

        ui.spinBox_C2_red->blockSignals(true);
        ui.spinBox_C2_blue->blockSignals(true);
        ui.spinBox_C2_green->blockSignals(true);

        ui.spinBox_C2_red->setValue(Color_Cn.x());
        ui.spinBox_C2_green->setValue(Color_Cn.y());
        ui.spinBox_C2_blue->setValue(Color_Cn.z());

        ui.spinBox_C2_red->blockSignals(false);
        ui.spinBox_C2_blue->blockSignals(false);
        ui.spinBox_C2_green->blockSignals(false);
    }

    // if(isFirstTime) return;
    view3d->renderMol();
}

void MainWindow::on_pushButton_C2_clicked() {
    if (isFirstTime)
        return;

    QColor color(Color_C2.x(), Color_C2.y(), Color_C2.z(), Opacity_bk);

    color_widgets::ColorDialog dialog;
    dialog.showNormal();
    dialog.setColor(color);
    dialog.exec();

    QColor c = dialog.color();
    Color_C2.Set(c.red(), c.green(), c.blue());
    // Color_Cn=c.alpha();

    ui.spinBox_C2_red->blockSignals(true);
    ui.spinBox_C2_green->blockSignals(true);
    ui.spinBox_C2_blue->blockSignals(true);

    ui.spinBox_C2_red->setValue(Color_C2.x());
    ui.spinBox_C2_green->setValue(Color_C2.y());
    ui.spinBox_C2_blue->setValue(Color_C2.z());

    ui.spinBox_C2_red->blockSignals(false);
    ui.spinBox_C2_green->blockSignals(false);
    ui.spinBox_C2_blue->blockSignals(false);

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_C2.x())
                    .arg(Color_C2.y())
                    .arg(Color_C2.z());
    ui.pushButton_C2->setStyleSheet(s);

    double a = 255.0;
    view3d->setColorC2_1(ui.spinBox_C2_red->value() / a,
                         ui.spinBox_C2_green->value() / a,
                         ui.spinBox_C2_blue->value() / a);

    if (ui.treeWidget->isT() || ui.treeWidget->isTd() || ui.treeWidget->isTh()) {
        view3d->setColorCn(ui.spinBox_C2_red->value() / a,
                           ui.spinBox_C2_green->value() / a,
                           ui.spinBox_C2_blue->value() / a);

        Color_Cn.Set(c.red(), c.green(), c.blue());
        ui.pushButton_Cn->setStyleSheet(s);

        ui.spinBox_Cn_red->blockSignals(true);
        ui.spinBox_Cn_blue->blockSignals(true);
        ui.spinBox_Cn_green->blockSignals(true);

        ui.spinBox_Cn_red->setValue(Color_Cn.x());
        ui.spinBox_Cn_green->setValue(Color_Cn.y());
        ui.spinBox_Cn_blue->setValue(Color_Cn.z());

        ui.spinBox_Cn_red->blockSignals(false);
        ui.spinBox_Cn_blue->blockSignals(false);
        ui.spinBox_Cn_green->blockSignals(false);
    }

    view3d->renderMol();
}

void MainWindow::on_pushButton_C3_clicked() {
    if (isFirstTime)
        return;

    QColor color(Color_C3.x(), Color_C3.y(), Color_C3.z(), Opacity_bk);

    color_widgets::ColorDialog dialog;
    dialog.showNormal();
    dialog.setColor(color);
    dialog.exec();

    QColor c = dialog.color();
    Color_C3.Set(c.red(), c.green(), c.blue());
    // Color_Cn=c.alpha();

    disconnect(ui.spinBox_C3_red, SIGNAL(valueChanged(int)), this,
               SLOT(on_spinBox_Cn_red_valueChanged(int)));
    disconnect(ui.spinBox_C3_blue, SIGNAL(valueChanged(int)), this,
               SLOT(on_spinBox_Cn_blue_valueChanged(int)));
    disconnect(ui.spinBox_C3_green, SIGNAL(valueChanged(int)), this,
               SLOT(on_spinBox_Cn_green_valueChanged(int)));

    ui.spinBox_C3_red->setValue(Color_C3.x());
    ui.spinBox_C3_green->setValue(Color_C3.y());
    ui.spinBox_C3_blue->setValue(Color_C3.z());

    connect(ui.spinBox_C3_red, SIGNAL(valueChanged(int)), this,
            SLOT(on_spinBox_Cn_red_valueChanged(int)));
    connect(ui.spinBox_C3_blue, SIGNAL(valueChanged(int)), this,
            SLOT(on_spinBox_Cn_blue_valueChanged(int)));
    connect(ui.spinBox_C3_green, SIGNAL(valueChanged(int)), this,
            SLOT(on_spinBox_Cn_green_valueChanged(int)));

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_C3.x())
                    .arg(Color_C3.y())
                    .arg(Color_C3.z());
    ui.pushButton_C3->setStyleSheet(s);

    double a = 255.0;
    view3d->setColorC3(ui.spinBox_C3_red->value() / a,
                       ui.spinBox_C3_green->value() / a,
                       ui.spinBox_C3_blue->value() / a);

    if (isC3toC2) {
        view3d->setColorC2_2(ui.spinBox_C3_red->value() / a,
                             ui.spinBox_C3_green->value() / a,
                             ui.spinBox_C3_blue->value() / a);
    }

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_C2_red_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    view3d->setColorC2_1(ui.spinBox_C2_red->value() / a,
                         ui.spinBox_C2_green->value() / a,
                         ui.spinBox_C2_blue->value() / a);
    if (isFirstTime)
        return;

    view3d->renderMol();
}

void MainWindow::on_spinBox_C2_green_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    view3d->setColorC2_1(ui.spinBox_C2_red->value() / a,
                         ui.spinBox_C2_green->value() / a,
                         ui.spinBox_C2_blue->value() / a);

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_C2.x())
                    .arg(Color_C2.y())
                    .arg(Color_C2.z());
    ui.pushButton_C2->setStyleSheet(s);

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_C2_blue_valueChanged(int arg1) {
    double a = 255.0;
    view3d->setColorC2_1(ui.spinBox_C2_red->value() / a,
                         ui.spinBox_C2_green->value() / a,
                         ui.spinBox_C2_blue->value() / a);

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_C2.x())
                    .arg(Color_C2.y())
                    .arg(Color_C2.z());
    ui.pushButton_C2->setStyleSheet(s);

    // if(isFirstTime) return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_C3_red_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    view3d->setColorC3(ui.spinBox_C3_red->value() / a,
                       ui.spinBox_C3_green->value() / a,
                       ui.spinBox_C3_blue->value() / a);
    if (isC3toC2) {
        view3d->setColorC2_2(ui.spinBox_C3_red->value() / a,
                             ui.spinBox_C3_green->value() / a,
                             ui.spinBox_C3_blue->value() / a);
    }

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_C3.x())
                    .arg(Color_C3.y())
                    .arg(Color_C3.z());
    ui.pushButton_C3->setStyleSheet(s);

    if (isC3toC2) {
        view3d->setColorC2_2(ui.spinBox_C3_red->value() / a,
                             ui.spinBox_C3_green->value() / a,
                             ui.spinBox_C3_blue->value() / a);
    }

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_C3_green_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    view3d->setColorC3(ui.spinBox_C3_red->value() / a,
                       ui.spinBox_C3_green->value() / a,
                       ui.spinBox_C3_blue->value() / a);
    if (isC3toC2) {
        view3d->setColorC2_2(ui.spinBox_C3_red->value() / a,
                             ui.spinBox_C3_green->value() / a,
                             ui.spinBox_C3_blue->value() / a);
    }

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_C3.x())
                    .arg(Color_C3.y())
                    .arg(Color_C3.z());
    ui.pushButton_C3->setStyleSheet(s);

    if (isC3toC2) {
        view3d->setColorC2_2(ui.spinBox_C3_red->value() / a,
                             ui.spinBox_C3_green->value() / a,
                             ui.spinBox_C3_blue->value() / a);
    }

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_C3_blue_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    view3d->setColorC3(ui.spinBox_C3_red->value() / a,
                       ui.spinBox_C3_green->value() / a,
                       ui.spinBox_C3_blue->value() / a);
    if (isC3toC2) {
        view3d->setColorC2_2(ui.spinBox_C3_red->value() / a,
                             ui.spinBox_C3_green->value() / a,
                             ui.spinBox_C3_blue->value() / a);
    }

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_C3.x())
                    .arg(Color_C3.y())
                    .arg(Color_C3.z());
    ui.pushButton_C3->setStyleSheet(s);

    if (isC3toC2) {
        view3d->setColorC2_2(ui.spinBox_C3_red->value() / a,
                             ui.spinBox_C3_green->value() / a,
                             ui.spinBox_C3_blue->value() / a);
    }

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_V_red_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    Color_V.Set(ui.spinBox_V_red->value(), ui.spinBox_V_green->value(),
                ui.spinBox_V_blue->value());

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_V.x())
                    .arg(Color_V.y())
                    .arg(Color_V.z());
    ui.pushButton_V->setStyleSheet(s);

    view3d->setColorSigmaV(Color_V / a);
    view3d->renderMol();
}

void MainWindow::on_spinBox_V_green_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    Color_V.Set(ui.spinBox_V_red->value(), ui.spinBox_V_green->value(),
                ui.spinBox_V_blue->value());

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_V.x())
                    .arg(Color_V.y())
                    .arg(Color_V.z());
    ui.pushButton_V->setStyleSheet(s);

    view3d->setColorSigmaV(Color_V / a);

    view3d->renderMol();
}

void MainWindow::on_spinBox_V_blue_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    Color_V.Set(ui.spinBox_V_red->value(), ui.spinBox_V_green->value(),
                ui.spinBox_V_blue->value());

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_V.x())
                    .arg(Color_V.y())
                    .arg(Color_V.z());
    ui.pushButton_V->setStyleSheet(s);

    view3d->setColorSigmaV(Color_V / a);
    view3d->renderMol();
}

void MainWindow::on_spinBox_V_opacity_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    view3d->setOpacityV(ui.spinBox_V_opacity->value() / a);

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_pushButton_V_clicked() {
    if (isFirstTime)
        return;

    QColor color(Color_V.x(), Color_V.y(), Color_V.z(), Opacity_V);

    color_widgets::ColorDialog dialog;
    dialog.showNormal();
    dialog.setColor(color);
    dialog.exec();

    QColor c = dialog.color();
    Color_V.Set(c.red(), c.green(), c.blue());

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_V.x())
                    .arg(Color_V.y())
                    .arg(Color_V.z());
    ui.pushButton_V->setStyleSheet(s);

    ui.spinBox_V_red->blockSignals(true);
    ui.spinBox_V_green->blockSignals(true);
    ui.spinBox_V_blue->blockSignals(true);
    ui.spinBox_V_opacity->blockSignals(true);

    ui.spinBox_V_red->setValue(Color_V.x());
    ui.spinBox_V_green->setValue(Color_V.y());
    ui.spinBox_V_blue->setValue(Color_V.z());
    // ui.spinBox_V_opacity->setValue(Opacity_V);

    ui.spinBox_V_red->blockSignals(false);
    ui.spinBox_V_green->blockSignals(false);
    ui.spinBox_V_blue->blockSignals(false);
    ui.spinBox_V_opacity->blockSignals(false);

    double a = 255.0;
    view3d->setColorSigmaV(Color_V / a);

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_pushButton_D_clicked() {
    if (isFirstTime)
        return;

    QColor color(Color_D.x(), Color_D.y(), Color_D.z(), Opacity_D);

    color_widgets::ColorDialog dialog;
    dialog.showNormal();
    dialog.setColor(color);
    dialog.exec();

    QColor c = dialog.color();
    Color_D.Set(c.red(), c.green(), c.blue());

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_D.x())
                    .arg(Color_D.y())
                    .arg(Color_D.z());
    ui.pushButton_D->setStyleSheet(s);

    ui.spinBox_D_red->blockSignals(true);
    ui.spinBox_D_green->blockSignals(true);
    ui.spinBox_D_blue->blockSignals(true);
    ui.spinBox_D_opacity->blockSignals(true);

    ui.spinBox_D_red->setValue(Color_D.x());
    ui.spinBox_D_green->setValue(Color_D.y());
    ui.spinBox_D_blue->setValue(Color_D.z());
    // ui.spinBox_D_opacity->setValue(Opacity_D);

    ui.spinBox_D_red->blockSignals(false);
    ui.spinBox_D_green->blockSignals(false);
    ui.spinBox_D_blue->blockSignals(false);
    ui.spinBox_D_opacity->blockSignals(false);

    double a = 255.0;
    view3d->setColorSigmaD(Color_D / a);

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_D_red_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    Color_D.Set(ui.spinBox_D_red->value(), ui.spinBox_D_green->value(),
                ui.spinBox_D_blue->value());

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_D.x())
                    .arg(Color_D.y())
                    .arg(Color_D.z());
    ui.pushButton_D->setStyleSheet(s);

    view3d->setColorSigmaD(Color_D / a);
    view3d->renderMol();
}

void MainWindow::on_spinBox_D_green_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    Color_D.Set(ui.spinBox_D_red->value(), ui.spinBox_D_green->value(),
                ui.spinBox_D_blue->value());

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_D.x())
                    .arg(Color_D.y())
                    .arg(Color_D.z());
    ui.pushButton_D->setStyleSheet(s);

    view3d->setColorSigmaD(Color_D / a);
    view3d->renderMol();
}

void MainWindow::on_spinBox_D_blue_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    Color_D.Set(ui.spinBox_D_red->value(), ui.spinBox_D_green->value(),
                ui.spinBox_D_blue->value());

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_D.x())
                    .arg(Color_D.y())
                    .arg(Color_D.z());
    ui.pushButton_D->setStyleSheet(s);

    view3d->setColorSigmaD(Color_D / a);
    view3d->renderMol();
}

void MainWindow::on_spinBox_D_opacity_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    view3d->setOpacityD(ui.spinBox_D_opacity->value() / a);

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_pushButton_H_clicked() {
    if (isFirstTime)
        return;

    QColor color(Color_H.x(), Color_H.y(), Color_H.z(), Opacity_H);

    color_widgets::ColorDialog dialog;
    dialog.showNormal();
    dialog.setColor(color);
    dialog.exec();

    QColor c = dialog.color();
    Color_H.Set(c.red(), c.green(), c.blue());

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_H.x())
                    .arg(Color_H.y())
                    .arg(Color_H.z());
    ui.pushButton_H->setStyleSheet(s);

    ui.spinBox_H_red->blockSignals(true);
    ui.spinBox_H_green->blockSignals(true);
    ui.spinBox_H_blue->blockSignals(true);
    ui.spinBox_H_opacity->blockSignals(true);

    ui.spinBox_H_red->setValue(Color_H.x());
    ui.spinBox_H_green->setValue(Color_H.y());
    ui.spinBox_H_blue->setValue(Color_H.z());
    // ui.spinBox_H_opacity->setValue(Opacity_H);

    ui.spinBox_H_red->blockSignals(false);
    ui.spinBox_H_green->blockSignals(false);
    ui.spinBox_H_blue->blockSignals(false);
    ui.spinBox_H_opacity->blockSignals(false);

    double a = 255.0;
    view3d->setColorSigmaH(Color_H / a);

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_H_red_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    Color_H.Set(ui.spinBox_H_red->value(), ui.spinBox_H_green->value(),
                ui.spinBox_H_blue->value());

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_H.x())
                    .arg(Color_H.y())
                    .arg(Color_H.z());
    ui.pushButton_H->setStyleSheet(s);

    view3d->setColorSigmaH(Color_H / a);
    view3d->renderMol();
}

void MainWindow::on_spinBox_H_green_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    Color_H.Set(ui.spinBox_H_red->value(), ui.spinBox_H_green->value(),
                ui.spinBox_H_blue->value());

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_H.x())
                    .arg(Color_H.y())
                    .arg(Color_H.z());
    ui.pushButton_H->setStyleSheet(s);
    view3d->setColorSigmaH(Color_H / a);
    view3d->renderMol();
}

void MainWindow::on_spinBox_H_blue_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    Color_H.Set(ui.spinBox_H_red->value(), ui.spinBox_H_green->value(),
                ui.spinBox_H_blue->value());

    QString s = tr("background-color: rgb(%1,%2,%3);")
                    .arg(Color_H.x())
                    .arg(Color_H.y())
                    .arg(Color_H.z());
    ui.pushButton_H->setStyleSheet(s);

    view3d->setColorSigmaH(Color_H / a);
    view3d->renderMol();
}

void MainWindow::on_spinBox_H_opacity_valueChanged(int arg1) {
    if (isFirstTime)
        return;

    double a = 255.0;
    view3d->setOpacityH(ui.spinBox_H_opacity->value() / a);

    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_V_type_valueChanged(int arg1) {
    view3d->setSigmaV_Type(arg1);
    // if(isFirstTime) return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_D_type_valueChanged(int arg1) {
    view3d->setSigmaD_Type(arg1);
    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_spinBox_H_type_valueChanged(int arg1) {
    view3d->setSigmaH_Type(arg1);
    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_pushButton_thermalMove_clicked() {
    mol->thermalDisplacement(100);
    view3d->renderMol();
}

void MainWindow::on_pushButton_rotate2_clicked() {
    if (mol == nullptr)
        return;

    float x, y, z, err = 0.01;
    x = (ui.lineEdit_XX->text()).toFloat();
    y = (ui.lineEdit_YY->text()).toFloat();
    z = (ui.lineEdit_ZZ->text()).toFloat();

    if (abs(x) < err && abs(y) < err && abs(z) < err) {
        QMessageBox::information(0, "Warning", "Please input rotation degree!");
        return;
    }

    if (abs(x) > err && abs(y) > err) {
        QMessageBox::information(0, "Warning", "Only one axis can be defined!");
        return;
    }

    if (abs(y) > err && abs(z) > err) {
        QMessageBox::information(0, "Warning", "Only one axis can be defined!");
        return;
    }

    if (abs(x) > err && abs(z) > err) {
        QMessageBox::information(0, "Warning", "Only one axis can be defined!");
        return;
    }

    vector3 v(0.0, 0.0, 0.0);

    if (abs(x) > err) {
        v.SetX(1.0);
        mol->rotate(v, x);
    }

    if (abs(y) > err) {
        v.SetY(1.0);
        mol->rotate(v, y);
    }

    if (abs(z) > err) {

        v.SetZ(1.0);
        mol->rotate(v, z);
    }

    view3d->renderMol();
    push2Stack("rotateMol");

    view3d->setDefaultOperationMode();
}

void MainWindow::on_pushButton_translate2_clicked() {
    if (mol == nullptr)
        return;

    float x, y, z, err = 0.0000001;
    x = (ui.lineEdit_XX->text()).toFloat();
    y = (ui.lineEdit_YY->text()).toFloat();
    z = (ui.lineEdit_ZZ->text()).toFloat();

    if (abs(x) < err && abs(y) < err && abs(z) < err) {
        QMessageBox::information(0, "Warning", "Please input rotation degree!");
        return;
    }

    if (abs(x) > err && abs(y) > err) {
        QMessageBox::information(0, "Warning", "Only one axis can be defined!");
        return;
    }

    if (abs(y) > err && abs(z) > err) {
        QMessageBox::information(0, "Warning", "Only one axis can be defined!");
        return;
    }

    if (abs(x) > err && abs(z) > err) {
        QMessageBox::information(0, "Warning", "Only one axis can be defined!");
        return;
    }

    mol->translate(x, y, z);

    view3d->renderMol(mol);

    push2Stack("translateMol");

    view3d->setDefaultOperationMode();
}

void MainWindow::on_pushButton_PatchMolecule_PG_clicked() {
    PGSymmetry *symmetry = new PGSymmetry(mol, 0.1);
    symmetry->setSymmetry(ui.comboBox_symmetry->currentText().toStdString());

    unsigned int NAtomsofPatched;
    symmetry->patchMolecule(NAtomsofPatched);
    if (NAtomsofPatched < 1)
        return;

    symmetry->updateMolData();
    delete symmetry;

    mol->reCalculateBonds();
    view3d->renderMol(mol);
    view3d->setDefaultOperationMode();

    push2Stack("Patch Molecule based on symmetry");
}

void MainWindow::on_dialAtomScale_valueChanged(int value) {
    view3d->setAtomScale(double(ui.dialAtomScale->value()) / 200.0);
    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_dialBondScale_valueChanged(int value) {
    view3d->setBondScale(double(ui.dialBondScale->value()) / 200.0);
    if (isFirstTime)
        return;
    view3d->renderMol();
}

void MainWindow::on_actionEditUndo_triggered() {
    if (undoList.size() < 1)
        return;

    // cout << "Undo :"<<endl;
    // cout << "size of undo and redo " <<undoList.size() << " " <<
    // redoList.size()<<endl;

    QString All = undoList.pop();
    redoList.push_back(All);

    // cout << "Recoved data:\n";
    // cout << All.toStdString().c_str();
    // cout << undoList.size() <<endl;

    if (undoList.size() > 0)
        All = undoList.last();
    else {
        mol->clearAll();
        view3d->renderMol(mol);
        return;
    }
    recoverStackData(All);
    ui.textEdit_dataMol->setText(dataMol0);

    // cout << mol->NumAtoms()<<endl;
}

void MainWindow::on_actionEditRedo_triggered() {
    if (redoList.size() < 1)
        return;

    QString All = redoList.pop();
    undoList.push_back(All);

    recoverStackData(All);
}

void MainWindow::on_pushButton_center_PG_clicked() {
    on_actionToolscentralize_triggered();
}

void MainWindow::on_actionToolscentralize_triggered() {
    view3d->setDefaultOperationMode();

    if (mol == nullptr)
        return;
    if (mol->NumAtoms() > 1) {
        mol->centralize();

        view3d->renderMol(mol);
        push2Stack("centralizeMol");
    }
}

void MainWindow::on_actionFileExport_triggered() {

    if (mol->NumAtoms() < 1)
        return;

    QStringList filters;
    filters << tr("XYZ") + " (*.xyz)" << tr("MDL Mol") + " (*.mol)"
            << tr("PDB") + " (*.pdb)" << tr("Sybyl Mol2") + " (*.mol2)";

    QString currentSuffix = QFileInfo(FileName).suffix();
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Save File"), QDir::currentPath(), filters.join(";;"));
    if (currentSuffix.size() < 1)
        currentSuffix = "xyz";

    if (fileName.isEmpty())
        return;

    OpenBabel::OBConversion conv;
    OpenBabel::OBFormat *format =
        conv.FindFormat(QFileInfo(fileName).suffix().toStdString().c_str());
    QString fileType = format->GetID();

    if (mol->writeMol(fileName, fileType)) {
        return;
    } else {
        QMessageBox::information(0, "Warning", "Cannot export moleclue!");
    }
}

void MainWindow::on_actionEditbond_triggered() {
    if ((view3d->numSelectedAtoms() == 2) || (view3d->numSelectedBonds() == 1)) {
        auto form = new bondProperty();
        if (form != nullptr) {
            form->setParent(this);
            form->setAttribute(Qt::WA_DeleteOnClose, true);
            form->exec();
        }
    }
}

void MainWindow::on_actionEditangle_triggered() {
    unsigned int num = view3d->numSelectedAtoms();
    if (num != 3)
        return;

    AngleProperty *form = new AngleProperty();
    if (form != nullptr) {
        form->setParent(this);
        form->setAttribute(Qt::WA_DeleteOnClose, true);
        form->exec();
    }
}

void MainWindow::on_actionEditdihedral_triggered() {
    unsigned int num = view3d->numSelectedAtoms();
    if (num != 4)
        return;

    TorsionProperty *form = new TorsionProperty();
    if (form != nullptr) {
        form->setParent(this);
        form->setAttribute(Qt::WA_DeleteOnClose, true);
        form->exec();
    }

    // view3d->setDefaultOperationMode();
}

void MainWindow::on_actionSymmetryView_triggered() {
    if (ui.tabWidget_2->isHidden()) {
        ui.tabWidget_2->show();
        if (mol->NumAtoms() > 2)
            on_pushButton_Symmetrypercept_PG_clicked();
    } else
        ui.tabWidget_2->hide();
}

void MainWindow::on_actionSingleBond_triggered() {
    if (view3d->numSelectedBonds() + view3d->numSelectedAtoms() < 1)
        return;

    view3d->setSelectedBondOrder(1);
    view3d->refresh();
}

void MainWindow::on_actionDoubleBond_triggered() {
    if (view3d->numSelectedBonds() + view3d->numSelectedAtoms() < 1)
        return;

    view3d->setSelectedBondOrder(2);
    view3d->refresh();
    return;
}

void MainWindow::on_actionTripleBond_triggered() {
    if (view3d->numSelectedBonds() + view3d->numSelectedAtoms() < 1)
        return;

    view3d->setSelectedBondOrder(3);
    view3d->refresh();
}

void MainWindow::on_actionQuadrupleBond_triggered() {
    if (view3d->numSelectedBonds() + view3d->numSelectedAtoms() < 1)
        return;

    view3d->setSelectedBondOrder(4);
    view3d->refresh();
}

void MainWindow::on_actionWeakBond_triggered() {
    if (view3d->numSelectedBonds() + view3d->numSelectedAtoms() < 1)
        return;

    view3d->setSelectedBondOrder(7);
    view3d->refresh();
}

void MainWindow::on_actionHydrogenbond_triggered() {
    if (view3d->numSelectedBonds() + view3d->numSelectedAtoms() < 1)
        return;

    view3d->setSelectedBondOrder(5);
    view3d->refresh();
}

void MainWindow::on_actionAromaticBond_triggered() {
    if (view3d->numSelectedBonds() + view3d->numSelectedAtoms() < 1)
        return;

    view3d->setSelectedBondOrder(6);
    view3d->refresh();
}

void MainWindow::on_pushButton_addHydrogen_clicked() {
    if (view3d->numSelectedAtoms() > 0) {
        for (unsigned int i = 0; i < view3d->selectedAtomList.size(); i++)
            mol->autoAddHydrogen(view3d->selectedAtomList[i]);

        view3d->releaseAllActors();
        view3d->renderMol(mol);
        QApplication::restoreOverrideCursor();

        push2Stack("Adding-Hydrogen");

    } else {
        QMessageBox::warning(this, "Warning", "请选择加H原子!");
        return;
    }
}

void MainWindow::on_actionToolsAddHydrogenAuto_triggered() {
    view3d->setDefaultOperationMode();

    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (view3d->numSelectedAtoms() > 0) {
        for (unsigned int i = 0; i < view3d->selectedAtomList.size(); i++) {
            mol->autoAddHydrogen(view3d->selectedAtomList[i]);
        }

    } else {
        mol->autoAdjustHydrogenWhole();
    }

    view3d->releaseAllActors();
    view3d->renderMol(mol);
    QApplication::restoreOverrideCursor();

    push2Stack("Adding-Hydrogen");
}

void MainWindow::on_actionMMFF94_triggered() {
    view3d->setDefaultOperationMode();

    if (mol->isXExisted()) {
        QMessageBox::warning(this, "Warning", "请删除X原子!");
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    mol->runMolecularMechanics("MMFF94");

    view3d->releaseAllActors();
    view3d->renderMol(mol);
    QApplication::restoreOverrideCursor();
    push2Stack("cleanMol");
}

void MainWindow::on_actionGhemical_triggered() {
    // view3d->setDefaultOperationMode();
    if (mol->isXExisted()) {
        QMessageBox::warning(this, "Warning", "请删除X原子!");
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    mol->runMolecularMechanics("Ghemical");

    view3d->releaseAllActors2();

    view3d->renderMol(mol);
    QApplication::restoreOverrideCursor();
    push2Stack("cleanMol");
}

void MainWindow::on_actionUFF_triggered() {
    // view3d->setDefaultOperationMode();
    if (mol->isXExisted()) {
        QMessageBox::warning(this, "Warning", "请删除X原子!");
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);
    mol->runMolecularMechanics("UFF");

    view3d->releaseAllActors2();
    view3d->renderMol(mol);
    QApplication::restoreOverrideCursor();
    push2Stack("cleanMol");
}

void MainWindow::on_actionMM2_triggered() {
    // view3d->setDefaultOperationMode();
    if (mol->isXExisted()) {
        QMessageBox::warning(this, "Warning", "请删除X原子!");
        return;
    }
    return;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    mol->runMolecularMechanics("MM2");

    view3d->releaseAllActors2();
    view3d->renderMol(mol);
    QApplication::restoreOverrideCursor();
    push2Stack("cleanMol");
}

void MainWindow::on_actionGaff_triggered() {
    // view3d->setDefaultOperationMode();
    if (mol->isXExisted()) {
        QMessageBox::warning(this, "Warning", "请删除X原子!");
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    mol->runMolecularMechanics("GAFF");

    view3d->releaseAllActors2();
    view3d->renderMol(mol);
    QApplication::restoreOverrideCursor();
    push2Stack("cleanMol");
}

void MainWindow::loadDefaultColorSets() {
    QString s;

    // color
    Color_bk.Set(255.0 * 0.4, 255.0 * 0.5, 255.0 * 0.6);
    Color_H.Set(255.0 * 0.749, 255.0 * 0.815, 255.0 * 0.815);
    Color_V.Set(255.0 * 0.10, 255.0 * 0.86, 255.0 * 0.94);
    Color_D.Set(255.0 * 0.3, 255.0 * 0.9, 255.0 * 0.4);

    Color_Cn.Set(255.0 * 0.9, 255.0 * 0.1, 255.0 * 0.1);
    Color_C2.Set(255.0 * 0.10, 255.0 * 0.86, 255.0 * 0.94);
    Color_C3.Set(255.0 * 0.3, 255.0 * 0.9, 255.0 * 0.4);

    ui.spinBox_V_red->setValue(Color_V.x());
    ui.spinBox_V_green->setValue(Color_V.y());
    ui.spinBox_V_blue->setValue(Color_V.z());

    ui.spinBox_D_red->setValue(Color_D.x());
    ui.spinBox_D_green->setValue(Color_D.y());
    ui.spinBox_D_blue->setValue(Color_D.z());

    ui.spinBox_H_red->setValue(Color_H.x());
    ui.spinBox_H_green->setValue(Color_H.y());
    ui.spinBox_H_blue->setValue(Color_H.z());

    // ui.spinBox_bk_red->setValue(Color_bk.x());
    // ui.spinBox_bk_green->setValue(Color_bk.y());
    // ui.spinBox_bk_blue->setValue(Color_bk.z());

    ui.spinBox_Cn_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");
    ui.spinBox_C2_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");
    ui.spinBox_C3_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");

    ui.spinBox_D_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");
    ui.spinBox_H_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");
    ui.spinBox_V_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");
    // ui.spinBox_bk_red->setStyleSheet("QSpinBox{color:rgb(255,0,0)}");

    ui.spinBox_Cn_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");
    ui.spinBox_C2_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");
    ui.spinBox_C3_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");

    ui.spinBox_D_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");
    ui.spinBox_H_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");
    ui.spinBox_V_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");
    // ui.spinBox_bk_green->setStyleSheet("QSpinBox{color:rgb(0,255,0)}");

    ui.spinBox_Cn_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");
    ui.spinBox_C2_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");
    ui.spinBox_C3_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");

    ui.spinBox_D_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");
    ui.spinBox_H_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");
    ui.spinBox_V_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");
    // ui.spinBox_bk_blue->setStyleSheet("QSpinBox{color:rgb(0,0,255)}");

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_V.x())
            .arg(Color_V.y())
            .arg(Color_V.z());
    ui.pushButton_V->setStyleSheet(s);

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_D.x())
            .arg(Color_D.y())
            .arg(Color_D.z());
    ui.pushButton_D->setStyleSheet(s);

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_H.x())
            .arg(Color_H.y())
            .arg(Color_H.z());
    ui.pushButton_H->setStyleSheet(s);

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_bk.x())
            .arg(Color_bk.y())
            .arg(Color_bk.z());
    // ui.pushButton_bk->setStyleSheet(s);

    Opacity_V = 255;
    Opacity_D = 255;
    Opacity_H = 255;

    ui.spinBox_V_opacity->setValue(Opacity_V);
    ui.spinBox_D_opacity->setValue(Opacity_D);
    ui.spinBox_H_opacity->setValue(Opacity_H);

    /*
  scaleThicknessSigma_V=view3d->getThicknessSigmaV_Scale();
  scaleThicknessSigma_D=view3d->getThicknessSigmaD_Scale();
  scaleThicknessSigma_H=view3d->getThicknessSigmaH_Scale();

  scaleThicknessSigmaV=scaleThicknessSigma_V;
  scaleThicknessSigmaD=scaleThicknessSigma_D;
  scaleThicknessSigmaH=scaleThicknessSigma_H;

  ui.doubleSpinBox_V_thickness->setValue(scaleThicknessSigma_V);
  ui.doubleSpinBox_D_thickness->setValue(scaleThicknessSigma_D);
  ui.doubleSpinBox_H_thickness->setValue(scaleThicknessSigma_H);

  ui.doubleSpinBox_radius_Cn->setValue(view3d->getRadiusCn_Scale());
  ui.doubleSpinBox_radius_C2->setValue(view3d->getRadiusC2_Scale());
  ui.doubleSpinBox_radius_C3->setValue(view3d->getRadiusC3_Scale());

  ui.doubleSpinBox_Cn_length->setValue(view3d->getCn_Length());
  ui.doubleSpinBox_C2_length->setValue(view3d->getC2_Length());
  ui.doubleSpinBox_C3_length->setValue(view3d->getC3_Length());
*/

    // Color_C2=255.0*view3d->getColorC2_1();
    // Color_C3=255.0*view3d->getColorC3();

    ui.spinBox_Cn_red->setValue(Color_Cn.x());
    ui.spinBox_Cn_green->setValue(Color_Cn.y());
    ui.spinBox_Cn_blue->setValue(Color_Cn.z());

    ui.spinBox_C2_red->setValue(Color_C2.x());
    ui.spinBox_C2_green->setValue(Color_C2.y());
    ui.spinBox_C2_blue->setValue(Color_C2.z());

    ui.spinBox_C3_red->setValue(Color_C3.x());
    ui.spinBox_C3_green->setValue(Color_C3.y());
    ui.spinBox_C3_blue->setValue(Color_C3.z());

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_Cn.x())
            .arg(Color_Cn.y())
            .arg(Color_Cn.z());
    ui.pushButton_Cn->setStyleSheet(s);

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_C2.x())
            .arg(Color_C2.y())
            .arg(Color_C2.z());
    ui.pushButton_C2->setStyleSheet(s);

    s = tr("background-color: rgb(%1,%2,%3);")
            .arg(Color_C3.x())
            .arg(Color_C3.y())
            .arg(Color_C3.z());
    ui.pushButton_C3->setStyleSheet(s);

    // ui.dialAtomScale->setValue(view3d->getAtomScale()*200);
    // ui.dialBondScale->setValue(view3d->getAtomScale()*200);
}

void MainWindow::on_actionreCalculateBonds_triggered() {
    on_pushButton_rebond_clicked();
}

void MainWindow::on_actionPeriodicTable_triggered() {
    if (view3d->numSelectedAtoms() < 1)
        return;

    PeriodicTable *pt = new PeriodicTable();
    pt->setView3d(view3d);
    pt->show();
}

void MainWindow::on_pushButton_rebond_clicked() {
    mol->reCalculateBonds();
    view3d->renderMol();
}

void MainWindow::on_pushButton_UFF_clicked() { on_actionUFF_triggered(); }

void MainWindow::on_pushButton_Ghemical_clicked() {
    on_actionGhemical_triggered();
}

void MainWindow::on_pushButton_removeX_clicked() {
    mol->removeX();
    view3d->renderMol(mol);
}

void MainWindow::on_actionXTB_optimize_triggered() {
    view3d->setDefaultOperationMode();

    unsigned int numAtoms = mol->NumAtoms();
    if (numAtoms < 1) {
        QMessageBox::warning(this, "Warning", "Empty molecue!");
        return;
    }

    if (mol->isXExisted()) {
        QMessageBox::warning(this, "Warning", "请删除X原子!");
        return;
    }

    ShowTextForm *form = new ShowTextForm("XTB calculation", nullptr);

    if (form != nullptr) {
        form->setMinimumWidth(1000);
        form->update();
        form->setCalXTB(true);
        form->setParent(this);
        form->setAttribute(Qt::WA_DeleteOnClose, true);
        form->exec();
    }
    push2Stack("running XTB");

    view3d->releaseAllActors();
}

void MainWindow::on_pushButton_GAFF_clicked() { on_actionGaff_triggered(); }

void MainWindow::on_pushButton_Atom_clicked() {
    if (view3d->numSelectedAtoms() < 1)
        return;

    auto form = new atomProperty(nullptr);
    if (form != nullptr) {
        form->setParent(this);
        form->setAttribute(Qt::WA_DeleteOnClose, true);
        form->exec();
    }
}

void MainWindow::on_pushButton_Bond_clicked() { on_actionEditbond_triggered(); }

void MainWindow::on_pushButton_Ring_clicked() {
    if (view3d->numSelectedRings() != 1)
        return;

    ringProperty *form = new ringProperty(nullptr);
    if (form != nullptr) {
        form->setParent(this);
        form->setAttribute(Qt::WA_DeleteOnClose, true);
        form->exec();
    }
}

void MainWindow::on_pushButton_Polyhedron_clicked() {}

void MainWindow::on_actionToolsPencil_triggered() {
    view3d->setPencilDrawMode();
    view3d->removeDynamicLineActor();
    view3d->releaseAllActors();
}

void MainWindow::on_actionEditCopy_triggered() {
    view3d->setDefaultOperationMode();
    view3d->copySeleted();
    // view3d->releaseAllActors();
}

void MainWindow::on_actionToolsRotate_triggered() {
    view3d->setDefaultOperationMode();
    // view3d->releaseAllActors();
    // view3d->updateView();
}

void MainWindow::on_actionEditCut_triggered() { view3d->cutSelected(); }
void MainWindow::on_actionEditPaste_triggered() { view3d->paste(); }

void MainWindow::on_actionxyz_triggered() {
    setXYZAxisVisible();
    // view3d->setXYZAxisVisible(!view3d->isXYZAxisVisible());
    // view3d->updateView();
}

void MainWindow::on_actionXOY_triggered() {
    view3d->setDefaultOperationMode();
    view3d->onViewXY();
}

void MainWindow::on_actionXOZ_triggered() {
    view3d->setDefaultOperationMode();
    view3d->onViewXZ();
}

void MainWindow::on_actionYOZ_triggered() {
    view3d->setDefaultOperationMode();
    view3d->onViewYZ();
}

void MainWindow::on_pushButton_startMovie_clicked() {
    ui.treeWidget->startMovie();
}

void MainWindow::on_pushButton_showAll_clicked() {
    ui.checkBox_PGElement->setCheckState(Qt::Unchecked);
    ui.checkBox_PGElement->setCheckState(Qt::Checked);
}

void MainWindow::on_pushButton_hideAll_clicked() {
    ui.checkBox_PGElement->setCheckState(Qt::Checked);
    ui.checkBox_PGElement->setCheckState(Qt::Unchecked);
}

bool MainWindow::isLoopPlaying() {
    if (ui.checkBox_loop->isChecked())
        return true;
    return false;
}

void MainWindow::setLoopPlayingStop() {
    ui.checkBox_loop->setCheckState(Qt::Unchecked);
}



void MainWindow::on_comboBox_example_currentTextChanged(const QString &arg1)
{
    QString path = QCoreApplication::applicationDirPath() + "/examples/";

    QFileInfo info(path);

    if (!info.isDir()) {
        QMessageBox::warning(0, tr("Error"), tr("Directory is not existed!"));
        return;
    }

    FileName = path + arg1 + ".xyz";
    qDebug()<<FileName<< " on_comboBox_example_activated";


    /*
  #ifdef WIN32
      FileName.replace("\/","\\");
  #endif

      QFile file(FileName);
      if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
          QMessageBox::information(0, "Warning","Cannot Open File: "+FileName);
          return;
      }
  */

    QFileInfo info_(FileName);
    if (!info_.isFile()) {
        QString t = FileName + " is not existed!";
        //QMessageBox::warning(0, tr("Error"), t);
        return;
    }

    loadFile(FileName);
    symmetryPercept();
}


void MainWindow::on_doubleSpinBox_movieOpacity_valueChanged(double arg1) {
    view3d->setOpacity_movie(arg1);
    view3d->refresh();
}

bool MainWindow::isC2_InVisible() {
    if (ui.checkBox_C2Hide->isChecked())
        return true;
    return false;
}
bool MainWindow::isC3_InVisible() {
    if (ui.checkBox_C3Hide->isChecked())
        return true;
    return false;
}
bool MainWindow::isSigmaV_InVisible() {
    if (ui.checkBox_SigmaV_Hide->isChecked())
        return true;
    return false;
}
bool MainWindow::isSigmaD_InVisible() {
    if (ui.checkBox_SigmaD_Hide->isChecked())
        return true;
    return false;
}
bool MainWindow::isSigmaH_InVisible() {
    if (ui.checkBox_SigmaH_Hide->isChecked())
        return true;
    return false;
}

bool MainWindow::isPGElement_InVisible() {
    if (!ui.checkBox_PGElement->isChecked())
        return true;

    return false;
}

void MainWindow::setPGElement_Visible() {
    if (!ui.checkBox_PGElement->isChecked()) {
        ui.checkBox_PGElement->blockSignals(true);
        ui.checkBox_PGElement->setCheckState(Qt::Checked);
        view3d->setSymmetryElementVisible(true);
        ui.checkBox_PGElement->blockSignals(false);
    }
}

void MainWindow::setPGElement_Visible(bool t) {
    ui.checkBox_PGElement->blockSignals(true);
    if (t) {
        ui.checkBox_PGElement->setCheckState(Qt::Checked);
        view3d->setSymmetryElementVisible(true);
    } else {
        ui.checkBox_PGElement->setCheckState(Qt::Unchecked);
        // view3d->setSymmetryElementVisible(t);
    }

    ui.checkBox_PGElement->blockSignals(false);
}

// up-down
void MainWindow::on_pushButton_flip_x_clicked() {
    if (!mol)
        return;

    mol->flip_X();

    view3d->refresh();
}

// front-back
void MainWindow::on_pushButton_flip_z_clicked() {
    if (!mol)
        return;
    mol->flip_Z();
    view3d->refresh();
}

// left-right
void MainWindow::on_pushButton_flip_y_clicked() {
    if (!mol)
        return;
    mol->flip_Y();
    view3d->refresh();
}

void MainWindow::on_action_author_triggered() {
    auto aboutdig = new AboutDialog(this);
    if (aboutdig != nullptr) {
        aboutdig->setAttribute(Qt::WA_DeleteOnClose, true);
        aboutdig->exec();
    }
}

void MainWindow::on_actionShortCut_triggered() {
    QString s = "Esc or q  : DefaultOperation\n";
    s += "space     : calculate distancde/angle/torsion or center\n";
    s += "home      : reset view\n";
    s += "key-up    : rotate up\n";
    s += "key-down  : rotate down\n";
    s += "key-right : rotate right\n";
    s += "key-left  : rotate left\n";
    s += "Shift     : begin selection mode\n";
    s += "1   : set Selected BondOrder to 1\n";
    s += "2   : (a) set Selected BondOrder to 2 (b) set C2 axis visible or not\n";
    s += "3   : (a) set Selected BondOrder to 3 (b) set C3 axis visible or not\n";
    s += "4   : (a) set Selected BondOrder to 4 (b) set C4 axis visible or not\n";
    s += "5   : (a) set Selected BondOrder to 5 (b) set C5 axis visible or not\n";
    s += "6   : (a) set Selected BondOrder to aromatic (b) set C6 axis visible "
         "or not\n";
    s += "7   : (a) set Selected BondOrder to weak\n";
    s += "                                                       \n";
    s += "Delete    : remove\n";
    s += "a   : select All\n";
    s += "b   : set select atom to B\n";
    s += "c   : (a) set select atom to C (b) centralize the whole molecule (c)\n";
    s += "d   : (a) set sigma-D visible of not (b) delete selected atoms/bonds\n";
    s += "e   : periodic Table visible\n";
    s += "f   : (a) set select atom to F (b) find symmetry\n";
    s += "i   : set center visible if existed\n";
    s += "h   : (a) set sigma-D visible of not (b) hide Hydrogen atom\n";
    s += "k   : set Kekule-style\n";
    s += "l   : set label visible or not\n";
    s += "m   : set whole molecule visible or not\n";
    s += "n   : set Cn  axis visible or not\n";
    s += "o   : set outline visible or not\n";
    s += "p   : set symmetry element visible or not\n";
    s += "r   : re-calculate long bonds\n";
    s += "s   : set select atom to S\n";
    s += "v   : (a) set sigma-V visible of not (b) ctrl-V paste\n";
    s += "x   : cut \n";
    s += "y   : redo\n";
    s += "z   : undo\n";
    s += "u   : Uff optization\n";
    s += "g   : Ghemical  optization";

    QMessageBox::information(0, "快捷方式", s);
    return;
}

void MainWindow::displayMessage(QString s) { ShowLabel.setText(s); }

void MainWindow::on_lineEdit_XX_textEdited(const QString &arg1) {
    ui.lineEdit_YY->clear();
    ui.lineEdit_ZZ->clear();
}

void MainWindow::on_lineEdit_YY_textEdited(const QString &arg1) {
    ui.lineEdit_XX->clear();
    ui.lineEdit_ZZ->clear();
}

void MainWindow::on_lineEdit_ZZ_textEdited(const QString &arg1) {
    ui.lineEdit_YY->clear();
    ui.lineEdit_XX->clear();
}

void MainWindow::on_lineEdit_X_textEdited(const QString &arg1) {
    ui.lineEdit_Y->clear();
    ui.lineEdit_Z->clear();
}

void MainWindow::on_lineEdit_Y_textEdited(const QString &arg1) {
    ui.lineEdit_X->clear();
    ui.lineEdit_Z->clear();
}

void MainWindow::on_lineEdit_Z_textEdited(const QString &arg1) {
    ui.lineEdit_Y->clear();
    ui.lineEdit_X->clear();
}




