#include "showTextForm.h"
#include <QApplication>
#include <QClipboard>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QProcess>
#include <QTextStream>
#include <cstddef>
#include <cstdlib>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <QRegularExpression>
#include <fstream>

#include "mainwindow.h"

ShowTextForm::ShowTextForm(QString title, QWidget *parent) : QDialog(parent) {
  ui.setupUi(this);

  if (title.size() < 1)
    setWindowTitle("XTB method");
  else
    setWindowTitle(title);

  ui.textEdit->setLineWrapMode(QTextEdit::NoWrap);
  ui.textEdit->setCurrentFont(QFont("Courier", 16));
  ui.textEdit->setTextBackgroundColor(Qt::black);

  ui.textEdit->setStyleSheet("QTextEdit {background-color: #000; color: #ccc; "
                             "font-family: monospace;}");

  ui.spinBox_charge->setRange(-30, 30);
  ui.spinBox_numberElectrons->setValue(0);

  isXTBCal = false;
  runShellProcess = nullptr;
}

ShowTextForm::~ShowTextForm() {
  if (runShellProcess)
    delete runShellProcess;
}

void ShowTextForm::setParent(MainWindow *p) {
  m_parent = p;
  mol = m_parent->getMol();
  view3d = m_parent->getView3D();
  initial();
}

void ShowTextForm::initial() {
  if (isXTBCal)
    initialXTB();
}

void ShowTextForm::on_pushButton_Close_clicked() { close(); }

void ShowTextForm::appendText(QString s) {
  s.replace("\n", "<br>");
  ui.textEdit->moveCursor(QTextCursor::End);
  ui.textEdit->textCursor().insertHtml("<pre>" + s + "</pre>");
  ui.textEdit->moveCursor(QTextCursor::End);
}

void ShowTextForm::clear() { ui.textEdit->clear(); }

void ShowTextForm::setText(QString s) {
  QString x(s);
  x.replace("\n", "<br>");
  ui.textEdit->setText(x);
}

void ShowTextForm::on_pushButton_run_clicked() {
  if (isXTBCal)
    runXTB();
}

void ShowTextForm::initialXTB() {
  QString fileName = "";
  setText("  Perform XTB calculation:\n\n");
  executePath = m_parent->getExecutePath();

  unsigned int numAtoms = mol->NumAtoms();

#ifdef WIN32
  executePath = executePath + "\\xtb-win";
  executePath.replace('\/', '\\');

  /*
      QString all=getenv("Path");
      cout <<all.toStdString()<<endl;
      if(!all.contains(executePath))
      {
          QString appendXTBPath="set Path=%Path%;"+executePath;
          cout << appendXTBPath.toStdString()<<endl;
          system(appendXTBPath.toStdString().c_str());
          //all=getenv("Path");
          //cout <<all.toStdString()<<endl;
      }
  */
  fileName = executePath + "\\test.xyz";
  commandXTB = executePath + "\\xtb.exe " + fileName;
  execute_parameters = " --opt ";
#else
  executePath = executePath + "/tmp";
  appendText("Using " + executePath + " as scratch directory\n");
  QDir dir(executePath);

  if (dir.exists())
    dir.removeRecursively();
  mkdir(executePath.toStdString().c_str(), 0755);

  commandXTB = "xtb " + fileName + " --opt   "; //+ executePath+"/test.log " ;
  fileName = executePath + "/test.xyz";
#endif

  // cout << commandXTB.toStdString()<<endl;
  // cout << fileName.toStdString()<<endl;
  ui.lineEdit_parameters->setText(execute_parameters);
  ui.lineEdit_path->setText(executePath);

  // prepare xtb xyz
  QFile file1(fileName);
  if (!file1.open(QIODevice::ReadWrite | QIODevice::Text))
    return;

  QString str2 = QString("%1\n").arg(numAtoms, 3);
  file1.write(str2.toUtf8());
  file1.write("run xtb by zhangfq\n");

  double x, y, z;
  appendText("       Coordinates of molecule:\n");
  appendText("----------------------------------------\n");
  for (unsigned int i = 0; i < numAtoms; i++) {
    QString asymbol = mol->getAtomSymbol(i);

    if (asymbol == "X" || asymbol == "Bq")
      continue;

    x = mol->getAtomXbyIndex(i);
    y = mol->getAtomYbyIndex(i);
    z = mol->getAtomZbyIndex(i);

    str2 = QString("%1 %2 %3 %4\n")
               .arg(asymbol, 3)
               .arg(x, 10, 'f', 5)
               .arg(y, 10, 'f', 5)
               .arg(z, 10, 'f', 5);
    appendText(str2);
    file1.write(str2.toUtf8());
  }
  file1.write("\n\n");
  file1.close();

  appendText("----------------------------------------\n");
  appendText("\n");
}

