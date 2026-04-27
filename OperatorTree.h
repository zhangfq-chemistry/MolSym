#ifndef OBTREE_H
#define OBTREE_H
#include <QIcon>
#include <QTimer>
#include <QTreeWidget>
#include <QSharedPointer>
#include <QVector>
#include <QMenu>


#include <QObject>
#include <QWidget>
#include <QLabel>

#include "math/vector3.h"

class MainWindow;
class QSettings;
class View3D;

class OperatorTree : public QTreeWidget
{
    Q_OBJECT

public:
    typedef QSharedPointer<QSettings> SettingsPtr;
    OperatorTree(QWidget *parent = 0);
    ~OperatorTree() ;

    MainWindow * getMainWindow();
    void setViewer(View3D *v) {view3d=v;}
    void setMainWin (MainWindow *  w) {mainWin =w; }



    QSize sizeHint() const override;
    void setSettingsObject(const SettingsPtr &settings);


    void updateCnv(QTreeWidgetItem *, bool );
    void updateCnh(QTreeWidgetItem *, bool );
    void updateCn(QTreeWidgetItem *, bool );

    void updateDn(QTreeWidgetItem *, bool );
    void updateDnd(QTreeWidgetItem *, bool );
    void updateDnh(QTreeWidgetItem *, bool );


    void updateCs(bool );
    void updateCi(bool );
    void updateSn(QTreeWidgetItem *,bool);
    void updateC00v(QTreeWidgetItem *, bool );
    void updateD00h(QTreeWidgetItem *, bool );


    void updateT(QTreeWidgetItem *, bool );
    void updateTd(QTreeWidgetItem *, bool );
    void updateTh(QTreeWidgetItem *, bool );

    void updateO(QTreeWidgetItem *, bool );
    void updateOh(QTreeWidgetItem *, bool );

    void updateI(QTreeWidgetItem *, bool );
    void updateIh(QTreeWidgetItem *, bool );

    void setAllUnchecked();
    void setAllChecked();

    void stopTimer();
    void startTimer();




    uint current_idx;
    bool current_visible;
    void update_C2();
    void update_SigmaV();
    void update_SigmaD();
    void update_SigmaH();


public slots:
    void updateSetting(QTreeWidgetItem *);
    void updateSelection(QTreeWidgetItem *);
    void setAutoRefresh(bool autoRefresh);
    void setFallbacksEnabled(bool enabled);
    void maybeRefresh();
    void refresh();




    void slot_rotation();
    void slot_reflection();
    void slot_reverse();
    void slot_rotation_reflection();
    void slot_rotation_reversion();




public:
    void setSymmetry(QString );

    void addViewsItems(QTreeWidgetItem * ,  QString ,  int );
    void removeAllItems();

    void addViewsItems_Cs(QTreeWidgetItem *);
    void addViewsItems_Ci(QTreeWidgetItem * );



    void addViewsItems_Cnv(QTreeWidgetItem * , QString );
    void addViewsItems_Cnh(QTreeWidgetItem * , QString );
    void addViewsItems_Cn(QTreeWidgetItem * , QString );
    void addViewsItems_Sn(QTreeWidgetItem * , QString );

    void addViewsItems_Dn(QTreeWidgetItem * , QString );
    void addViewsItems_Dnh(QTreeWidgetItem * , QString );
    void addViewsItems_Dnd(QTreeWidgetItem * , QString );
    void addViewsItems_D00h(QTreeWidgetItem *);



    void addViewsItems_T(QTreeWidgetItem *  );
    void addViewsItems_Td(QTreeWidgetItem *  );
    void addViewsItems_Th(QTreeWidgetItem *  );


    void addViewsItems_O(QTreeWidgetItem *  );
    void addViewsItems_Oh(QTreeWidgetItem *  );

    void addViewsItems_I(QTreeWidgetItem *  );
    void addViewsItems_Ih(QTreeWidgetItem *  );

    void initSymmetry();

    bool isCnv () {return _isCnv;}
    bool isCn () {return _isCn;}
    bool isCnh () {return _isCnh;}

