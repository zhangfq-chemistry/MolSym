#include "aboutDialog.h"


AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent)
{
   ui.setupUi(this);

   //ui.pictureWidget->sizePolicy().setHeightForWidth(true);
   //ui.pictureWidget->load(QString(":/resources/images/splash.jpg"));
  // ui->textBrowser->setHtml(LoadTextFromFile(":/resources/credits.htm"));
  // if (!IvRestoreWindowSize("AboutDialog/Size", this))
   //   IvGuessSubDialogSize(this);
}

AboutDialog::~AboutDialog()
{
   //delete ui;
}
