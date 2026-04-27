#include "OperatorTree.h"
#include "View3D.h"
#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
// #include <QDesktopWidget>
#include <QMessageBox>

MainWindow *OperatorTree::getMainWindow() { return mainWin; }

OperatorTree::OperatorTree(QWidget *parent) : QTreeWidget(parent) {
  // setObjectName(QStringLiteral("treeWidget"))
  // setGeometry(QRect(10, 1, 191, 691));

  QStringList labels;
  labels << tr("Setting") << tr("Type") << tr("Value");
  setHeaderLabels(labels);
  header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  header()->setSectionResizeMode(2, QHeaderView::Stretch);
  header()->setVisible(false);

  // refreshTimer.setInterval(2000);

  groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                      QIcon::Normal, QIcon::Off);
  groupIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                      QIcon::Normal, QIcon::On);
  keyIcon.addPixmap(style()->standardPixmap(QStyle::SP_FileIcon));

  connect(this, &QTreeWidget::itemClicked, this,
          &OperatorTree::updateSelection);

  setColumnCount(3);
  setColumnWidth(0, 100);
  setColumnWidth(1, 100);

  QString currentOperation = "";
}

OperatorTree::~OperatorTree() {
  clear();

  if (timer != nullptr)
    delete timer;
}

void OperatorTree::initSymmetry() {
  _isCnv = _isCn = _isCnh = false;
  _isDnd = _isDnh = _isDn = false;
  _isT = _isTd = _isTh = false;
  _isO = _isOh = false;
  _isI = _isIh = false;
  _isC00v = _isD00h = false;
  _isCs = _isCi = _isCs = _isSn = false;
  orderCn = 1;
}

void OperatorTree::removeAllItems() {
  // find the item of the Atom
  QTreeWidgetItemIterator itemFirstBranches(this);
  while (*itemFirstBranches) {
    if ((*itemFirstBranches)->text(1) == "root") {
      int size = (*itemFirstBranches)->childCount();
      while (size--)
        delete (*itemFirstBranches)->child(size);
    }
    ++itemFirstBranches;
  }

  //  delete itemObtial;

  //  if(itemObtial)
  //      itemObtial->setText(0,"None");
}

QSize OperatorTree::sizeHint() const {
  const QRect availableGeometry =
      QGuiApplication::primaryScreen()->availableGeometry();
  return QSize(availableGeometry.width() * 2 / 3,
               availableGeometry.height() * 2 / 3);
}

void OperatorTree::setAutoRefresh(bool autoRefresh) {
  this->autoRefresh = autoRefresh;
  if (!settings.isNull()) {
    if (autoRefresh) {
      maybeRefresh();
      refreshTimer.start();
    } else {
      refreshTimer.stop();
    }
  }
}

void OperatorTree::setFallbacksEnabled(bool enabled) {
  if (!settings.isNull()) {
    settings->setFallbacksEnabled(enabled);
    refresh();
  }
}

void OperatorTree::maybeRefresh() {
  if (state() != EditingState)
    refresh();
}

void OperatorTree::refresh() {
  if (settings.isNull())
    return;

  disconnect(this, &QTreeWidget::itemChanged, this,
             &OperatorTree::updateSetting);

  settings->sync();
  updateChildItems(0);

  connect(this, &QTreeWidget::itemChanged, this, &OperatorTree::updateSetting);
}

void OperatorTree::updateSetting(QTreeWidgetItem *item) {
  QString key = item->text(0);

  QTreeWidgetItem *ancestor = item->parent();
  while (ancestor) {
    key.prepend(ancestor->text(0) + QLatin1Char('/'));
    ancestor = ancestor->parent();
  }

  settings->setValue(key, item->data(2, Qt::UserRole));
  if (autoRefresh)
    refresh();
}

void OperatorTree::updateChildItems(QTreeWidgetItem *parent) {
  int dividerIndex = 0;

  foreach (QString group, settings->childGroups()) {
    QTreeWidgetItem *child;
    int childIndex = findChild(parent, group, dividerIndex);
    if (childIndex != -1) {
      child = childAt(parent, childIndex);
      child->setText(1, QString());
      child->setText(2, QString());
      child->setData(2, Qt::UserRole, QVariant());
      moveItemForward(parent, childIndex, dividerIndex);
    } else {
      child = createItem(group, parent, dividerIndex);
    }
    child->setIcon(0, groupIcon);
    ++dividerIndex;

    settings->beginGroup(group);
    updateChildItems(child);
    settings->endGroup();
  }

  foreach (const QString &key, settings->childKeys()) {
    QTreeWidgetItem *child;
    int childIndex = findChild(parent, key, 0);

    if (childIndex == -1 || childIndex >= dividerIndex) {
      if (childIndex != -1) {
        child = childAt(parent, childIndex);
        for (int i = 0; i < child->childCount(); ++i)
          delete childAt(child, i);
        moveItemForward(parent, childIndex, dividerIndex);
      } else {
        child = createItem(key, parent, dividerIndex);
      }
      child->setIcon(0, keyIcon);
      ++dividerIndex;
    } else {
      child = childAt(parent, childIndex);
    }

    QVariant value = settings->value(key);
    if (value.type() == QVariant::Invalid) {
      child->setText(1, "Invalid");
    } else {
      child->setText(1, value.typeName());
    }
    // child->setText(2, VariantDelegate::displayText(value));
    child->setData(2, Qt::UserRole, value);
  }

  while (dividerIndex < childCount(parent))
    delete childAt(parent, dividerIndex);
}

void OperatorTree::moveItemForward(QTreeWidgetItem *parent, int oldIndex,
                                   int newIndex) {
  for (int i = 0; i < oldIndex - newIndex; ++i)
    delete childAt(parent, newIndex);
}

QTreeWidgetItem *OperatorTree::childAt(QTreeWidgetItem *parent, int index) {
  if (parent)
    return parent->child(index);
  else
    return topLevelItem(index);
}

int OperatorTree::childCount(QTreeWidgetItem *parent) {
  if (parent)
    return parent->childCount();
  else
    return topLevelItemCount();
}

int OperatorTree::findChild(QTreeWidgetItem *parent, const QString &text,
                            int startIndex) {
  for (int i = startIndex; i < childCount(parent); ++i) {
    if (childAt(parent, i)->text(0) == text)
      return i;
  }
  return -1;
}

QTreeWidgetItem *OperatorTree::createItem(const QString &text,
                                          QTreeWidgetItem *parent, int index) {
  QTreeWidgetItem *after = 0;
  if (index != 0)
    after = childAt(parent, index - 1);

  QTreeWidgetItem *item;
  if (parent)
    item = new QTreeWidgetItem(parent, after);
  else
    item = new QTreeWidgetItem(this, after);

  item->setText(0, text);
  // item->setFlags(item->flags() | Qt::ItemIsEditable);
  return item;
}

void OperatorTree::setSymmetry(QString name) {
  nameSymmetry = name;
  QString PG = nameSymmetry.toUpper();
  QString temp = nameSymmetry;

  removeAllItems();

  if ((!itemSymmetry) || (itemSymmetry == nullptr))
    itemSymmetry = new QTreeWidgetItem(this, QStringList(name));
  itemSymmetry->setBackground(0, QBrush(QColor("#e5ebf4")));
  itemSymmetry->setText(0, name);
  itemSymmetry->setText(1, QString("root"));
  itemSymmetry->setForeground(0, QBrush(QColor(Qt::blue)));

  initSymmetry();

  if (PG == "T") {
    _isT = true;
    addViewsItems_T(itemSymmetry);
    return;
  } else if (PG == "TD") {
    _isTd = true;
    addViewsItems_Td(itemSymmetry);
    return;
  } else if (PG == "TH") {
    _isTh = true;
    addViewsItems_Th(itemSymmetry);
    return;
  }

  else if (PG == "O") {
    _isO = true;
    addViewsItems_O(itemSymmetry);
    return;
  } else if (PG == "OH") {
    _isOh = true;
    addViewsItems_Oh(itemSymmetry);
    return;
  }

  else if (PG == "I") {
    _isI = true;
    addViewsItems_I(itemSymmetry);
    return;
  } else if (PG == "IH") {
    _isIh = true;
    addViewsItems_Ih(itemSymmetry);
    return;
  }

  if (PG.startsWith("S")) {
    _isSn = true;
    addViewsItems_Sn(itemSymmetry, nameSymmetry);
    return;
  }

  // Cnv  Cnh  Cn  CS  CI C00V
  if (PG.startsWith("C")) {
    QString Cn = nameSymmetry;
    temp.remove(0, 1);

    if (PG == "C1") {
      _isC1 = true;
      _isCn = true;
      addViewsItems_Cn(itemSymmetry, nameSymmetry);
      return;
    }

    else if (PG == "CS") {
      _isCs = true;
      addViewsItems_Cs(itemSymmetry);
      return;
    }

    else if (PG == "CI") {
      _isCi = true;
      addViewsItems_Ci(itemSymmetry);
      return;
    }

    else if (PG == "C**V" || PG == "COOV" || PG == "C00V") {
      _isC00v = true;

      auto item = new QTreeWidgetItem(itemSymmetry);
      item->setBackground(0, QBrush(QColor("#e5ebf4")));
      item->setText(0, "C00");
      itemSymmetry->addChild(item);

      auto item1 = new QTreeWidgetItem(itemSymmetry);
      item1->setBackground(0, QBrush(QColor("#e5ebf4")));
      item1->setText(0, "Sigma V");
      itemSymmetry->addChild(item1);

      view3d->initSigmaV_list(1);
      return;
    }

    //------------------------------------
    else if (PG.endsWith("H")) //! Cnh
    {
      _isCnh = true;

      addViewsItems_Cnh(itemSymmetry, nameSymmetry);
      return;
    }

    if (PG.endsWith("V")) //! Cnv
    {
      _isCnv = true;
      addViewsItems_Cnv(itemSymmetry, nameSymmetry);

      return;
    }
    //-----------------------------------
    else //! Cn
    {
      _isCn = true;
      addViewsItems_Cn(itemSymmetry, nameSymmetry);
      return;
    }
  }

  // Dn  Dnh
  if (PG.at(0) == 'D') {
    if (PG == "D**H" || PG == "DOOH" || PG == "D00H") {
      addViewsItems_D00h(itemSymmetry);
      return;
    }

    else if (PG.endsWith("H")) // Dnh
    {
      _isDnh = true;

      addViewsItems_Dnh(itemSymmetry, nameSymmetry);
      return;
    } else if (PG.endsWith("D")) // Dnd
    {
      _isDnd = true;
      addViewsItems_Dnd(itemSymmetry, nameSymmetry);
      return;
    } else // Dn
    {
      _isDn = true;
      addViewsItems_Dn(itemSymmetry, nameSymmetry);
      return;
    }
  }
}

void OperatorTree::updateSelection(QTreeWidgetItem *item) {
  currentIdx = 0;
  currentOperation = "";
  currentItem = item;

  // the root
  if (!item->parent())
    return;

  if (item->childCount() > 0)
    return;

  uint _visible = 0;

  if (item->checkState(0) == Qt::Checked) {
    item->setCheckState(0, Qt::Unchecked);
    _visible = 0;
  } else {
    item->setCheckState(0, Qt::Checked);
    _visible = 1;
  }

  if (_isI) {
    updateI(item, _visible);
    return;
  }

  else if (_isIh) {
    updateIh(item, _visible);
    return;
  }

  else if (_isO) {
    updateO(item, _visible);
    return;
  }

  else if (_isOh) {
    updateOh(item, _visible);
    return;
  }

  else if (_isTd) {
    updateTd(item, _visible);
    return;
  }

  else if (_isT) {
    updateT(item, _visible);
    return;
  }

  else if (_isTh) {
    updateTh(item, _visible);
    return;
  }

  else if (_isCs) {
    updateCs(_visible);
    return;
  }

  else if (_isCi) {
    updateCi(_visible);
    return;
  }

  else if (_isSn) {
    updateSn(item, _visible);
    return;
  }

  else if (_isCn) {
    updateCn(item, _visible);
    return;
  }

  else if (_isCnv) {
    updateCnv(item, _visible);
    return;
  }

  else if (_isCnh) {
    updateCnh(item, _visible);
    return;
  }

  else if (_isDnh) {
    updateDnh(item, _visible);
    return;
  }

  else if (_isDn) {
    updateDn(item, _visible);
    return;
  }

  else if (_isDnd) {
    updateDnd(item, _visible);
    return;
  }

  if (_isC00v) {
    updateC00v(item, _visible);
    return;
  }

  if (_isD00h) {
    updateD00h(item, _visible);
    return;
  }
}