void ShowTextForm::runXTB() {
  if (runShellProcess == nullptr)
    runShellProcess = new QProcess(this);

  if (runShellProcess) {
    appendText("New process ok!\n");
    execute_parameters = ui.lineEdit_parameters->text();
    QString all = commandXTB + execute_parameters;
    // cout << commandXTB.toStdString()<<endl;
    // cout << execute_parameters.toStdString()<<endl;
    // cout << all.toStdString()<<endl;
    appendText(all);
    appendText("\n");
  }

#ifdef WIN32
  QStringList _files;
  QString _filesNames = "xtb.xyz xtb.log wbo xtbrestart xtbopo.mol";
  _files = _filesNames.split(QRegExp("\\s+"));
  for (uint i = 0; i < 5; i++) {
    QString _filename = m_parent->getExecutePath() + "/" + _files[i];
    _filename.replace("\/", "\\");

    QFile file(_filename);
    if (file.exists())
      remove(_filename.toStdString().c_str());
  }
#else
  QFile file(m_parent->getExecutePath() + "/xtbopt.xyz");
  //_filename=m_parent->getExecutePath()+"/xtbopt.xyz";
  // cout << "try to open "<< _filename.toStdString() <<endl;
  // if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
  //    QMessageBox::information(0, "Warning","Cannot Open File:"+_filename);
  //   return;
  //}
#endif

  QString all = commandXTB + execute_parameters;
  runShellProcess->start(all);

  log = "";
  connect(runShellProcess, SIGNAL(readyRead()), this, SLOT(readProcess()));
  connect(runShellProcess, SIGNAL(finished(int)), this,
          SLOT(finishedProcess()));
}

void ShowTextForm::readProcess() {
  // log = QString::fromLocal8Bit(runShellProcess->readAll());
  log = QString::fromUtf8(runShellProcess->readAll());
  appendText(log);
}

void ShowTextForm::finishedProcess() {
  if (runShellProcess) {
    disconnect(runShellProcess, SIGNAL(readyRead()), this, SLOT(readProcess()));
    disconnect(runShellProcess, SIGNAL(finished(int)), this,
               SLOT(finishedProcess()));
  }

  QString ss = ui.textEdit->toPlainText();
  if (ss.contains("no basis found for atom")) {
    QMessageBox::information(0, "Error", "Cannot optimize based on XTB!");
    return;
  }

  QString _result = "";
  QString _filename = "";

#ifdef WIN32
  QFile file(m_parent->getExecutePath() + "\\xtbopt.xyz");
  _filename = m_parent->getExecutePath() + "\\xtbopt.xyz";
  _filename.replace("\/", "\\");

  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    _result = in.readAll();
    file.close();
    appendText(_result);
    mol->loadxyzfromQString(_result);
  } else {
    QFile file1(m_parent->getExecutePath() + "\\xtbtopo.mol");
    QString _filename_mol = m_parent->getExecutePath() + "\\xtbtopo.mol";
    _filename_mol.replace("\/", "\\");

    if (!file1.open(QIODevice::ReadOnly | QIODevice::Text)) {
      QMessageBox::information(0, "Warning",
                               "Cannot Open File: " + _filename_mol);
      return;
    }

    QTextStream in(&file1);
    _result = in.readAll();
    file1.close();
    appendText(_result);
    mol->loadxyzfromQString(_result);
    cout << _result.toStdString() << endl;
  }

