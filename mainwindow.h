/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <stack>
#include <QStack>


QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
QT_END_NAMESPACE


#include "View3D.h"
#include "textEditor.h"

#include "atomProperty.h"
#include "ui_mainwindow.h"
#include "Mol.h"



class QStandardItemModel;

class templateMol;
class TemplateView3d;
class AtomTemplate;
class ringTemplate;


struct MolData
{
    vector < QString > atomSymbols;   //H, He, Li
    vector <vector3> atom3d;          //x,y,z
    vector < vector < unsigned int > > bonds; //beg,end, order

    vector < vector < unsigned int > > rings; //idx
    vector < vector3 > ringCenter;
    vector < vector3 > ringRadius;
    vector < vector3 > ringNormal;
    vector < bool >    ringAromatic;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() {}

 //   bool openFile(const QString &fileName);
     QString getExecutePath ();

     void recoverfromStack();



protected:
    void closeEvent(QCloseEvent *event) override;


public slots:
    void on_actionFileNew_triggered();
    void on_actionFileOpen_triggered();
    void on_actionFileSave2PNG_triggered();
    void on_pushButton_clean_clicked();


    void on_pushButton_rotate_clicked();

    void on_pushButton_translate_clicked();

    void on_pushButton_cleardata_clicked();


    void on_pushButton_Symmetrypercept_PG_clicked();

    void on_pushButton_SymmetryRefine_PG_clicked();

    void on_pushButton_center_PG_clicked();

    void on_pushButton_PatchMolecule_PG_clicked();

    void on_checkBox_PGElement_stateChanged(int arg1);

    void on_checkBox_kekule_stateChanged(int arg1);


    void on_checkBox_xyzAxis_stateChanged(int arg1);


    void on_checkBox_HideMol_stateChanged(int arg1);

    void on_checkBox_HideHydrogen_stateChanged(int arg1);


    //template
    void on_pushButton_tmp_Draw_clicked();
    void on_pushButton_tmp_Metal_clicked();
    void on_pushButton_tmp_R_clicked();
    void on_pushButton_tmp_Ring_clicked();

    void on_checkBox_outline_stateChanged(int arg1);
    void on_checkBox_atomLabel_stateChanged(int arg1);

    //void on_checkBox_stateChanged(int arg1);

    void on_doubleSpinBox_diskSize_Cn_valueChanged(double arg1);

    void on_doubleSpinBox_diskSize_C2_valueChanged(double arg1);

    void on_doubleSpinBox_diskSize_C3_valueChanged(double arg1);

    void on_doubleSpinBox_radius_Cn_valueChanged(double arg1);

    void on_doubleSpinBox_radius_C2_valueChanged(double arg1);

    void on_doubleSpinBox_radius_C3_valueChanged(double arg1);

    void on_doubleSpinBox_thickness_Cn_valueChanged(double arg1);

    void on_doubleSpinBox_thickness_C2_valueChanged(double arg1);

    void on_doubleSpinBox_thickness_C3_valueChanged(double arg1);



    //void on_pushButton_bk_clicked();

    //length
    void on_doubleSpinBox_V_length_valueChanged(double arg1);
    void on_doubleSpinBox_D_length_valueChanged(double arg1);
    void on_doubleSpinBox_H_length_valueChanged(double arg1);

    void on_doubleSpinBox_Cn_length_valueChanged(double arg1);
    void on_doubleSpinBox_C2_length_valueChanged(double arg1);
    void on_doubleSpinBox_C3_length_valueChanged(double arg1);

    //thickness
    void on_doubleSpinBox_V_thickness_valueChanged(double arg1);
    void on_doubleSpinBox_D_thickness_valueChanged(double arg1);
    void on_doubleSpinBox_H_thickness_valueChanged(double arg1);

    //visible
    void on_checkBox_CnHide_stateChanged(int arg1);
    void on_checkBox_C2Hide_stateChanged(int arg1);
    void on_checkBox_C3Hide_stateChanged(int arg1);
    void on_checkBox_SigmaV_Hide_stateChanged(int arg1);
    void on_checkBox_SigmaD_Hide_stateChanged(int arg1);
    void on_checkBox_SigmaH_Hide_stateChanged(int arg1);


    //Horizontal C2 type
    void on_checkBox_TypeC2_I_stateChanged(int arg1);
    void on_checkBox_TypeC2_II_stateChanged(int arg1);


    void on_pushButton_Cn_clicked();
    void on_pushButton_C2_clicked();
    void on_pushButton_C3_clicked();


    //Cn
    void on_spinBox_Cn_green_valueChanged(int arg1);
    void on_spinBox_Cn_blue_valueChanged(int arg1);
    void on_spinBox_Cn_red_valueChanged(int arg1);


    //C2
    void on_spinBox_C2_red_valueChanged(int arg1);
    void on_spinBox_C2_green_valueChanged(int arg1);
    void on_spinBox_C2_blue_valueChanged(int arg1);