void OperatorTree::updateSn(QTreeWidgetItem *item, bool _visible) {
  currentItem = item;

  if (item->text(0).contains("S"))
    currentOperation = "Sn";
  else
    currentOperation = "In";

  if (item->text(0).contains("i"))
    currentOperation = "Center";

  if (_visible > 0) {
    mainWin->modifyCheckBox_CnHide(false);

    if (currentOperation == "In" || currentOperation == "Sn") {
      int count_i = item->parent()->childCount();
      for (int j = 0; j < count_i; j++) {
        if (currentItem->parent()->child(j)->text(0).contains("S"))
          currentItem->parent()->child(j)->setCheckState(0, Qt::Checked);

        if (currentItem->parent()->child(j)->text(0).contains("I"))
          currentItem->parent()->child(j)->setCheckState(0, Qt::Checked);
      }
    }
  } else {
    mainWin->modifyCheckBox_CnHide(true);

    if (currentOperation == "In" || currentOperation == "Sn") {
      int count_i = item->parent()->childCount();
      for (int j = 0; j < count_i; j++) {
        if (currentItem->parent()->child(j)->text(0).contains("S"))
          currentItem->parent()->child(j)->setCheckState(0, Qt::Unchecked);

        if (currentItem->parent()->child(j)->text(0).contains("I"))
          currentItem->parent()->child(j)->setCheckState(0, Qt::Unchecked);
      }
    }
  }
}

void OperatorTree::updateCnv(QTreeWidgetItem *item, bool _visible) {
  currentItem = item;

  // principal axis
  if (item->text(0).contains("C")) {
    currentOperation = "Cn";

    if (_visible > 0)
      mainWin->modifyCheckBox_CnHide(false);
    else
      mainWin->modifyCheckBox_CnHide(true);
    return;
  }

  // sigma v
  if (item->parent()->text(0).contains("Sigma V")) {
    currentOperation = "SignamV";

    QString txt = item->text(0);
    currentIdx = txt.toInt() - 1;
    check_All_SigmaV_state(_visible);

    view3d->refresh();
  }
}

void OperatorTree::updateDnh(QTreeWidgetItem *item, bool _visible) {
  // principal axis
  if (item->text(0).startsWith("C") || item->text(0).startsWith("I")) {
    if (_visible > 0)
      mainWin->modifyCheckBox_CnHide(false);
    else
      mainWin->modifyCheckBox_CnHide(true);

    currentOperation = "Cn";

    return;
  }

  // center
  if (item->text(0).startsWith("i")) {
    if (_visible > 0) {
      view3d->setCiVisible(true);
      setCenter_Visible();
    } else {
      view3d->setCiVisible(false);
    }

    view3d->refresh();

    currentOperation = "Center";

    return;
  }

  // Horizontal C2
  if (item->parent()->text(0).contains("Horizontal-C2")) {
    setC2_Visible();

    QString txt = item->text(0);
    uint idx = txt.toInt();

    view3d->setC2_Visible(idx - 1, _visible);

    currentIdx = idx - 1;
    check_All_C2_state(_visible);

    view3d->refresh();

    currentOperation = "HorizontalC2";
  }

  // sigma H
  if (item->text(0).startsWith("Sigma H")) {
    if (_visible > 0)
      mainWin->modifycheckBox_SigmaH_Hide(false);
    else
      mainWin->modifycheckBox_SigmaH_Hide(true);

    currentOperation = "SigmaH";
    return;
  }

  // sigma V
  if (item->parent()->text(0).contains("Sigma V")) {
    QString txt = item->text(0);

    currentIdx = txt.toInt() - 1;
    check_All_SigmaV_state(_visible);

    view3d->refresh();

    currentOperation = "SigmaV";
  }
}

void OperatorTree::updateCnh(QTreeWidgetItem *item, bool _visible) {
  // center
  if (item->text(0).startsWith("i")) {
    if (_visible > 0) {
      view3d->setCiVisible(true);
      setCenter_Visible();
    } else
      view3d->setCiVisible(false);

    view3d->refresh();

    currentOperation = "Center";
    return;
  }

  // principal axis
  if (item->text(0).startsWith("C")) {
    if (_visible > 0)
      mainWin->modifyCheckBox_CnHide(false);
    else
      mainWin->modifyCheckBox_CnHide(true);

    currentOperation = "Cn";
    return;
  }

  // sigma H
  if (item->text(0).startsWith("Sigma H")) {
    if (_visible > 0)
      mainWin->modifycheckBox_SigmaH_Hide(false);
    else
      mainWin->modifycheckBox_SigmaH_Hide(true);

    currentOperation = "SigmaH";
    return;
  }
}

void OperatorTree::updateCn(QTreeWidgetItem *item, bool _visible) {
  if (_visible > 0)
    mainWin->modifyCheckBox_CnHide(false);
  else
    mainWin->modifyCheckBox_CnHide(true);

  currentOperation = "Cn";
}

void OperatorTree::updateDn(QTreeWidgetItem *item, bool _visible) {
  // principal axis
  if (item->text(0).startsWith("C")) {
    if (_visible > 0)
      mainWin->modifyCheckBox_CnHide(false);
    else
      mainWin->modifyCheckBox_CnHide(true);

    currentOperation = "Cn";
    return;
  }

  // Horizontal C2
  if (item->parent()->text(0).contains("Horizontal-C2")) {
    uint idx = item->text(0).toInt();
    view3d->setC2_Visible(idx - 1, _visible);

    currentIdx = idx - 1;
    check_All_C2_state(_visible);

    view3d->refresh();

    currentOperation = "HorizontalC2";
  }
}

void OperatorTree::check_All_C2_state(bool _visible) {
  // update the check state
  if (_visible) {
    mainWin->setPGElement_Visible(true);
    if (orderCn % 2 > 0) // odd
    {
      mainWin->setC2_Visible(0);
      view3d->setC2_1_Visible(true);
    } else // even
    {
      if ((currentIdx + 1) % 2 > 0) { // odd
        view3d->setC2_1_Visible(true);
        mainWin->setC2_Visible(1);
      } else {
        view3d->setC2_2_Visible(true);
        mainWin->setC2_Visible(2);
      }
    }

    return;
  }

  int count_i = currentItem->parent()->childCount();

  if (orderCn % 2 > 0) // odd
  {
    uint t = 0;
    for (int j = 0; j < count_i; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }
    if (t < 1)
      mainWin->setC2_Invisible(0);

  } else { // even
    uint t1 = 0, t2 = 0;

    for (int j = 0; j < count_i; j = j + 2)
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t1++;
    for (int j = 1; j < count_i; j = j + 2)
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t2++;

    if (t1 < 1)
      mainWin->setC2_Invisible(1);

    if (t2 < 1)
      mainWin->setC2_Invisible(2);
  }
}

void OperatorTree::check_All_C3_state(bool _visible) {
  if (_visible) {
    mainWin->setPGElement_Visible(1);
    view3d->setSymmetryElementVisible(true);
    mainWin->setC3_Visible();
    view3d->setC3_Visible(true);
  }

  view3d->setC3_Visible(currentIdx, _visible);

  if (_visible)
    return;

  int count_i = currentItem->parent()->childCount();

  uint t = 0;
  for (int j = 0; j < count_i; j++) {
    if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
      t++;
  }

  if (t < 1)
    mainWin->setAllC3_Visible(false);
}

void OperatorTree::check_All_SigmaV_state(bool _visible) {
  if (_visible)
    mainWin->setPGElement_Visible(true);

  uint idx = currentIdx + 1;

  if (_isCnv) {
    if (_visible)
      view3d->setSigmaVVisible(true);

    if (orderCn % 2 == 1)
      view3d->setSigmaV_Visible(idx - 1, _visible);
    else {
      if (idx % 2 == 1) {
        if (_visible)
          mainWin->setSigmaV_Visible(1);
        view3d->setSigmaV_Visible((idx + 1) / 2 - 1, _visible);
        // currentIdx=(idx+1)/2-1;
      } else {

        mainWin->setSigmaV_Visible(2);
        view3d->setSigmaD_Visible(idx / 2 - 1, _visible);
        // currentIdx=idx/2-1;

        if (_visible)
          view3d->setSigmaDVisible(true);
      }
    }

    int count_i = currentItem->parent()->childCount();

    if (orderCn % 2 > 0) // odd
    {
      uint t = 0;
      for (int j = 0; j < count_i; j++) {
        if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
          t++;
      }

      if (t < 1)
        mainWin->setSigmaV_Invisible(0);
      else
        mainWin->setSigmaV_Visible(0);
    } else { // even
      uint t1 = 0, t2 = 0;

      for (int j = 0; j < count_i; j = j + 2)
        if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
          t1++;

      for (int j = 1; j < count_i; j = j + 2)
        if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
          t2++;

      if (t1 < 1)
        mainWin->setSigmaV_Invisible(1);
      else
        mainWin->setSigmaV_Visible(1);

      if (t2 < 1)
        mainWin->setSigmaV_Invisible(2);
      else
        mainWin->setSigmaV_Visible(2);
    }

    return;
  }

  if (_isDnh) {
    if (orderCn % 2 > 0) // odd
    {
      view3d->setSigmaV_Visible(currentIdx, _visible);

      // view3d->displayList_sigmaV_visible();

      if (_visible)
        mainWin->setSigmaD_Visible(1);
    } else {
      if (idx % 2 == 1)
        view3d->setSigmaV_Visible((idx + 1) / 2 - 1, _visible);
      else {
        mainWin->setSigmaV_Visible(2);
        view3d->setSigmaD_Visible(idx / 2 - 1, _visible);
      }
    }

    int count_i = currentItem->parent()->childCount();

    if (orderCn % 2 > 0) // odd
    {
      uint t = 0;
      for (int j = 0; j < count_i; j++) {
        if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
          t++;
      }

      if (t < 1)
        mainWin->setSigmaD_Invisible(1);
      else
        mainWin->setSigmaD_Visible(1);
    }

    else { // even
      uint t1 = 0, t2 = 0;
      for (int j = 0; j < count_i; j = j + 2)
        if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
          t1++;

      for (int j = 1; j < count_i; j = j + 2)
        if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
          t2++;

      if (t1 < 1)
        mainWin->setSigmaV_Invisible(1);
      else
        mainWin->setSigmaV_Visible(1);

      if (t2 < 1)
        mainWin->setSigmaD_Invisible(1);
      else
        mainWin->setSigmaD_Visible(1);
    }
  }
}

void OperatorTree::check_All_SigmaD_state(bool _visible) {
  uint idx = currentIdx + 1;

  if (_visible)
    mainWin->setPGElement_Visible(true);

  if (_isTd) {
    uint t = 0;
    for (int j = 2; j < 4; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }

    if (t < 1)
      mainWin->setSigmaD_Invisible(0);
    else
      mainWin->setSigmaD_Visible(0);

    return;
  }

  if (orderCn % 2 == 1) {
    view3d->setSigmaV_Visible(currentIdx, _visible);
  } else {
    if (idx % 2 == 1) {
      currentIdx = (idx + 1) / 2 - 1;
      view3d->setSigmaV_Visible(currentIdx, _visible);
    } else {
      currentIdx = idx / 2 - 1;
      view3d->setSigmaD_Visible(currentIdx, _visible);
    }
  }

  // check all
  int count_i = currentItem->parent()->childCount();
  if (orderCn % 2 > 0) // odd
  {
    uint t = 0;
    for (int j = 0; j < count_i; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }

    if (t < 1)
      mainWin->setSigmaV_Invisible(0);

    if (isDnd() && t < 1)
      mainWin->setSigmaD_Invisible(1);

  } else { // even
    uint t1 = 0, t2 = 0;

    for (int j = 0; j < count_i; j = j + 2)
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t1++;

    for (int j = 1; j < count_i; j = j + 2)
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t2++;

    if (t1 < 1)
      mainWin->setSigmaV_Invisible(1);
    else
      mainWin->setSigmaV_Visible(1);

    if (t2 < 1)
      mainWin->setSigmaD_Invisible(1);
    else
      mainWin->setSigmaD_Visible(1);
  }
}

void OperatorTree::check_All_SigmaH_state(bool _visible) {
  /*
  if(_isTd)
  {
      uint t=0;
      for(int j=4;j<6;j++){
          if(currentItem->parent()->child(j)->checkState(0)==Qt::Checked)
              t++;
      }

      if(t<1)
          mainWin->setSigmaH_Invisible(0);
      else
          mainWin->setSigmaH_Visible(0);

      return;
  }
  */
}

void OperatorTree::updateDnd(QTreeWidgetItem *item, bool _visible) {
  // principal axis
  if (item->text(0).startsWith("C")) {
    if (_visible > 0)
      mainWin->modifyCheckBox_CnHide(false);
    else
      mainWin->modifyCheckBox_CnHide(true);

    currentOperation = "Cn";
    return;
  }

  if (item->text(0).startsWith("S")) {
    currentOperation = "S2n";

    if (_visible > 0)
      mainWin->modifyCheckBox_CnHide(false);
    else
      mainWin->modifyCheckBox_CnHide(true);

    return;
  }

  // center
  if (item->text(0).startsWith("i")) {
    if (_visible > 0) {
      view3d->setCiVisible(true);
      setCenter_Visible();
    } else
      view3d->setCiVisible(false);

    view3d->refresh();

    currentOperation = "Center";
    return;
  }

  // sigma D
  if (item->parent()->text(0).contains("Sigma D")) {
    currentOperation = "SigmaD";

    QString txt = item->text(0);
    // uint idx=txt.toInt();

    currentIdx = txt.toInt() - 1;
    check_All_SigmaD_state(_visible);

    /*
            if(orderCn%2==1)
            {
                setSigmaD_Visible();
                view3d->setSigmaV_Visible(idx-1,_visible);
            }
            else
            {
                if (idx%2==1)
                {
                    setSigmaV_Visible();
                    view3d->setSigmaV_Visible((idx+1)/2-1,_visible);
                }
                else
                {
                    setSigmaD_Visible();
                    view3d->setSigmaD_Visible(idx/2-1,_visible);
                }
            }
    */
    view3d->refresh();
    return;
  }

  // Horizontal C2
  if (item->parent()->text(0).contains("Horizontal-C2")) {
    setC2_Visible();

    QString txt = item->text(0);
    uint idx = txt.toInt();

    view3d->setC2_Visible(idx - 1, _visible);

    currentIdx = idx - 1;
    check_All_C2_state(_visible);

    view3d->refresh();
    currentOperation = "HorizontalC2";
  }
}