#else
  QFile file(m_parent->getExecutePath() + "/xtbopt.xyz");
  //_filename=m_parent->getExecutePath()+"/xtbopt.xyz";
  // cout << "try to open "<< _filename.toStdString() <<endl;
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::information(0, "Warning", "Cannot Open File:" + _filename);
    return;
  }
  mol->loadxyzfromQString(_result);
#endif

  // mol->centralize();
  view3d->renderMol(mol);

  // clean xtb files
  QStringList _files;
  QString _filesNames = "xtbopt.xyz xtbopt.log wbo xtbrestart xtbtopo.mol";
  _files = _filesNames.split(QRegularExpression("\\s+"));
  QFile file_XTB;

  for (uint i = 0; i < 5; i++) {
    _filename = m_parent->getExecutePath() + "/" + _files[i];
#ifdef WIN32
    _filename.replace("\/", "\\");
#endif

    file_XTB.setFileName(_filename.toStdString().c_str());
    if (file_XTB.exists())
      remove(_filename.toStdString().c_str());
  }
}

void ShowTextForm::on_spinBox_charge_valueChanged(const QString &arg1) {
  execute_parameters = ui.lineEdit_parameters->text();

  int charge = ui.spinBox_charge->value();

  if ((!execute_parameters.contains("-c")) &&
      (!execute_parameters.contains("--chrg"))) {
    QString str = QString(" %1  %2 ").arg(" -c ").arg(charge);
    execute_parameters = str + execute_parameters;

    ui.lineEdit_parameters->setText(execute_parameters);
    return;
  }

  // refresh it
  if (execute_parameters.contains("-c")) {
    QString str = execute_parameters;
    QStringList ll = str.split(QRegularExpression("\\s+"));

    str = "";
    for (size_t i = 0; i < ll.size(); i++) {
      if (ll[i].contains("-c")) {
        if (charge != 0)
          str = str + QString(" %1  %2 ").arg(" -c ").arg(charge);
        i = i + 1;
        continue;
      }
      str = str + " " + ll[i];
    }

    ui.lineEdit_parameters->setText(str);
  }

  if (execute_parameters.contains("--chrg")) {
    QString str = execute_parameters;
    QStringList ll = str.split(QRegularExpression("\\s+"));

    str = "";
    for (size_t i = 0; i < ll.size(); i++) {
      if (ll[i].contains("--chrg")) {
        if (charge != 0)
          str = str + QString(" %1  %2 ").arg(" --chrg ").arg(charge);
        i = i + 1;
        continue;
      }
      str = str + " " + ll[i];
    }

    ui.lineEdit_parameters->setText(str);
  }
}

void ShowTextForm::on_spinBox_numberElectrons_valueChanged(
    const QString &arg1) {
  int nAlphaBeta = ui.spinBox_numberElectrons->value();

  execute_parameters = ui.lineEdit_parameters->text();

  if ((!execute_parameters.contains("-u")) &&
      (!execute_parameters.contains("--uhf"))) {
    QString str = QString(" %1  %2 ").arg(" -u ").arg(nAlphaBeta);
    execute_parameters = str + execute_parameters;
    ui.lineEdit_parameters->setText(execute_parameters);
    return;
  }

  // refresh it
  if (execute_parameters.contains("-u")) {
    QString str = execute_parameters;
    QStringList ll = str.split(QRegularExpression("\\s+"));

    str = "";
    for (size_t i = 0; i < ll.size(); i++) {
      if (ll[i].contains("-u")) {
        if (nAlphaBeta != 0)
          str = str + QString(" %1  %2 ").arg(" -u ").arg(nAlphaBeta);
        i = i + 1;
        continue;
      }
      str = str + " " + ll[i];
    }

    ui.lineEdit_parameters->setText(str);
  }

  if (execute_parameters.contains("--uhf")) {
    QString str = execute_parameters;
    QStringList ll = str.split(QRegularExpression("\\s+"));

    str = "";
    for (size_t i = 0; i < ll.size(); i++) {
      if (ll[i].contains("--uhf")) {
        if (nAlphaBeta != 0)
          str = str + QString(" %1  %2 ").arg(" --uhf ").arg(nAlphaBeta);
        i = i + 1;
        continue;
      }
      str = str + " " + ll[i];
    }

    ui.lineEdit_parameters->setText(str);
  }
}