    //C3
    void on_spinBox_C3_red_valueChanged(int arg1);
    void on_spinBox_C3_green_valueChanged(int arg1);
    void on_spinBox_C3_blue_valueChanged(int arg1);

    //sigma-V
    void on_spinBox_V_red_valueChanged(int arg1);
    void on_spinBox_V_green_valueChanged(int arg1);
    void on_spinBox_V_blue_valueChanged(int arg1);
    void on_spinBox_V_opacity_valueChanged(int arg1);
    void on_pushButton_V_clicked();


    //sigma-D
    void on_pushButton_D_clicked();
    void on_spinBox_D_red_valueChanged(int arg1);
    void on_spinBox_D_green_valueChanged(int arg1);
    void on_spinBox_D_blue_valueChanged(int arg1);
    void on_spinBox_D_opacity_valueChanged(int arg1);

    //sigma-H
    void on_pushButton_H_clicked();
    void on_spinBox_H_red_valueChanged(int arg1);
    void on_spinBox_H_green_valueChanged(int arg1);
    void on_spinBox_H_blue_valueChanged(int arg1);
    void on_spinBox_H_opacity_valueChanged(int arg1);


    void onTextEditPaste();


    void on_spinBox_V_type_valueChanged(int arg1);
    void on_spinBox_D_type_valueChanged(int arg1);
    void on_spinBox_H_type_valueChanged(int arg1);



    void on_pushButton_thermalMove_clicked();


    void on_pushButton_rotate2_clicked();
    void on_pushButton_translate2_clicked();

    void on_dialAtomScale_valueChanged(int value);
    void on_dialBondScale_valueChanged(int value);
    void on_comboBox_symmetry_currentIndexChanged(int index);



    void on_actionEditCut_triggered();
    void on_actionEditUndo_triggered();
    void on_actionEditRedo_triggered();
    void on_actionToolscentralize_triggered();
    void on_actionFileExport_triggered();


    void on_actionSymmetryView_triggered();

    void on_actionSingleBond_triggered();
    void on_actionDoubleBond_triggered();
    void on_actionTripleBond_triggered();
    void on_actionQuadrupleBond_triggered();
    void on_actionWeakBond_triggered();
    void on_actionHydrogenbond_triggered();
    void on_actionAromaticBond_triggered();


    void on_actionToolsAddHydrogenAuto_triggered();


    void on_actionMMFF94_triggered();
    void on_actionGhemical_triggered();
    void on_actionUFF_triggered();
    void on_actionMM2_triggered();
    void on_actionGaff_triggered();

    void on_actionreCalculateBonds_triggered();

    void on_actionPeriodicTable_triggered();

    void on_pushButton_rebond_clicked();

    void on_pushButton_UFF_clicked();
    void on_pushButton_Ghemical_clicked();
    void on_pushButton_GAFF_clicked();

    void on_pushButton_removeX_clicked();

    void on_actionXTB_optimize_triggered();


    void on_pushButton_Atom_clicked();
    void on_pushButton_Bond_clicked();
    void on_pushButton_Ring_clicked();
    void on_pushButton_Polyhedron_clicked();



public:
    void on_pushButton_addHydrogen_clicked();

    void addMolecule(QString &);

    void clearAllViews();
    void loadFile(QString );
    void saveFile(QString );

    void displayMolCoordinates(HMol * );

    void setFreshView (bool s) {needFreshView=s;}

    View3D * getView3D () {return view3d;}

    HMol * getMol () {return mol;}


    void  loadTemplateMol ();
    void  clearTemplate() ;
    void  setTemplateName(QString );


    void updateLastStackOne();
    void buildMolData ();
    QString buildSelectedAtomBondInfo();

    void push2Stack (QString );
    void push2Stack_selection (QString ); //special case for selection
    void recoverStackData (QString & );

    void undo () {on_actionEditUndo_triggered();}
    void redo () {on_actionEditRedo_triggered();}

    void setLinkedMode() {view3d->setLinkTemplateMode();}


    void getDataFromPanel();
    void updatePanelParameters();
    void updateBasedOnSymmetry(QString );

    void loadDefaultColorSets();
    void symmetryPercept();

    void disable_Cn();
    void disable_C2();
    void disable_C3();
    void disable_SigmaV();
    void disable_SigmaH();
    void disable_SigmaD();

public:
    void checkBox_SigmaV_Hide_stateChanged_Td(int arg1);
    void checkBox_SigmaV_Hide_stateChanged_Th(int arg1);
    void checkBox_SigmaV_Hide_stateChanged_Oh(int arg1);
    void checkBox_SigmaV_Hide_stateChanged_Ih(int arg1);
public:
    void setSymmetryElementVisible( );

    void setSigmaV_Visible();
    void setSigmaD_Visible();
    void setSigmaH_Visible();


    void setOutLineVisible();
    void setXYZAxisVisible( );
    void setAtomLabelVisible();
    void setCn_Visible() ;
    void setMolVisible( ) ;

    bool isC2_InVisible();
    bool isC3_InVisible();
    bool isSigmaV_InVisible();
    bool isSigmaD_InVisible();
    bool isSigmaH_InVisible();
    bool isPGElement_InVisible();