void OperatorTree::updateCs(bool _visible) {
  if (_visible > 0) {
    mainWin->modifycheckBox_SigmaV_Hide(false);
    view3d->setAllSigmaV_Visible(true);
  } else {
    mainWin->modifycheckBox_SigmaV_Hide(true);
    view3d->setAllSigmaV_Visible(false);
  }

  currentOperation = "SigmaV";
}

void OperatorTree::updateCi(bool _visible) {
  if (_visible > 0) {
    view3d->setCiVisible(true);
    setCenter_Visible();
  } else
    view3d->setCiVisible(false);

  view3d->refresh();

  currentOperation = "Reverse";
}

void OperatorTree::updateC00v(QTreeWidgetItem *item, bool _visible) {
  if (item->text(0).startsWith("C00")) {
    if (_visible > 0) {
      mainWin->modifyCheckBox_CnHide(false);
      currentOperation = "Cn";
    } else
      mainWin->modifyCheckBox_CnHide(true);
    return;
  }

  if (item->text(0).startsWith("Sigma")) {
    if (_visible > 0) {
      mainWin->modifycheckBox_SigmaV_Hide(false);
      view3d->setAllSigmaV_Visible(true);
    } else {
      mainWin->modifycheckBox_SigmaV_Hide(true);
      view3d->setAllSigmaV_Visible(false);
    }
  }
}

void OperatorTree::updateD00h(QTreeWidgetItem *item, bool _visible) {
  // principal axis
  if (item->text(0).startsWith("C00")) {
    currentOperation = "C00";
    if (_visible > 0)
      mainWin->modifyCheckBox_CnHide(false);
    else
      mainWin->modifyCheckBox_CnHide(true);
    return;
  }

  // center
  if (item->text(0).startsWith("i")) {
    currentOperation = "Center";
    if (_visible > 0) {
      view3d->setCiVisible(true);
      setCenter_Visible();
    } else
      view3d->setCiVisible(false);

    view3d->refresh();
    return;
  }

  // sigma H
  if (item->text(0).endsWith("H")) {
    currentOperation = "SigmaH";
    if (_visible > 0)
      mainWin->modifycheckBox_SigmaH_Hide(false);
    else
      mainWin->modifycheckBox_SigmaH_Hide(true);
    return;
  }

  if (item->text(0).endsWith("V")) {
    currentOperation = "SigmaV";

    if (_visible > 0) {
      mainWin->modifycheckBox_SigmaV_Hide(false);
      view3d->setAllSigmaV_Visible(true);
    } else {
      mainWin->modifycheckBox_SigmaV_Hide(true);
      view3d->setAllSigmaV_Visible(false);
    }
    return;
  }

  if (item->text(0).startsWith("C2")) {
    currentOperation = "C2";
    if (_visible > 0)
      view3d->setC2_Visible(false);
    else
      view3d->setC2_Visible(true);
    return;
  }
}

void OperatorTree::updateT(QTreeWidgetItem *item, bool _visible) {
  // principal axis
  if (item->parent()->text(0).startsWith("C2")) {
    currentOperation = "C2";

    QString txt = item->text(0);
    uint idx = txt.toInt();
    currentIdx = idx - 1;

    view3d->setS4_Visible(idx - 1, _visible);

    if (idx == 1) {
      if (_visible > 0)
        mainWin->modifyCheckBox_CnHide(false);
      else
        mainWin->modifyCheckBox_CnHide(true);
      return;
    } else {
      if (_visible) {
        mainWin->setC2_Visible(1);
        mainWin->setPGElement_Visible(1);
      }

      view3d->setC2_Visible(idx - 1, _visible);
    }

    // update panel
    int count_i = currentItem->parent()->childCount();

    uint t = 0;
    for (int j = 1; j < count_i; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }
    if (t < 1)
      mainWin->setC2_Invisible(0);
    else
      mainWin->setC2_Visible(0);

    view3d->refresh();
    return;
  }

  // C3
  if (item->parent()->text(0).contains("C3")) {
    currentOperation = "C3";

    QString txt = item->text(0);
    uint idx = txt.toInt();
    currentIdx = idx - 1;

    check_All_C3_state(_visible);

    view3d->refresh();
  }
}

void OperatorTree::updateTd(QTreeWidgetItem *item, bool _visible) {
  if (_visible)
    mainWin->setPGElement_Visible(true);

  // principal axis
  if (item->parent()->text(0).startsWith("S4")) {
    currentOperation = "S4";

    QString txt = item->text(0);
    uint idx = txt.toInt();

    view3d->setS4_Visible(idx - 1, _visible);

    if (idx == 1) {
      if (_visible > 0)
        mainWin->modifyCheckBox_CnHide(false);
      else
        mainWin->modifyCheckBox_CnHide(true);
      return;
    }

    int count_i = currentItem->parent()->childCount();

    uint t = 0;
    for (int j = 1; j < count_i; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }
    if (t < 1)
      mainWin->setC2_Invisible(0);
    else
      mainWin->setC2_Visible(0);

    view3d->refresh();
    return;
  }

  // sigma D
  if (item->parent()->text(0).contains("Sigma D")) {
    currentOperation = "SigmaD";

    QString txt = item->text(0);
    uint idx = txt.toInt();

    if (idx < 3) { // 1,2
      setSigmaV_Visible();
      view3d->setSigmaV_Visible(idx - 1, _visible);
    }

    if (idx > 2 && idx < 5) { // 3,4
      setSigmaD_Visible();
      view3d->setSigmaD_Visible(idx - 3, _visible);
    }

    if (idx > 4) { // 5,6
      setSigmaH_Visible();
      view3d->setSigmaH_Visible(idx - 5, _visible);
    }

    uint t = 0;
    for (uint j = 0; j < 2; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }
    if (t < 1)
      mainWin->setSigmaV_Invisible(1);
    else
      mainWin->setSigmaV_Visible(1);

    t = 0;
    for (uint j = 2; j < 4; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }
    if (t < 1)
      mainWin->setSigmaD_Invisible(1);
    else
      mainWin->setSigmaD_Visible(1);

    t = 0;
    for (uint j = 4; j < 6; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }
    if (t < 1)
      mainWin->setSigmaH_Invisible(1);
    else
      mainWin->setSigmaH_Visible(1);

    view3d->refresh();
    return;
  }

  // C2
  if (item->parent()->text(0).contains("C2")) {
    currentOperation = "C2";

    QString txt = item->text(0);
    uint idx = txt.toInt();
    currentIdx = idx - 1;

    view3d->setS4_Visible(idx - 1, _visible);

    if (idx == 1) {
      if (_visible > 0)
        mainWin->modifyCheckBox_CnHide(false);
      else
        mainWin->modifyCheckBox_CnHide(true);
      return;
    } else {
      if (_visible) {
        mainWin->setC2_Visible(1);
        mainWin->setPGElement_Visible(1);
      }

      view3d->setC2_Visible(idx - 1, _visible);
    }

    // update panel
    if (!_visible) {
      int count_i = currentItem->parent()->childCount();

      uint t = 0;
      for (int j = 1; j < count_i; j++) {
        if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
          t++;
      }
      if (t < 1)
        mainWin->setC2_Invisible(0);
    }

    view3d->refresh();
    return;
  }

  // C3
  if (item->parent()->text(0).contains("C3")) {
    currentOperation = "C3";
    QString txt = item->text(0);
    uint idx = txt.toInt();
    currentIdx = idx - 1;

    check_All_C3_state(_visible);

    view3d->refresh();
  }
}

void OperatorTree::updateTh(QTreeWidgetItem *item, bool _visible) {
  if (_visible)
    mainWin->setPGElement_Visible(true);

  // principal axis
  if (item->parent()->text(0).startsWith("C2")) {
    currentOperation = "C2";

    QString txt = item->text(0);
    uint idx = txt.toInt();
    currentIdx = idx - 1;

    if (idx == 1) {
      if (_visible > 0)
        mainWin->modifyCheckBox_CnHide(false);
      else
        mainWin->modifyCheckBox_CnHide(true);
      return;
    } else {
      if (_visible) {
        mainWin->setC2_Visible(1);
        mainWin->setPGElement_Visible(1);
      }

      view3d->setC2_Visible(idx - 1, _visible);
    }

    // update panel
    if (!_visible) {
      int count_i = currentItem->parent()->childCount();

      uint t = 0;
      for (int j = 1; j < count_i; j++) {
        if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
          t++;
      }
      if (t < 1)
        mainWin->setC2_Invisible(0);
    }

    view3d->refresh();
    return;
  }

  // C3
  if (item->parent()->text(0).contains("C3")) {
    currentOperation = "C3";

    QString txt = item->text(0);
    uint idx = txt.toInt();
    currentIdx = idx - 1;

    check_All_C3_state(_visible);

    view3d->refresh();
  }

  // center
  if (item->text(0).startsWith("i")) {
    currentOperation = "Center";
    if (_visible > 0) {
      view3d->setCiVisible(true);
      setCenter_Visible();
    } else
      view3d->setCiVisible(false);

    view3d->refresh();
    return;
  }

  // sigma H
  if (item->parent()->text(0).startsWith("Sigma H")) {
    currentOperation = "SigmaH";

    QString txt = item->text(0);
    uint idx = txt.toInt();

    if (idx == 3) {
      if (_visible > 0)
        mainWin->modifycheckBox_SigmaV_Hide(false);
      else
        mainWin->modifycheckBox_SigmaV_Hide(true);
    }

    if (idx == 2) {
      if (_visible > 0)
        mainWin->modifycheckBox_SigmaD_Hide(false);
      else
        mainWin->modifycheckBox_SigmaD_Hide(true);
    }

    if (idx == 1) {
      if (_visible > 0)
        mainWin->modifycheckBox_SigmaH_Hide(false);
      else
        mainWin->modifycheckBox_SigmaH_Hide(true);
    }

    if (_visible > 0) {
      mainWin->setPGElement_Visible(true);
      // view3d->set
    }

    view3d->refresh();
    return;
  }
}