    bool  isDnd() {return _isDnd;}
    bool  isDnh(){return _isDnh;}
    bool  isDn() {return _isDnh;}

    bool  isT() {return _isT;}
    bool  isTd() {return _isTd;}
    bool  isTh() {return _isTh;}

    bool  isI() {return _isI;}
    bool  isIh() {return _isIh;}

    bool  isO() {return _isO;}
    bool  isOh() {return _isOh;}

    bool  isC00v() {return _isC00v;}
    bool  isD00h() {return _isD00h;}
    bool  isCs() {return _isCs;}
    bool  isCi() {return _isCi;}
    bool  isSn() {return _isSn;}
    uint  getCnOrder (){return orderCn;}





    void updateChildItems(QTreeWidgetItem *parent);
    void moveItemForward(QTreeWidgetItem *parent, int oldIndex, int newIndex);

    QTreeWidgetItem *createItem(const QString &text, QTreeWidgetItem *parent,
                                int index);
    QTreeWidgetItem *childAt(QTreeWidgetItem *parent, int index);

    int childCount(QTreeWidgetItem *parent);
    int findChild(QTreeWidgetItem *parent, const QString &text, int startIndex);

    void startMovie();

    void startMovie_I();
    void startMovie_Ih();
    void startMovie_O();
    void startMovie_Oh();
    void startMovie_Td();
    void startMovie_T();
    void startMovie_Th();

    void startMovie_Cs();
    void startMovie_Ci();

    void startMovie_Sn();
    void startMovie_Cn();
    void startMovie_Cnv();
    void startMovie_Cnh();


    void startMovie_Dnh();
    void startMovie_Dn();
    void startMovie_Dnd();
    void startMovie_C00v();
    void startMovie_D00h();

    void startMovie_rotation(vector3 axis=VZ, uint order=1);
    void startMovie_reflection(vector3 axis=VZ);
    void startMovie_reverse();
    void startMovie_rotation_reflection(vector3 axis=VZ, uint order=2);
    void startMovie_rotation_reversion(vector3 axis=VZ, uint order=2);


    void setC2_Visible ();
    void setC2_II_Visible ();
    void setC3_Visible ();
    void setSigmaV_Visible ();
    void setSigmaD_Visible ();
    void setSigmaH_Visible ();
    void setCenter_Visible ();


    void set_SigmaV_Checked(uint type,bool t);
    void set_SigmaD_Checked(uint type,bool t);

    void set_SigmaH_Checked_index(uint index,bool t);
    void set_SigmaV_Checked_index(uint index,bool t);
    void set_SigmaD_Checked_index(uint index,bool t);


    void set_SigmaD1_Checked_index(uint index,bool t);
    void set_SigmaD2_Checked_index(uint index,bool t);
    void set_SigmaD3_Checked_index(uint index,bool t);

    void set_C2_Checked(uint type=0,bool t=true);
    void set_C3_Checked(bool t=true);

    void set_Cn_Checked(bool t=true);



    void check_All_C2_state(bool );
    void check_All_C3_state(bool );

    void check_All_SigmaV_state(bool );
    void check_All_SigmaD_state(bool );
    void check_All_SigmaH_state(bool );






private:
    uint currentIdx;
    QString currentOperation;
    QTreeWidgetItem * currentItem;

    QIcon groupIcon;
    QIcon keyIcon;


    QString nameSymmetry="C1";


    View3D * view3d=nullptr;
    MainWindow * mainWin=nullptr;
    QTreeWidgetItem * itemSymmetry=nullptr;

    SettingsPtr settings;
    QTimer refreshTimer;
    bool autoRefresh;

    bool _isCnv, _isCn, _isCnh, _isDnd, _isDnh, _isDn, _isT, _isTd, _isTh;
    bool _isI, _isIh, _isC00v, _isD00h, _isCs, _isCi,_isC1, _isSn;
    bool _isO, _isOh;


    uint orderCn=1;
    QTimer * timer = nullptr;



};

#endif // OBTREE_H