    void setSigmaV_Visible(uint type);
    void setSigmaD_Visible(uint type);
    void setSigmaH_Visible(uint type);

    void setSigmaV_Invisible(uint type);
    void setSigmaD_Invisible(uint type);
    void setSigmaH_Invisible(uint type);



    void setAllC3_Visible(bool t);
    void setAllC2_Visible(bool t);

    void setC2_Invisible(uint type);
    void setC2_Visible(uint type);
    void setC3_Visible();
    void setPGElement_Visible();  //reverse
    void setKekuleStyle() ;
    void setPGElement_Visible(bool );

    void setHydrogenVisible( ) ;

    void modifyCheckBox_CnHide (bool t=true, bool blockSignal=false);
    void modifycheckBox_SigmaH_Hide(bool );
    void modifycheckBox_SigmaD_Hide(bool t);
    void modifycheckBox_SigmaV_Hide(bool t);

    uint getMovieFrames();

    bool isLoopPlaying();
    void setLoopPlayingStop();

    bool update_checkBox_PGElement=false;




public:
    QString nameTMol;
    HMol * mol;
    View3D  *  view3d;
    TextEditor *  infoview;


    TemplateView3d  * view3dT;

    QStack < QString  > undoList;
    QStack < QString  > redoList;
    QString dataMol,dataMol0;
    bool isFirstTime;
    void setFirstTime(bool b){isFirstTime=b;}
    void initSymmetry();

    void displayMessage(QString );
    

private:
    enum { MaxRecentFiles = 5 };
    bool isMetalTemplate;
/*
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool loadFile(const QString &fileName);
    static bool hasRecentFiles();
    void prependToRecentFiles(const QString &fileName);
    void setRecentFilesVisible(bool visible);
    MdiChild *activeMdiChild() const;
    QMdiSubWindow *findMdiChild(const QString &fileName) const;
    */

    QMdiArea *mdiArea;
    QString FileName;
    QLabel ShowLabel, VersionLabel;;

   // QMenu *windowMenu;
   // QAction *newAct;
  //  QAction *saveAct;
   // QAction *saveAsAct;
  //  QAction *recentFileActs[MaxRecentFiles];
  //  QAction *recentFileSeparator;
  //  QAction *recentFileSubMenuAct;



public slots:

  void actionShow3D();
  void on3DViewActive();
  void onInformActive();

  void on_actionToolsPencil_triggered();
  void on_actionEditCopy_triggered();
  void on_actionToolsRotate_triggered();


private slots:
  void on_actionEditPaste_triggered();
  void on_actionxyz_triggered();
  void on_actionEditbond_triggered();
  void on_actionEditangle_triggered();
  void on_actionEditdihedral_triggered();

  void on_actionXOY_triggered();
  void on_actionXOZ_triggered();
  void on_actionYOZ_triggered();

  void on_pushButton_startMovie_clicked();
  void on_pushButton_showAll_clicked();
  void on_pushButton_hideAll_clicked();


  void on_doubleSpinBox_movieOpacity_valueChanged(double arg1);

  void on_pushButton_flip_x_clicked();

  void on_pushButton_flip_z_clicked();

  void on_pushButton_flip_y_clicked();

  void on_action_author_triggered();

  void on_actionShortCut_triggered();



  void on_lineEdit_XX_textEdited(const QString &arg1);

  void on_lineEdit_YY_textEdited(const QString &arg1);

  void on_lineEdit_ZZ_textEdited(const QString &arg1);

  void on_lineEdit_X_textEdited(const QString &arg1);

  void on_lineEdit_Y_textEdited(const QString &arg1);

  void on_lineEdit_Z_textEdited(const QString &arg1);


  void on_comboBox_example_currentTextChanged(const QString &arg1);

  private:
  Ui::MainWindow ui;
  bool isStandardOrientation, needFreshView;

  vector3 Color_V,Color_D,Color_H,Color_bk;
  vector3 Color_Cn,Color_C2,Color_C3;

  int  Opacity_V, Opacity_D, Opacity_H,Opacity_bk;
  double scaleThicknessSigma_V,
         scaleThicknessSigma_D,
         scaleThicknessSigma_H,
         scaleThickness_Cn;

  //back-up
  double scaleThicknessSigmaV,
         scaleThicknessSigmaD,
         scaleThicknessSigmaH,
         scaleThicknessCn,
         scaleThicknessC2,
         scaleThicknessC3;

  vector3 ColorV,ColorD,ColorH,Colorbk,ColorCn;
  int  OpacityV, OpacityD, OpacityH;
  bool isC3toC2;
  bool _isCnv, _isCn, _isCnh, _isDnd, _isDnh, _isDn, _isT, _isTd, _isTh;
  bool _isI, _isIh, _isC00v, _isD00h, _isCs, _isCi,_isC1, _isSn;
  bool _isO, _isOh;

  double value_V_length=0;
  double value_D_length=0;
  double value_H_length=0;

};

#endif