void OperatorTree::updateO(QTreeWidgetItem *item, bool _visible) {
  if (_visible)
    mainWin->setPGElement_Visible(true);

  // principal axis
  if (item->parent()->text(0).startsWith("C4")) {
    currentOperation = "C4";
    QString txt = item->text(0);
    uint idx = txt.toInt();

    view3d->setC4_Visible(idx - 1, _visible);

    uint t = 0;
    for (uint j = 0; j < 3; j++) {
      if (item->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }

    if (t > 0) {
      mainWin->modifyCheckBox_CnHide(false, true);
      view3d->setCnVisible(true);
      mainWin->setPGElement_Visible();
    } else {
      mainWin->modifyCheckBox_CnHide(true, true);
      view3d->setCnVisible(false);
    }

    view3d->refresh();
    return;
  }

  // C3
  if (item->parent()->text(0).contains("C3")) {
    setC3_Visible();

    currentOperation = "C3";
    QString txt = item->text(0);
    uint idx = txt.toInt();

    view3d->setC3_Visible(idx - 1, _visible);
    view3d->refresh();
    return;
  }

  // C2
  if (item->parent()->text(0).contains("C2")) {
    setC2_Visible();

    currentOperation = "C2";
    QString txt = item->text(0);
    uint idx = txt.toInt();

    view3d->setC2_Visible(idx - 1, _visible);
    view3d->refresh();
    return;
  }
}

void OperatorTree::updateOh(QTreeWidgetItem *item, bool _visible) {
  if (_visible)
    mainWin->setPGElement_Visible(true);

  currentItem = item;

  if (item->parent()->text(0).startsWith("C4") ||
      item->parent()->text(0).startsWith("C3") ||
      item->parent()->text(0).startsWith("C2")) {
    updateO(item, _visible);
    return;
  }

  // center
  if (item->text(0).startsWith("i")) {
    currentOperation = "Center";
    if (_visible > 0) {
      view3d->setCiVisible(true);
      setCenter_Visible();
    } else
      view3d->setCiVisible(false);

    view3d->refresh();
    return;
  }

  // sigma H
  if (item->parent()->text(0).startsWith("Sigma H")) {
    currentOperation = "SigmaH";
    QString txt = item->text(0);
    uint idx = txt.toInt();

    view3d->setSigmaH_Visible(idx - 1, _visible);

    uint t = 0;
    for (uint j = 0; j < 3; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }

    if (t < 1)
      mainWin->setSigmaH_Invisible(1);
    else
      mainWin->setSigmaH_Visible(1);

    view3d->refresh();
    return;
  }

  // sigma D
  if (item->parent()->text(0).contains("Sigma D")) {
    currentOperation = "SigmaD";
    QString txt = item->text(0);
    uint idx = txt.toInt();

    view3d->setSigmaD_Visible(idx - 1, _visible);

    // 0,1
    uint t = 0;
    for (uint j = 0; j < 2; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }
    if (t < 1)
      mainWin->setSigmaV_Invisible(1);
    else
      mainWin->setSigmaV_Visible(1);

    // 2,3,4,5
    t = 0;
    for (uint j = 2; j < 6; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }
    if (t < 1)
      mainWin->setSigmaD_Invisible(1);
    else
      mainWin->setSigmaD_Visible(1);

    view3d->refresh();
    return;
  }
}

void OperatorTree::updateI(QTreeWidgetItem *item, bool _visible) {
  if (_visible)
    mainWin->setPGElement_Visible(true);

  if (item->parent()->text(0).startsWith("C5")) {
    currentOperation = "C5";
    QString txt = item->text(0);
    currentIdx = txt.toInt();

    view3d->setC5_Visible(currentIdx - 1, _visible);

    uint t = 0;
    for (uint j = 0; j < 6; j++) {
      if (item->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }

    if (t) {
      view3d->setSymmetryElementVisible(true);
      mainWin->modifyCheckBox_CnHide(false, true);
      view3d->setC5_Visible(true);
      view3d->setCnVisible(true);
    } else
      mainWin->modifyCheckBox_CnHide(true, true);

    view3d->refresh();
    return;
  }

  // C3
  if (item->parent()->text(0).startsWith("C3")) {
    setC3_Visible();

    currentOperation = "C3";
    QString txt = item->text(0);
    currentIdx = txt.toInt();

    view3d->setC3_Visible(currentIdx - 1, _visible);

    uint t = 0;
    for (uint j = 0; j < 10; j++) {
      if (item->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }

    if (t) {
      view3d->setSymmetryElementVisible(true);
      view3d->setC3_Visible(true);
      // mainWin->setC3_Visible(0);
    } else
      mainWin->setAllC3_Visible(false);

    view3d->refresh();
    return;
  }

  // C2
  if (item->parent()->text(0).startsWith("C2")) {
    setC2_Visible();
    currentOperation = "C2";
    QString txt = item->text(0);
    currentIdx = txt.toInt();

    view3d->setC2_Visible(currentIdx - 1, _visible);
    uint t = 0;
    for (uint j = 0; j < 15; j++) {
      if (item->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }
    if (t > 1) {
      view3d->setSymmetryElementVisible(true);
      view3d->setC2_Visible(true);
      mainWin->setC2_Visible(0);
    } else {
      mainWin->setC2_Invisible(0);
    }

    view3d->refresh();
    return;
  }
}

void OperatorTree::updateIh(QTreeWidgetItem *item, bool _visible) {
  if (_visible)
    mainWin->setPGElement_Visible(true);

  if (item->parent()->text(0).startsWith("C5") ||
      item->parent()->text(0).contains("C3") ||
      item->parent()->text(0).contains("C2")) {
    updateI(item, _visible);
    view3d->refresh();
    return;
  }

  // center
  if (item->text(0).startsWith("i")) {
    currentOperation = "Center";

    if (_visible > 0) {
      view3d->setCiVisible(true);
      setCenter_Visible();
    } else
      view3d->setCiVisible(false);

    view3d->refresh();
    return;
  }

  // sigma D
  if (item->parent()->text(0).contains("Sigma D-1")) {
    currentOperation = "SigmaD1";
    QString txt = item->text(0);
    uint idx = txt.toInt();

    // setSigmaV_Visible();
    view3d->setSigmaV_Visible(idx - 1, _visible);

    uint t = 0;
    for (uint j = 0; j < 5; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }
    if (t < 1)
      mainWin->setSigmaV_Invisible(1);
    else
      mainWin->setSigmaV_Visible(1);

    view3d->refresh();
    return;
  }

  if (item->parent()->text(0).contains("Sigma D-2")) {
    currentOperation = "SigmaD2";
    QString txt = item->text(0);
    uint idx = txt.toInt();

    // setSigmaD_Visible();
    view3d->setSigmaD_Visible(idx - 1, _visible);

    uint t = 0;
    for (uint j = 0; j < 3; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }
    if (t < 1)
      mainWin->setSigmaD_Invisible(1);
    else
      mainWin->setSigmaD_Visible(1);

    view3d->refresh();
    return;
  }

  if (item->parent()->text(0).contains("Sigma D-3")) {
    currentOperation = "SigmaD3";
    QString txt = item->text(0);
    uint idx = txt.toInt();

    // setSigmaH_Visible();
    view3d->setSigmaH_Visible(idx - 1, _visible);

    uint t = 0;
    for (uint j = 0; j < 2; j++) {
      if (currentItem->parent()->child(j)->checkState(0) == Qt::Checked)
        t++;
    }
    if (t < 1)
      mainWin->setSigmaH_Invisible(1);
    else
      mainWin->setSigmaH_Visible(1);

    view3d->refresh();
    return;
  }
}

void OperatorTree::addViewsItems_Sn(QTreeWidgetItem *itemRoot,
                                    QString nameofSymmetry) {
  auto item = new QTreeWidgetItem(itemSymmetry);
  item->setBackground(0, QBrush(QColor("#e5ebf4")));
  item->setText(0, nameofSymmetry);
  itemRoot->addChild(item);

  nameofSymmetry.remove(0, 1);
  orderCn = nameofSymmetry.toInt();

  if (orderCn % 4 < 1) {
    auto item1 = new QTreeWidgetItem(itemSymmetry);
    item1->setBackground(0, QBrush(QColor("#e5ebf4")));
    QString In = "I" + QString::number(orderCn);
    itemRoot->addChild(item1);
    item1->setText(0, In);
    itemRoot->addChild(item1);
  } else {
    auto item_i = new QTreeWidgetItem(itemRoot);
    item_i->setBackground(0, QBrush(QColor("#e5ebf4")));
    item_i->setText(0, "i");
    itemRoot->addChild(item_i);
  }
}

void OperatorTree::addViewsItems_Cn(QTreeWidgetItem *itemRoot,
                                    QString nameofSymmetry) {
  // cout << "OperatorTree::addViewsItems_Cn(QTreeWidgetItem  " <<endl;
  QString Cn = nameofSymmetry;
  QString temp = Cn;

  temp.remove(0, 1);
  orderCn = temp.toInt();

  auto item0 = new QTreeWidgetItem(itemRoot);
  // item0->setFlags(item0->flags() | Qt::ItemIsEditable);
  item0->setBackground(0, QBrush(QColor("#e5ebf4")));
  item0->setText(0, Cn);
  itemRoot->addChild(item0);
}

void OperatorTree::addViewsItems_Cnv(QTreeWidgetItem *itemRoot,
                                     QString nameofSymmetry) {
  QString Cn = nameofSymmetry;
  QString temp = Cn;

  temp.remove(0, 1);
  temp.chop(1);

  orderCn = temp.toInt();

  Cn.chop(1);
  auto item0 = new QTreeWidgetItem(itemRoot);
  // item0->setFlags(item0->flags() | Qt::ItemIsEditable);
  item0->setBackground(0, QBrush(QColor("#e5ebf4")));
  item0->setText(0, Cn);
  item0->setData(0, Qt::CheckStateRole, QVariant());
  itemRoot->addChild(item0);

  auto item = new QTreeWidgetItem(itemRoot);
  // item->setFlags(item->flags() | Qt::ItemIsEditable);
  item->setBackground(0, QBrush(QColor("#e5ebf4")));
  item->setText(0, "Sigma V");
  item->setData(0, Qt::CheckStateRole, QVariant());
  itemRoot->addChild(item);

  addViewsItems(item, "Sigma V", orderCn);

  // odd
  if (orderCn % 2 > 0) {
    view3d->initSigmaV_list(orderCn);
  } else {
    view3d->initSigmaV_list(orderCn / 2);
    view3d->initSigmaD_list(orderCn / 2);
  }
}

void OperatorTree::addViewsItems_Cnh(QTreeWidgetItem *itemRoot,
                                     QString nameofSymmetry) {
  QString Cn = nameofSymmetry;
  QString temp = Cn;

  temp.remove(0, 1);
  temp.chop(1);
  orderCn = temp.toInt();

  Cn.chop(1);
  if (orderCn % 2 == 1)
    Cn += "(I" + QString::number(orderCn * 2) + ")";
  else {
    auto item_i = new QTreeWidgetItem(itemRoot);
    item_i->setBackground(0, QBrush(QColor("#e5ebf4")));
    item_i->setText(0, "i");
    itemRoot->addChild(item_i);
  }

  auto item0 = new QTreeWidgetItem(itemRoot);

  item0->setBackground(0, QBrush(QColor("#e5ebf4")));
  item0->setText(0, Cn);
  item0->setData(0, Qt::CheckStateRole, QVariant());
  itemRoot->addChild(item0);

  auto item = new QTreeWidgetItem(itemRoot);
  // item->setFlags(item->flags() | Qt::ItemIsEditable);
  item->setBackground(0, QBrush(QColor("#e5ebf4")));
  item->setText(0, "Sigma H");
  item->setData(0, Qt::CheckStateRole, QVariant());
  itemRoot->addChild(item);
}

void OperatorTree::addViewsItems_Dn(QTreeWidgetItem *itemRoot,
                                    QString nameofSymmetry) {
  QString temp = nameofSymmetry;
  temp.remove(0, 1);
  orderCn = temp.toInt();

  // principal axis
  QString Cn = nameofSymmetry;
  Cn.replace(0, 1, "C");

  auto item_Cn = new QTreeWidgetItem(itemRoot);
  // item_Cn->setFlags(item_Cn->flags() | Qt::ItemIsEditable);
  item_Cn->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_Cn->setText(0, Cn);
  itemRoot->addChild(item_Cn);

  auto item = new QTreeWidgetItem(itemRoot);
  // item->setFlags(item->flags() | Qt::ItemIsEditable);
  item->setBackground(0, QBrush(QColor("#e5ebf4")));
  item->setText(0, "Horizontal-C2");
  item->setData(0, Qt::CheckStateRole, QVariant());
  itemRoot->addChild(item);
  addViewsItems(item, "Horinzontal-C2", orderCn);

  view3d->initC2_Horizontal_list(orderCn);

  cout << " addViewsItems_Dnh " << endl;
}

void OperatorTree::addViewsItems_D00h(QTreeWidgetItem *itemRoot) {
  _isD00h = true;

  // center
  auto item_i = new QTreeWidgetItem(itemRoot);
  item_i->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_i->setText(0, "i");
  itemRoot->addChild(item_i);

  auto item_Cn = new QTreeWidgetItem(itemRoot);
  item_Cn->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_Cn->setText(0, "C00");
  itemRoot->addChild(item_Cn);

  auto item_sigmaH = new QTreeWidgetItem(itemRoot);
  item_sigmaH->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_sigmaH->setText(0, "Sigma H");
  itemRoot->addChild(item_sigmaH);

  auto item_sigmaV = new QTreeWidgetItem(itemRoot);
  item_sigmaV->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_sigmaV->setText(0, "Sigma V");
  itemRoot->addChild(item_sigmaV);

  view3d->initSigmaV_list(1);
}

void OperatorTree::addViewsItems_Dnh(QTreeWidgetItem *itemRoot,
                                     QString nameofSymmetry) {
  QString temp = nameofSymmetry;
  temp.remove(0, 1);
  temp.chop(1);
  orderCn = temp.toInt();

  // center
  if (orderCn % 2 == 0) {
    auto item_i = new QTreeWidgetItem(itemRoot);
    // item_i->setFlags(item_i->flags() | Qt::ItemIsEditable);
    item_i->setBackground(0, QBrush(QColor("#e5ebf4")));
    item_i->setText(0, "i");
    // item_i->setData(0, Qt::CheckStateRole, QVariant());
    itemRoot->addChild(item_i);

    // principal axis
    QString Cn = nameofSymmetry;
    Cn.replace(0, 1, "C");
    Cn.chop(1);

    auto item0 = new QTreeWidgetItem(itemRoot);
    // item0->setFlags(item0->flags() | Qt::ItemIsEditable);
    item0->setBackground(0, QBrush(QColor("#e5ebf4")));
    item0->setText(0, Cn);
    // item0->setData(0, Qt::CheckStateRole, QVariant());
    itemRoot->addChild(item0);
  } else {
    // principal axis
    QString Cn = nameofSymmetry;
    Cn.replace(0, 1, "C");
    Cn.chop(1);

    QString In = Cn + "(I" + QString::number(orderCn * 2) + ")";

    auto item0 = new QTreeWidgetItem(itemRoot);
    // item0->setFlags(item0->flags() | Qt::ItemIsEditable);
    item0->setBackground(0, QBrush(QColor("#e5ebf4")));
    item0->setText(0, In);
    // item0->setData(0, Qt::CheckStateRole, QVariant());
    itemRoot->addChild(item0);
  }

  // horizontal C2
  auto item = new QTreeWidgetItem(itemRoot);
  // item->setFlags(item->flags() | Qt::ItemIsEditable);
  item->setBackground(0, QBrush(QColor("#e5ebf4")));
  item->setText(0, "Horizontal-C2");
  item->setData(0, Qt::CheckStateRole, QVariant());
  itemRoot->addChild(item);
  addViewsItems(item, "Horinzontal-C2", orderCn);

  // sigma v
  auto item_v = new QTreeWidgetItem(itemRoot);
  // item_v->setFlags(item_v->flags() | Qt::ItemIsEditable);
  item_v->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_v->setText(0, "Sigma V");
  item_v->setData(0, Qt::CheckStateRole, QVariant());
  itemRoot->addChild(item_v);
  addViewsItems(item_v, "Sigma V", orderCn);

  // sigma H
  auto item1 = new QTreeWidgetItem(itemRoot);
  // item1->setFlags(item1->flags() | Qt::ItemIsEditable);
  item1->setBackground(0, QBrush(QColor("#e5ebf4")));
  item1->setText(0, "Sigma H");
  // item1->setData(0, Qt::CheckStateRole, QVariant());
  itemRoot->addChild(item1);

  // odd
  if (orderCn % 2 > 0) {
    view3d->initSigmaV_list(orderCn);
  }
  // even
  else {
    view3d->initSigmaV_list(orderCn / 2);
    view3d->initSigmaD_list(orderCn / 2);
  }

  view3d->initC2_Horizontal_list(orderCn);
}

void OperatorTree::addViewsItems_Dnd(QTreeWidgetItem *itemRoot,
                                     QString nameofSymmetry) {
  QString Cn = nameofSymmetry;
  QString temp = Cn;

  temp.remove(0, 1);
  temp.chop(1);
  orderCn = temp.toInt();

  // odd
  if (orderCn % 2 > 0) {
    auto item_i = new QTreeWidgetItem(itemRoot);
    // item_i->setFlags(item_i->flags() | Qt::ItemIsEditable);
    item_i->setBackground(0, QBrush(QColor("#e5ebf4")));
    item_i->setText(0, "i");
    // item_i->setData(0, Qt::CheckStateRole, QVariant());
    itemRoot->addChild(item_i);
  }

  // principal axis
  Cn.replace(0, 1, "C");
  Cn.chop(1);
  Cn += "(S" + QString::number(orderCn * 2) + ")";

  auto item0 = new QTreeWidgetItem(itemRoot);
  // item0->setFlags(item0->flags() | Qt::ItemIsEditable);
  item0->setBackground(0, QBrush(QColor("#e5ebf4")));
  item0->setText(0, Cn);
  itemRoot->addChild(item0);

  QString S2n = "S" + QString::number(orderCn * 2);
  auto item_S2n = new QTreeWidgetItem(itemRoot);
  // item0->setFlags(item0->flags() | Qt::ItemIsEditable);
  item_S2n->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_S2n->setText(0, S2n);
  itemRoot->addChild(item_S2n);

  auto item = new QTreeWidgetItem(itemRoot);
  // item->setFlags(item->flags() | Qt::ItemIsEditable);
  item->setBackground(0, QBrush(QColor("#e5ebf4")));
  item->setText(0, "Horizontal-C2");
  item->setData(0, Qt::CheckStateRole, QVariant());
  itemRoot->addChild(item);
  addViewsItems(item, "Horinzontal-C2", orderCn);

  // sigma H
  auto item1 = new QTreeWidgetItem(itemRoot);
  // item1->setFlags(item1->flags() | Qt::ItemIsEditable);
  item1->setBackground(0, QBrush(QColor("#e5ebf4")));
  item1->setText(0, "Sigma D");
  item1->setData(0, Qt::CheckStateRole, QVariant());
  itemRoot->addChild(item1);
  addViewsItems(item1, "Sigma D", orderCn);

  // odd
  if (orderCn % 2 > 0) {
    view3d->initSigmaV_list(orderCn);
  }
  // even
  else {
    view3d->initSigmaV_list(orderCn / 2);
    view3d->initSigmaD_list(orderCn / 2);
  }

  view3d->initC2_Horizontal_list(orderCn);
}

void OperatorTree::addViewsItems_Cs(QTreeWidgetItem *itemRoot) {
  auto item = new QTreeWidgetItem(itemRoot);
  item->setBackground(0, QBrush(QColor("#e5ebf4")));
  item->setText(0, "Sigma V");
  itemRoot->addChild(item);

  view3d->initSigmaV_list(1);
}

void OperatorTree::addViewsItems_Ci(QTreeWidgetItem *itemRoot) {
  auto item = new QTreeWidgetItem(itemRoot);
  item->setBackground(0, QBrush(QColor("#e5ebf4")));
  item->setText(0, "i");
  itemRoot->addChild(item);
}

void OperatorTree::addViewsItems_T(QTreeWidgetItem *itemRoot) {
  auto item_Cn = new QTreeWidgetItem(itemRoot);
  item_Cn->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_Cn->setText(0, "C2");
  itemRoot->addChild(item_Cn);
  addViewsItems(item_Cn, "C2", 3);

  // 4C3
  auto item_C3 = new QTreeWidgetItem(itemRoot);
  item_C3->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_C3->setText(0, "C3");
  itemRoot->addChild(item_C3);
  addViewsItems(item_C3, "C3", 4);

  view3d->initC3_list(4);
  view3d->initS4_list(6);
}

void OperatorTree::addViewsItems_Td(QTreeWidgetItem *itemRoot) {
  // S4/I4(principal axis)
  QString I4 = "S4(I4,C2)";

  auto item_Cn = new QTreeWidgetItem(itemRoot);
  item_Cn->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_Cn->setText(0, I4);
  itemRoot->addChild(item_Cn);
  addViewsItems(item_Cn, "S4", 3);

  // 4C3
  auto item_C3 = new QTreeWidgetItem(itemRoot);
  item_C3->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_C3->setText(0, "C3");
  itemRoot->addChild(item_C3);
  addViewsItems(item_C3, "C3", 4);

  // 6 SigmaD
  auto item_SigmaD = new QTreeWidgetItem(itemRoot);
  item_SigmaD->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_SigmaD->setText(0, "Sigma D");
  itemRoot->addChild(item_SigmaD);
  addViewsItems(item_SigmaD, "SigmaD", 6);

  view3d->initS4_list(3);
  view3d->initC3_list(4);
  view3d->initC2_list(6);

  view3d->initSigmaV_list(2);
  view3d->initSigmaD_list(2);
  view3d->initSigmaH_list(2);
}

void OperatorTree::addViewsItems_Th(QTreeWidgetItem *itemRoot) {
  // center
  auto item_i = new QTreeWidgetItem(itemRoot);
  item_i->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_i->setText(0, "i");
  itemRoot->addChild(item_i);

  // 3C2
  QString C2 = "C2";
  auto item_Cn = new QTreeWidgetItem(itemRoot);
  item_Cn->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_Cn->setText(0, C2);
  itemRoot->addChild(item_Cn);
  addViewsItems(item_Cn, "C2", 3);

  // 4C3
  auto item_C3 = new QTreeWidgetItem(itemRoot);
  item_C3->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_C3->setText(0, "C3");
  itemRoot->addChild(item_C3);
  addViewsItems(item_C3, "C3", 4);

  // 3SigmaH
  auto item_sigmaH = new QTreeWidgetItem(itemRoot);
  item_sigmaH->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_sigmaH->setText(0, "Sigma H");
  itemRoot->addChild(item_sigmaH);
  addViewsItems(item_sigmaH, "sigmaH", 3);

  view3d->initS4_list(3);
  view3d->initC3_list(4);

  view3d->initSigmaV_list(1);
  view3d->initSigmaD_list(1);
  view3d->initSigmaH_list(1);
}

void OperatorTree::addViewsItems_O(QTreeWidgetItem *itemRoot) {
  // 3C4
  auto item_Cn = new QTreeWidgetItem(itemRoot);
  item_Cn->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_Cn->setText(0, "C4");
  itemRoot->addChild(item_Cn);
  addViewsItems(item_Cn, "C4", 3);

  // 4C3
  auto item_C3 = new QTreeWidgetItem(itemRoot);
  item_C3->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_C3->setText(0, "C3");
  itemRoot->addChild(item_C3);
  addViewsItems(item_C3, "C3", 4);

  // 6 C2
  auto item_C2 = new QTreeWidgetItem(itemRoot);
  item_C2->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_C2->setText(0, "C2");
  itemRoot->addChild(item_C2);
  addViewsItems(item_C2, "C2", 6);

  view3d->initC4_list(3);
  view3d->initC3_list(4);
  view3d->initC2_list(6);
}

void OperatorTree::addViewsItems_Oh(QTreeWidgetItem *itemRoot) {
  // center
  auto item_i = new QTreeWidgetItem(itemRoot);
  item_i->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_i->setText(0, "i");
  itemRoot->addChild(item_i);

  // 3C4
  auto item_Cn = new QTreeWidgetItem(itemRoot);
  item_Cn->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_Cn->setText(0, "C4");
  itemRoot->addChild(item_Cn);
  addViewsItems(item_Cn, "C4", 3);

  // 4C3
  auto item_C3 = new QTreeWidgetItem(itemRoot);
  item_C3->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_C3->setText(0, "C3");
  itemRoot->addChild(item_C3);
  addViewsItems(item_C3, "C3", 4);

  // 6 C2
  auto item_C2 = new QTreeWidgetItem(itemRoot);
  item_C2->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_C2->setText(0, "C2");
  itemRoot->addChild(item_C2);
  addViewsItems(item_C2, "C2", 6);

  // 3SigmaH
  auto item_sigmaH = new QTreeWidgetItem(itemRoot);
  item_sigmaH->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_sigmaH->setText(0, "Sigma H");
  itemRoot->addChild(item_sigmaH);
  addViewsItems(item_sigmaH, "sigmaH", 3);

  // 6 SigmaD
  auto item_SigmaD = new QTreeWidgetItem(itemRoot);
  item_SigmaD->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_SigmaD->setText(0, "Sigma D");
  itemRoot->addChild(item_SigmaD);
  addViewsItems(item_SigmaD, "SigmaD", 6);

  view3d->initC4_list(3);
  view3d->initC3_list(4);
  view3d->initC2_list(6);

  view3d->initSigmaV_list(6);
  view3d->initSigmaD_list(6);
  view3d->initSigmaH_list(3);
}

void OperatorTree::addViewsItems_I(QTreeWidgetItem *itemRoot) {
  // 6C5
  auto item_Cn = new QTreeWidgetItem(itemRoot);
  item_Cn->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_Cn->setText(0, "C5");
  itemRoot->addChild(item_Cn);
  addViewsItems(item_Cn, "C5", 6);

  // 10C3
  auto item_C3 = new QTreeWidgetItem(itemRoot);
  item_C3->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_C3->setText(0, "C3");
  itemRoot->addChild(item_C3);
  addViewsItems(item_C3, "C3", 10);

  // 15 C2
  auto item_C2 = new QTreeWidgetItem(itemRoot);
  item_C2->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_C2->setText(0, "C2");
  itemRoot->addChild(item_C2);
  addViewsItems(item_C2, "C2", 15);

  view3d->initC5_list(6);
  view3d->initC3_list(10);
  view3d->initC2_list(15);
}

void OperatorTree::addViewsItems_Ih(QTreeWidgetItem *itemRoot) {
  // center
  auto item_i = new QTreeWidgetItem(itemRoot);
  item_i->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_i->setText(0, "i");
  itemRoot->addChild(item_i);

  // 6C5
  auto item_Cn = new QTreeWidgetItem(itemRoot);
  item_Cn->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_Cn->setText(0, "C5(S10)");
  itemRoot->addChild(item_Cn);
  addViewsItems(item_Cn, "C5", 6);

  // 10C3
  auto item_C3 = new QTreeWidgetItem(itemRoot);
  item_C3->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_C3->setText(0, "C3(S6)");
  itemRoot->addChild(item_C3);
  addViewsItems(item_C3, "C3", 10);

  // 15 C2
  auto item_C2 = new QTreeWidgetItem(itemRoot);
  item_C2->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_C2->setText(0, "C2");
  itemRoot->addChild(item_C2);
  addViewsItems(item_C2, "C2", 15);

  // 15 SigmaD
  auto item_SigmaD = new QTreeWidgetItem(itemRoot);
  item_SigmaD->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_SigmaD->setText(0, "Sigma D-1");
  itemRoot->addChild(item_SigmaD);
  addViewsItems(item_SigmaD, "SigmaD", 5);

  auto item_SigmaD1 = new QTreeWidgetItem(itemRoot);
  item_SigmaD1->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_SigmaD1->setText(0, "Sigma D-2");
  itemRoot->addChild(item_SigmaD1);
  addViewsItems(item_SigmaD1, "SigmaD", 3);

  auto item_SigmaD2 = new QTreeWidgetItem(itemRoot);
  item_SigmaD2->setBackground(0, QBrush(QColor("#e5ebf4")));
  item_SigmaD2->setText(0, "Sigma D-3");
  itemRoot->addChild(item_SigmaD2);
  addViewsItems(item_SigmaD2, "SigmaD", 2);

  view3d->initC5_list(6);
  view3d->initC3_list(10);
  view3d->initC2_list(15);

  view3d->initSigmaV_list(5);
  view3d->initSigmaD_list(3);
  view3d->initSigmaH_list(2);
}

void OperatorTree::addViewsItems(QTreeWidgetItem *itemRoot, QString ObName,
                                 int number) {
  for (int idx = 0; idx < number; idx++) {
    auto item = new QTreeWidgetItem(itemRoot, QStringList(ObName));
    // item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setBackground(0, QBrush(QColor("#e5ebf4")));

    itemRoot->addChild(item);

    item->setBackground(0, QBrush(QColor("#546892")));
    item->setCheckState(0, Qt::Checked);
    item->setBackground(0, QBrush(QColor("#e5ebf4")));

    // store the index of molecule and  this orbital
    item->setText(0, QString("%1").arg(idx + 1)); // store id
  }
}

void OperatorTree::set_Cn_Checked(bool t) {
  if (!itemSymmetry)
    return;
  if (itemSymmetry == nullptr)
    return;

  int counts = itemSymmetry->childCount();

  QTreeWidgetItem *child;

  for (int i = 0; i < counts; i++) {
    child = itemSymmetry->child(i);

    if (_isI || _isIh) {
      if (child->text(0).startsWith("C5")) {
        for (int j = 0; j < 6; j++) {
          if (t)
            child->child(j)->setCheckState(0, Qt::Checked);
          else
            child->child(j)->setCheckState(0, Qt::Unchecked);
        }
        break;
      }
    }

    if (_isOh || _isO) {
      if (child->text(0).startsWith("C4")) {
        for (int j = 0; j < 3; j++) {
          if (t)
            child->child(j)->setCheckState(0, Qt::Checked);
          else
            child->child(j)->setCheckState(0, Qt::Unchecked);
        }
        break;
      }
    }

    if (_isT || _isTh) {
      if (child->text(0).startsWith("C2")) {
        if (t)
          child->child(0)->setCheckState(0, Qt::Checked);
        else
          child->child(0)->setCheckState(0, Qt::Unchecked);
      }
      break;
    }

    if (_isTd) {
      if (child->text(0).startsWith("S4")) {
        if (t)
          child->child(0)->setCheckState(0, Qt::Checked);
        else
          child->child(0)->setCheckState(0, Qt::Unchecked);
      }
      break;
    }

    if (_isCn || _isCnv || _isCnh || _isDn || _isDnh) {
      if (child->text(0).startsWith("C")) {
        if (t)
          child->setCheckState(0, Qt::Checked);
        else
          child->setCheckState(0, Qt::Unchecked);
        break;
      }
    }

    if (_isDnd) {
      bool is = false;
      if (child->text(0).startsWith("C"))
        is = true;
      else if (child->text(0).startsWith("S"))
        if (!child->text(0).startsWith("Sigma"))
          is = true;

      if (is) {
        if (t)
          child->setCheckState(0, Qt::Checked);
        else
          child->setCheckState(0, Qt::Unchecked);
      }
    }

    if (_isSn) {
      if (child->text(0).startsWith("I") || child->text(0).startsWith("S")) {
        if (t)
          child->setCheckState(0, Qt::Checked);
        else
          child->setCheckState(0, Qt::Unchecked);
      }
    }

    /*
    int count_i=child->childCount();

    if(count_i<1)   {
        child->setData(0, Qt::CheckStateRole, QVariant());
        continue;
    }

    for(int j=0;j<count_i;j++)
    {
        if(t)
            child->child(j)->setCheckState(0,Qt::Checked);
        else
            child->child(j)->setCheckState(0,Qt::Unchecked);

        view3d->setC2_Visible(j,t);
    }
    */
  }
}

void OperatorTree::set_C2_Checked(uint type, bool t) {
  if (itemSymmetry == nullptr)
    return;

  int counts = itemSymmetry->childCount();

  QTreeWidgetItem *child;
  for (int i = 0; i < counts; i++) {
    child = itemSymmetry->child(i);

    if (_isTd) {
      if ((!child->text(0).startsWith("S4")))
        continue;
    } else {
      if ((!child->text(0).endsWith("C2")))
        continue;
    }

    int count_i = child->childCount();

    if (count_i < 1) {
      child->setData(0, Qt::CheckStateRole, QVariant());
      continue;
    }

    if (type < 1) // all
    {
      int j = 0; // principal axis

      if (_isT || _isTd || _isTh)
        j = 1;

      for (; j < count_i; j++) {
        if (t)
          child->child(j)->setCheckState(0, Qt::Checked);
        else
          child->child(j)->setCheckState(0, Qt::Unchecked);

        view3d->setC2_Visible(j, t);
      }

      return;
    }

    if (type == 1) // odd
    {
      for (int j = 0; j < count_i; j = j + 2) {
        if (t)
          child->child(j)->setCheckState(0, Qt::Checked);
        else
          child->child(j)->setCheckState(0, Qt::Unchecked);
        view3d->setC2_Visible(j, t);
      }
      return;
    }

    if (type == 2) // even
    {
      for (int j = 1; j < count_i; j = j + 2) {
        if (t)
          child->child(j)->setCheckState(0, Qt::Checked);
        else
          child->child(j)->setCheckState(0, Qt::Unchecked);
        view3d->setC2_Visible(j, t);
      }
      return;
    }

    break;
  }
}

void OperatorTree::set_C3_Checked(bool t) {
  if (itemSymmetry == nullptr)
    return;
  int counts = itemSymmetry->childCount();

  QTreeWidgetItem *child;
  for (int i = 0; i < counts; i++) {
    child = itemSymmetry->child(i);

    if (!child->text(0).endsWith("C3")) {
      if (!child->text(0).contains("(S6)"))
        continue;
    }

    int count_i = child->childCount();

    if (count_i < 1) {
      child->setData(0, Qt::CheckStateRole, QVariant());
      continue;
    }

    for (int j = 0; j < count_i; j++) {
      if (t) {
        child->child(j)->setCheckState(0, Qt::Checked);
        view3d->setC3_Visible(j, 1);
        continue;
      }
      child->child(j)->setCheckState(0, Qt::Unchecked);
      view3d->setC3_Visible(j, 0);
    }
  }
}

void OperatorTree::set_SigmaV_Checked(uint type, bool t) {
  if (itemSymmetry == nullptr)
    return;

  int counts = itemSymmetry->childCount();

  QTreeWidgetItem *child;
  for (int i = 0; i < counts; i++) {
    child = itemSymmetry->child(i);

    if (!child->text(0).startsWith("Sigma V"))
      continue;

    int count_i = child->childCount();

    if (count_i < 1) {
      child->setData(0, Qt::CheckStateRole, QVariant());
      continue;
    }

    if (type < 1) {
      for (int j = 0; j < count_i; j++) {
        if (t)
          child->child(j)->setCheckState(0, Qt::Checked);
        else
          child->child(j)->setCheckState(0, Qt::Unchecked);

        // view3d->setSigmaV_Visible(j,t);
      }

      return;
    }

    if (type == 1) // odd
    {
      if (orderCn % 2 > 0) // odd
      {
        for (int j = 0; j < count_i; j = j + 1) {
          if (t)
            child->child(j)->setCheckState(0, Qt::Checked);
          else
            child->child(j)->setCheckState(0, Qt::Unchecked);
        }

        // for(int j=0;j<count_i;j++)
        //    view3d->setSigmaV_Visible(j,t);
      }

      else {
        for (int j = 0; j < count_i; j = j + 2) {
          if (t)
            child->child(j)->setCheckState(0, Qt::Checked);
          else
            child->child(j)->setCheckState(0, Qt::Unchecked);
        }

        // for(int j=0;j<count_i;j++)
        //     view3d->setSigmaD_Visible(j,t);
      }
      return;
    }

    if (type == 2) // even
    {
      // for(int j=0;j<count_i;j++)
      //     view3d->setSigmaD_Visible(j,t);

      for (int j = 1; j < count_i; j = j + 2) {
        if (t)
          child->child(j)->setCheckState(0, Qt::Checked);
        else
          child->child(j)->setCheckState(0, Qt::Unchecked);
      }

      return;
    }
  }
}

void OperatorTree::set_SigmaD_Checked(uint type, bool t) {
  if (itemSymmetry == nullptr)
    return;

  int counts = itemSymmetry->childCount();

  QTreeWidgetItem *child;

  for (int i = 0; i < counts; i++) {
    child = itemSymmetry->child(i);

    if (!child->text(0).startsWith("Sigma D"))
      continue;

    int count_i = child->childCount();

    if (count_i < 1) {
      child->setData(0, Qt::CheckStateRole, QVariant());
      continue;
    }

    if (type < 1) {
      for (int j = 0; j < count_i; j++) {
        if (t)
          child->child(j)->setCheckState(0, Qt::Checked);
        else
          child->child(j)->setCheckState(0, Qt::Unchecked);

        // view3d->setSigmaV_Visible(j,t);
      }

      return;
    }

    if (type == 1) // odd
    {
      if (orderCn % 2 > 0) // odd
      {
        for (int j = 0; j < count_i; j = j + 1) {
          if (t)
            child->child(j)->setCheckState(0, Qt::Checked);
          else
            child->child(j)->setCheckState(0, Qt::Unchecked);
        }

        // for(int j=0;j<count_i;j++)
        //    view3d->setSigmaV_Visible(j,t);
      }

      else {
        for (int j = 0; j < count_i; j = j + 2) {
          if (t)
            child->child(j)->setCheckState(0, Qt::Checked);
          else
            child->child(j)->setCheckState(0, Qt::Unchecked);
        }

        // for(int j=0;j<count_i;j++)
        //     view3d->setSigmaD_Visible(j,t);
      }
      return;
    }

    if (type == 2) // even
    {
      // for(int j=0;j<count_i;j++)
      //     view3d->setSigmaD_Visible(j,t);

      for (int j = 1; j < count_i; j = j + 2) {
        if (t)
          child->child(j)->setCheckState(0, Qt::Checked);
        else
          child->child(j)->setCheckState(0, Qt::Unchecked);
      }

      return;
    }
  }
}

void OperatorTree::set_SigmaH_Checked_index(uint index, bool t) {
  if (itemSymmetry == nullptr)
    return;
  int counts = itemSymmetry->childCount();

  QTreeWidgetItem *child;

  for (int i = 0; i < counts; i++) {
    child = itemSymmetry->child(i);

    if (child->text(0).startsWith("Sigma H")) {
      if (t)
        child->child(index)->setCheckState(0, Qt::Checked);
      else
        child->child(index)->setCheckState(0, Qt::Unchecked);

      break;
    }
  }
}

void OperatorTree::set_SigmaD_Checked_index(uint index, bool t) {
  if (itemSymmetry == nullptr)
    return;
  int counts = itemSymmetry->childCount();

  QTreeWidgetItem *child;

  for (int i = 0; i < counts; i++) {
    child = itemSymmetry->child(i);

    if (child->text(0).startsWith("Sigma D")) {
      if (t)
        child->child(index)->setCheckState(0, Qt::Checked);
      else
        child->child(index)->setCheckState(0, Qt::Unchecked);

      break;
    }
  }
}

void OperatorTree::set_SigmaD1_Checked_index(uint index, bool t) {
  if (itemSymmetry == nullptr)
    return;
  int counts = itemSymmetry->childCount();

  QTreeWidgetItem *child;

  for (int i = 0; i < counts; i++) {
    child = itemSymmetry->child(i);

    if (child->text(0).startsWith("Sigma D") && child->text(0).endsWith("1")) {
      if (t)
        child->child(index)->setCheckState(0, Qt::Checked);
      else
        child->child(index)->setCheckState(0, Qt::Unchecked);

      break;
    }
  }
}

void OperatorTree::set_SigmaD2_Checked_index(uint index, bool t) {
  if (itemSymmetry == nullptr)
    return;
  int counts = itemSymmetry->childCount();

  QTreeWidgetItem *child;

  for (int i = 0; i < counts; i++) {
    child = itemSymmetry->child(i);

    if (child->text(0).startsWith("Sigma D") && child->text(0).endsWith("2")) {
      if (t)
        child->child(index)->setCheckState(0, Qt::Checked);
      else
        child->child(index)->setCheckState(0, Qt::Unchecked);

      break;
    }
  }
}

void OperatorTree::set_SigmaD3_Checked_index(uint index, bool t) {
  if (itemSymmetry == nullptr)
    return;
  int counts = itemSymmetry->childCount();

  QTreeWidgetItem *child;

  for (int i = 0; i < counts; i++) {
    child = itemSymmetry->child(i);

    if (child->text(0).startsWith("Sigma D") && child->text(0).endsWith("3")) {
      if (t)
        child->child(index)->setCheckState(0, Qt::Checked);
      else
        child->child(index)->setCheckState(0, Qt::Unchecked);

      break;
    }
  }
}

void OperatorTree::set_SigmaV_Checked_index(uint index, bool t) {
  if (itemSymmetry == nullptr)
    return;
  int counts = itemSymmetry->childCount();

  QTreeWidgetItem *child;

  for (int i = 0; i < counts; i++) {
    child = itemSymmetry->child(i);

    if (child->text(0).startsWith("Sigma V")) {
      if (t)
        child->child(index)->setCheckState(0, Qt::Checked);
      else
        child->child(index)->setCheckState(0, Qt::Unchecked);
      break;
    }
  }
}

void OperatorTree::setAllChecked() {
  if (itemSymmetry == nullptr)
    return;

  int counts = itemSymmetry->childCount();
  for (int i = 0; i < counts; i++) {
    QTreeWidgetItem *child = itemSymmetry->child(i);
    int count_i = child->childCount();

    if (count_i < 1) {
      child->setData(0, Qt::CheckStateRole, QVariant());
      continue;
    }

    for (int j = 0; j < count_i; j++)
      child->child(j)->setCheckState(0, Qt::Checked);
  }

  view3d->setVisibleAll_PG();
}

void OperatorTree::setAllUnchecked() {
  if (itemSymmetry == nullptr)
    return;

  int counts = itemSymmetry->childCount();
  for (int i = 0; i < counts; i++) {
    QTreeWidgetItem *child = itemSymmetry->child(i);
    // child->setCheckState(0,Qt::Unchecked);

    int count_i = child->childCount();
    if (count_i < 1) {
      child->setData(0, Qt::CheckStateRole, QVariant());
      continue;
    }

    for (int j = 0; j < count_i; j++) {
      child->child(j)->setCheckState(0, Qt::Unchecked);
    }
  }

  view3d->setInvisibleAll_PG();
}

void OperatorTree::startMovie() {
  if (currentItem->checkState(0) != Qt::Checked) {
    QMessageBox::information(0, "Warning",
                             "Please set it checked and then start movie!");
    return;
  }

  if (currentOperation.length() < 1)
    return;

  if (_isCs) {
    startMovie_Cs();
    return;
  }

  if (_isCi) {
    startMovie_Ci();
    return;
  }

  if (_isI) {
    startMovie_I();
    return;
  }

  else if (_isIh) {
    startMovie_Ih();
    return;
  }

  else if (_isO) {
    startMovie_O();
    return;
  }

  else if (_isOh) {
    startMovie_Oh();
    return;
  }

  else if (_isTd) {
    startMovie_Td();
    return;
  }

  else if (_isT) {
    startMovie_T();
    return;
  }

  else if (_isTh) {
    startMovie_Th();
    return;
  }

  else if (_isSn) {
    startMovie_Sn();
    return;
  }

  else if (_isCn) {
    startMovie_Cn();
    return;
  }

  else if (_isCnv) {
    startMovie_Cnv();
    return;
  }

  else if (_isCnh) {
    startMovie_Cnh();
    return;
  }

  else if (_isDnh) {
    startMovie_Dnh();
    return;
  }

  else if (_isDn) {
    startMovie_Dn();
    return;
  }

  else if (_isDnd) {
    startMovie_Dnd();
    return;
  }

  if (_isC00v) {
    startMovie_C00v();
  }

  if (_isD00h) {
    startMovie_D00h();
  }
}

void OperatorTree::startMovie_I() {
  uint idx = currentItem->text(0).toInt();

  // C5={0,0,1};C5={sin63.434948822922,0.0,cos63.434948822922}
  if (currentOperation.startsWith("C5")) {
    if (idx == 1) {
      startMovie_rotation(VZ, 5);
      return;
    }

    double angle = 63.434948822922 / 180.0 * 3.141592654;
    vector3 axis(sin(angle), 0.0, cos(angle));
    matrix3x3 m;
    if (idx == 2) {
      startMovie_rotation(axis, 5);
      return;
    }

    else if (idx == 3) {
      m.SetupRotateMatrix(VZ, 72.0);
      startMovie_rotation(m * axis, 5);
      return;
    }

    else if (idx == 4) {
      m.SetupRotateMatrix(VZ, 144.0);
      startMovie_rotation(m * axis, 5);
      return;
    }

    else if (idx == 5) {
      matrix3x3 m;
      m.SetupRotateMatrix(VZ, 216.0);
      startMovie_rotation(m * axis, 5);
      return;
    } else if (idx == 6) {
      matrix3x3 m;
      m.SetupRotateMatrix(VZ, -72.0);
      startMovie_rotation(m * axis, 5);
      return;
    }
    return;
  }

  // C3={cos36.0*sin37.377368142265787,sin36.0*sin37.377368142265787,cos37.377368142265787}
  if (currentOperation.startsWith("C3")) {
    double angle =
        37.377368142265787 / 180.0 * 3.141592654; // 0.6523581398977546
    // double ang=36.0/180.0*3.141592654; =0.62831853
    vector3 axis(cos(0.62831853) * sin(angle), sin(0.62831853) * sin(angle),
                 cos(angle));
    matrix3x3 m;

    if (idx == 1) {
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 2) {
      m.SetupRotateMatrix(VZ, 72.0);
      startMovie_rotation(m * axis, 3);
      return;
    }

    else if (idx == 3) {
      m.SetupRotateMatrix(VZ, 72.0 * 2.0);
      startMovie_rotation(m * axis, 3);
      return;
    }

    else if (idx == 4) {
      m.SetupRotateMatrix(VZ, 72.0 * 3.0);
      startMovie_rotation(m * axis, 3);
      return;
    }

    else if (idx == 5) {
      m.SetupRotateMatrix(VZ, 72.0 * 4.0);
      startMovie_rotation(m * axis, 3);
      return;
    }

    //---------------------------------
    if (idx == 6) {
      axis.Set(1.914927, 0.000000, -0.365718);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 7) {
      axis.Set(0.591745, 1.821204, -0.365718);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 8) {
      axis.Set(1.549208, -1.125566, 0.365718);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 9) {
      axis.Set(1.549208, 1.125566, 0.365718);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 10) {
      axis.Set(-0.591745, 1.821204, 0.365718);
      startMovie_rotation(axis, 3);
      return;
    }
  }

  if (currentOperation.startsWith("C2")) {
    //  C2={sin31.717474413001757,0.0,cos31.717474413001757}
    double angle = 31.717474413001757 / 180.0 * 3.141592654;
    vector3 axis(sin(angle), 0.0, cos(angle));
    matrix3x3 m;

    if (idx == 1) {
      startMovie_rotation(axis, 2);
      return;
    }

    else if (idx == 2) {
      m.SetupRotateMatrix(VZ, 72.0);
      startMovie_rotation(m * axis, 2);
      return;
    }

    else if (idx == 3) {
      m.SetupRotateMatrix(VZ, 72.0 * 2.0);
      startMovie_rotation(m * axis, 2);
      return;
    }

    else if (idx == 4) {
      m.SetupRotateMatrix(VZ, 72.0 * 3.0);
      startMovie_rotation(m * axis, 2);
      return;
    }

    else if (idx == 5) {
      m.SetupRotateMatrix(VZ, 72.0 * 4.0);
      startMovie_rotation(m * axis, 2);
      return;
    }

    //-------------------------------------
    if (idx == 6) {
      startMovie_rotation(VY, 2);
      return;
    }
    if (idx == 7) {
      m.SetupRotateMatrix(VZ, 36.0);
      startMovie_rotation(m * VY, 2);
      return;
    }
    if (idx == 8) {
      m.SetupRotateMatrix(VZ, 36.0 * 2);
      startMovie_rotation(m * VY, 2);
      return;
    }
    if (idx == 9) {
      m.SetupRotateMatrix(VZ, 36.0 * 3);
      startMovie_rotation(m * VY, 2);
      return;
    }
    if (idx == 10) {
      m.SetupRotateMatrix(VZ, 36.0 * 4);
      startMovie_rotation(m * VY, 2);
      return;
    }

    //-------------------------------------
    // angle=?????
    axis.Set(0.949931, 0.6901655, 0.725683);
    if (idx == 11) {
      startMovie_rotation(axis, 2);
      return;
    }

    else if (idx == 12) {
      m.SetupRotateMatrix(VZ, 72.0);
      startMovie_rotation(m * axis, 2);
      return;
    }

    else if (idx == 13) {
      m.SetupRotateMatrix(VZ, 72.0 * 2.0);
      startMovie_rotation(m * axis, 2);
      return;
    }

    else if (idx == 14) {
      m.SetupRotateMatrix(VZ, 72.0 * 3.0);
      startMovie_rotation(m * axis, 2);
      return;
    }

    else if (idx == 15) {
      m.SetupRotateMatrix(VZ, 72.0 * 4.0);
      startMovie_rotation(m * axis, 2);
      return;
    }
  }
}

void OperatorTree::startMovie_Ih() {
  if (currentOperation.startsWith("Center")) {
    startMovie_reverse();
    return;
  }

  if (currentOperation.startsWith("C5") || currentOperation.startsWith("C3") ||
      currentOperation.startsWith("C2")) {
    startMovie_I();
    return;
  }

  if (currentOperation.startsWith("SigmaD1")) {
    uint idx = currentItem->text(0).toInt();
    matrix3x3 m;

    if (idx == 1) {
      startMovie_reflection(VY);
      return;
    }

    else if (idx == 2) {
      m.SetupRotateMatrix(VZ, 36.0);
      startMovie_reflection(m * VY);
      return;
    }

    else if (idx == 3) {
      m.SetupRotateMatrix(VZ, 36.0 * 2);
      startMovie_reflection(m * VY);
      return;
    }

    else if (idx == 4) {
      m.SetupRotateMatrix(VZ, 36.0 * 3);
      startMovie_reflection(m * VY);
      return;
    }

    else if (idx == 5) {
      m.SetupRotateMatrix(VZ, 36.0 * 4);
      startMovie_reflection(m * VY);
      return;
    }
  }

  if (currentOperation.startsWith("SigmaD2")) {
    matrix3x3 m, rot;
    m.SetupRotateMatrix(VZ, 36.0);
    vector3 _normal = m * VY;

    vector3 C3(0.4911235, 0.35682208, 0.79465447);

    uint idx = currentItem->text(0).toInt();

    if (idx == 1) {
      startMovie_reflection(_normal);
      return;
    }

    else if (idx == 2) {
      rot.SetupRotateMatrix(C3, 60.0);
      startMovie_reflection(rot * _normal);
      return;
    }

    else if (idx == 3) {
      rot.SetupRotateMatrix(C3, 120.0);
      startMovie_reflection(rot * _normal);
      return;
    }
  }

  // double angle=31.717474413001757/180.0*3.141592654;
  //
  if (currentOperation.startsWith("SigmaD3")) {
    matrix3x3 rot;
    vector3 C2(0.5257311122034952, 0.0, 0.8506508082999016);
    rot.SetupRotateMatrix(C2, 90.0);

    uint idx = currentItem->text(0).toInt();

    if (idx == 1) {
      startMovie_reflection(VZ);
      return;
    }

    if (idx == 2) {
      startMovie_reflection(VY);
      return;
    }
  }
}

void OperatorTree::startMovie_O() {
  uint idx = currentItem->text(0).toInt();

  if (currentOperation.startsWith("C4")) {
    if (idx == 1) {
      startMovie_rotation(VZ, 4);
      return;
    }

    else if (idx == 2) {
      startMovie_rotation(VX, 4);
      return;
    }

    else if (idx == 3) {
      startMovie_rotation(VY, 4);
      return;
    }
    return;
  }

  if (currentOperation.startsWith("C3")) {
    vector3 axis(1, 1, 1);

    if (idx == 1) {
      axis.Set(1, 1, 1);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 2) {
      axis.Set(-1, -1, 1);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 3) {
      axis.Set(-1, 1, -1);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 4) {
      axis.Set(1, -1, -1);
      startMovie_rotation(axis, 3);
      return;
    }
  }

  if (currentOperation.startsWith("C2")) {
    vector3 axis(1, 1, 1);

    if (idx == 1) {
      axis.Set(0, 1, 1);
      startMovie_rotation(axis, 2);
      return;
    }

    else if (idx == 2) {
      axis.Set(-1, 0, 1);
      startMovie_rotation(axis, 2);
      return;
    }

    else if (idx == 3) {
      axis.Set(-1, -1, 0);
      startMovie_rotation(axis, 2);
      return;
    }

    else if (idx == 4) {
      axis.Set(1, 0, 1);
      startMovie_rotation(axis, 2);
      return;
    }

    else if (idx == 5) {
      axis.Set(-1, 1, 0);
      startMovie_rotation(axis, 2);
      return;
    }

    else if (idx == 6) {
      axis.Set(1, 1, 0);
      startMovie_rotation(axis, 2);
      return;
    }
  }
}

void OperatorTree::startMovie_Oh() {
  if (currentOperation.startsWith("Center")) {
    startMovie_reverse();
    return;
  }

  if (currentOperation.startsWith("C4") || currentOperation.startsWith("C3") ||
      currentOperation.startsWith("C2")) {
    startMovie_O();
    return;
  }

  if (currentOperation.startsWith("SigmaH")) {
    uint idx = currentItem->text(0).toInt();

    if (idx == 1) {
      startMovie_reflection(VZ);
      return;
    }

    else if (idx == 2) {
      startMovie_reflection(VY);
      return;
    }

    else if (idx == 3) {
      startMovie_reflection(VX);
      return;
    }
  }

  if (currentOperation.startsWith("SigmaD")) {
    uint idx = currentItem->text(0).toInt();

    if (idx == 1) {
      vector3 axis(1, 1, 0);
      startMovie_reflection(axis);
      return;
    }

    else if (idx == 2) {
      vector3 axis(-1, 1, 0);
      startMovie_reflection(axis);
      return;
    }

    else if (idx == 3) {
      vector3 axis(1, 0, 1);
      startMovie_reflection(axis);
      return;
    }

    else if (idx == 4) {
      vector3 axis(-1, 0, 1);
      startMovie_reflection(axis);
      return;
    }

    else if (idx == 5) {
      vector3 axis(0, 1, -1);
      startMovie_reflection(axis);
      return;
    }

    else if (idx == 6) {
      vector3 axis(0, 1, 1);
      startMovie_reflection(axis);
      return;
    }
  }
}

void OperatorTree::startMovie_Td() {
  uint idx = currentItem->text(0).toInt();

  if (currentOperation.startsWith("S4")) {

    if (idx == 1) {
      // startMovie_rotation(VZ,2);
      startMovie_rotation_reflection(VZ, 4);
      return;
    }

    else if (idx == 2) {
      // startMovie_rotation(VX,2);
      startMovie_rotation_reflection(VX, 4);
      return;
    }

    else if (idx == 3) {
      // startMovie_rotation(VY,2);
      startMovie_rotation_reflection(VY, 4);
      return;
    }

    return;
  }

  if (currentOperation.startsWith("C3")) {
    vector3 axis(1, 1, 1);

    if (idx == 1) {
      axis.Set(1, 1, 1);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 2) {
      axis.Set(-1, -1, 1);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 3) {
      axis.Set(-1, 1, -1);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 4) {
      axis.Set(1, -1, -1);
      startMovie_rotation(axis, 3);
      return;
    }
  }

  if (currentOperation.startsWith("SigmaD")) {
    if (idx == 1) {
      vector3 axis(1, 0, 1);
      startMovie_reflection(axis);
      return;
    }

    else if (idx == 2) {
      vector3 axis(-1, 0, 1);
      startMovie_reflection(axis);
      return;
    }

    else if (idx == 3) {
      vector3 axis(1, 1, 0);
      startMovie_reflection(axis);
      return;
    }

    else if (idx == 4) {
      vector3 axis(-1, 1, 0);
      startMovie_reflection(axis);
      return;
    }

    else if (idx == 5) {
      vector3 axis(0, 1, -1);
      startMovie_reflection(axis);
      return;
    }

    else if (idx == 6) {
      vector3 axis(0, 1, 1);
      startMovie_reflection(axis);
      return;
    }
  }
}

void OperatorTree::startMovie_T() {
  uint idx = currentItem->text(0).toInt();

  cout << idx << endl;

  if (currentOperation.startsWith("C2")) {
    // setC2_Visible();

    if (idx == 1) {
      startMovie_rotation(VZ, 2);
      return;
    }

    else if (idx == 2) {
      startMovie_rotation(VX, 2);
      return;
    }

    else if (idx == 3) {
      startMovie_rotation(VY, 2);
      return;
    }
  }

  if (currentOperation.startsWith("C3")) {
    // setC3_Visible();

    vector3 axis;

    if (idx == 1) {
      axis.Set(1, 1, 1);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 2) {
      axis.Set(-1, -1, 1);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 3) {
      axis.Set(-1, 1, -1);
      startMovie_rotation(axis, 3);
      return;
    }

    else if (idx == 4) {
      axis.Set(1, -1, -1);
      startMovie_rotation(axis, 3);
      return;
    }
  }
}

void OperatorTree::startMovie_Th() {
  if (currentOperation.startsWith("C2") || currentOperation.startsWith("C3")) {
    startMovie_T();
    return;
  }

  if (currentOperation.startsWith("Center")) {
    startMovie_reverse();
    return;
  }

  if (currentOperation.startsWith("SigmaH")) {

    uint idx = currentItem->text(0).toInt();

    if (idx == 1) {
      startMovie_reflection(VZ);
      return;
    }

    else if (idx == 2) {
      startMovie_reflection(VY);
      return;
    }

    else if (idx == 3) {
      startMovie_reflection(VX);
      return;
    }
  }
}

void OperatorTree::stopTimer() {
  if (timer != nullptr)
    timer->stop();
  view3d->setRenderMovie(false);
  // view3d->clearMovieData();
}

void OperatorTree::startTimer() {
  if (timer != nullptr)
    timer->start();
  view3d->setRenderMovie(true);
}

void OperatorTree::startMovie_Cnh() {
  if (currentOperation.contains("Cn")) {
    startMovie_Cn();
    return;
  }

  if (currentOperation.startsWith("Center")) {
    startMovie_reverse();
    return;
  }

  if (currentOperation.startsWith("SigmaH")) {
    startMovie_reflection(VZ);
  }
}

void OperatorTree::startMovie_Dnh() {
  if (currentOperation.contains("Cn") ||
      currentOperation.contains("HorizontalC2")) {
    startMovie_Dn();
    return;
  }

  if (currentOperation.startsWith("Center")) {
    startMovie_reverse();
    return;
  }

  if (currentOperation.startsWith("SigmaV")) {
    uint idx = currentItem->text(0).toInt();

    // cout << orderCn << " OperatorTree::startMovie_Dnd()"<< endl;

    vector3 axis;
    double theta = (idx - 1) * 3.141592654 / orderCn;

    // odd
    if (orderCn % 2 > 0)
      theta = theta + 3.14159264 / 2.0;

    axis.Set(cos(theta), sin(theta), 0.0);
    startMovie_reflection(axis);
    return;
  }

  if (currentOperation.startsWith("SigmaH")) {
    startMovie_reflection(VZ);
    return;
  }
}

void OperatorTree::startMovie_Dn() {
  // currentIdx
  if (currentOperation.contains("Cn")) {
    startMovie_rotation(VZ, orderCn);
    return;
  }

  if (currentOperation.contains("HorizontalC2")) {
    uint idx = currentItem->text(0).toInt();

    // cout << orderCn << "inside OperatorTree::startMovie_Dn()"<< endl;

    vector3 axis;
    double theta = idx * 3.141592654 / orderCn;

    // even
    if (orderCn % 2 < 1)
      theta = theta + 3.14159264;

    axis.Set(cos(theta), sin(theta), 0.0);

    startMovie_rotation(axis, 2);
  }
}

void OperatorTree::startMovie_Dnd() {
  if (currentOperation.contains("Cn") ||
      currentOperation.contains("HorizontalC2")) {
    startMovie_Dn();
    return;
  }

  if (currentOperation.contains("S2n")) {
    startMovie_rotation_reflection(VZ, orderCn * 2);
    return;
  }

  if (currentOperation.startsWith("Center")) {
    startMovie_reverse();
    return;
  }

  if (currentOperation.startsWith("SigmaD")) {
    uint idx = currentItem->text(0).toInt();

    // cout << orderCn << " OperatorTree::startMovie_Dnd()"<< endl;

    vector3 axis;
    double theta = (idx - 1) * 180 / orderCn + 180 / orderCn / 2;

    // odd
    if (orderCn % 2 > 0)
      theta += 90.0;

    theta = theta / 180 * 3.141592654;
    axis.Set(cos(theta), sin(theta), 0.0);
    startMovie_reflection(axis);
    return;
  }
}

void OperatorTree::startMovie_C00v() {}

void OperatorTree::startMovie_D00h() {
  if (currentOperation.startsWith("Rotation")) {
    // startMovie_rotation_(VZ, 36);
    return;
  }

  if (currentOperation.startsWith("Center")) {
    startMovie_reverse();
    return;
  }

  if (currentOperation.startsWith("SigmaH")) {
    startMovie_reflection(VZ);
    return;
  }
}

void OperatorTree::startMovie_Cn() { startMovie_rotation(VZ, orderCn); }

void OperatorTree::startMovie_Cs() { startMovie_reflection(VY); }

void OperatorTree::startMovie_Ci() { startMovie_reverse(); }

void OperatorTree::startMovie_Cnv() {

  // currentIdx
  if (currentOperation.contains("Cn")) {
    startMovie_Cn();
    return;
  }

  if (currentOperation.contains("SignamV")) {
    uint idx = currentItem->text(0).toInt();

    vector3 axis;
    double theta = (idx - 1) * 3.141592654 / orderCn;

    // odd
    if (orderCn % 2 > 0)
      theta = theta + 3.14159264 / 2.0;

    axis.Set(cos(theta), sin(theta), 0.0);
    startMovie_reflection(axis);
  }
}

void OperatorTree::startMovie_Sn() {
  if (currentOperation.contains("Sn")) {
    startMovie_rotation_reflection(VZ, orderCn);
    return;
  }

  if (currentOperation.contains("In")) {
    startMovie_rotation_reversion(VZ, orderCn);
    return;
  }

  if (currentOperation.contains("Center")) {
    startMovie_reverse();
  }
}

//---------------------------------------------------------------
void OperatorTree::startMovie_rotation(vector3 axis, uint order) {
  uint nFrames = mainWin->getMovieFrames();
  double angle = 360.0 / double(order);

  double ang = angle / double(nFrames);

  // cout << "startMovie_rotation(vector3 axis, uint order)  "<< endl;
  // cout << order << "  "<< angle<< endl;
  // cout << nFrames << "  "<< ang<< endl;

  view3d->copyMol();
  view3d->initIdxFrame();
  view3d->setRotateVector_movie(axis);
  view3d->setRotateAngle_movie(ang);
  view3d->setRenderMovie(true);

  if (timer != nullptr)
    delete timer;
  timer = new QTimer(this);
  timer->setInterval(100);

  connect(timer, SIGNAL(timeout()), this, SLOT(slot_rotation()));
  timer->start();
}

void OperatorTree::startMovie_reflection(vector3 axis) {
  view3d->copyMol();
  view3d->initIdxFrame();
  view3d->setRenderMovie(true);
  view3d->setReflectionVector_movie(axis);
  view3d->genReflectionAtoms();

  if (timer != nullptr)
    delete timer;
  timer = new QTimer(this);
  timer->setInterval(100);

  connect(timer, SIGNAL(timeout()), this, SLOT(slot_reflection()));
  timer->start();
}

void OperatorTree::startMovie_reverse() {
  // cout << " void OperatorTree::startMovie_reverse()"<<endl;
  view3d->copyMol();
  view3d->initIdxFrame();
  view3d->genReverseAtoms();
  view3d->setRenderMovie(true);

  if (timer != nullptr)
    delete timer;
  timer = new QTimer(this);
  timer->setInterval(100);

  connect(timer, SIGNAL(timeout()), this, SLOT(slot_reverse()));
  timer->start();
}

void OperatorTree::startMovie_rotation_reflection(vector3 axis, uint order) {
  uint nFrames = mainWin->getMovieFrames();

  double angle = 360.0 / double(order);
  double ang = angle / double(nFrames);

  view3d->copyMol();
  view3d->initIdxFrame();
  view3d->setRotateVector_movie(axis);
  view3d->setReflectionVector_movie(axis);
  view3d->setRotateAngle_movie(ang);
  view3d->genRotationReflectionAtoms(axis, angle);
  view3d->setRenderMovie(true);

  if (timer != nullptr)
    delete timer;
  timer = new QTimer(this);
  timer->setInterval(100);

  connect(timer, SIGNAL(timeout()), this, SLOT(slot_rotation_reflection()));
  timer->start();
}

void OperatorTree::startMovie_rotation_reversion(vector3 axis, uint order) {
  uint nFrames = mainWin->getMovieFrames();

  double angle = 360.0 / double(order);
  double ang = angle / double(nFrames);

  view3d->copyMol();
  view3d->initIdxFrame();
  view3d->setRotateVector_movie(axis);
  view3d->setRotateAngle_movie(ang);
  view3d->genRotationReversionAtoms(axis, angle);
  view3d->setRenderMovie(true);

  if (timer != nullptr)
    delete timer;
  timer = new QTimer(this);
  timer->setInterval(100);

  connect(timer, SIGNAL(timeout()), this, SLOT(slot_rotation_reversion()));
  timer->start();
}

void OperatorTree::slot_reflection() { view3d->startMovie_reflect(); }

void OperatorTree::slot_rotation() { view3d->startMovie_rotate(); }

void OperatorTree::slot_reverse() { view3d->startMovie_reverse(); }

void OperatorTree::slot_rotation_reflection() {
  view3d->startMovie_rotate_reflect();
}

void OperatorTree::slot_rotation_reversion() {
  view3d->startMovie_rotate_reverse();
}

void OperatorTree::setC2_Visible() {
  if (currentItem->checkState(0) == Qt::Unchecked)
    return;

  if (mainWin->isC2_InVisible())
    mainWin->setC2_Visible(0);
}

void OperatorTree::setC3_Visible() {
  if (currentItem->checkState(0) == Qt::Unchecked)
    return;

  if (mainWin->isC3_InVisible())
    mainWin->setC3_Visible();
}

void OperatorTree::setSigmaV_Visible() {
  if (currentItem->checkState(0) == Qt::Unchecked)
    return;

  if (mainWin->isSigmaV_InVisible())
    mainWin->setSigmaV_Visible(0);
}

void OperatorTree::setSigmaD_Visible() {
  if (currentItem->checkState(0) == Qt::Unchecked)
    return;

  // if(mainWin->isSigmaD_InVisible())
  //     mainWin->setSigmaD_Visible(0);
}

void OperatorTree::setSigmaH_Visible() {
  if (currentItem->checkState(0) == Qt::Unchecked)
    return;
  if (mainWin->isSigmaH_InVisible())
    mainWin->setSigmaH_Visible();
}

void OperatorTree::setCenter_Visible() {
  if (currentItem->checkState(0) == Qt::Unchecked)
    return;

  view3d->setCiVisible(true);
  if (mainWin->isPGElement_InVisible())
    mainWin->setPGElement_Visible();
}
