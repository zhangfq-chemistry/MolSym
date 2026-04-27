#ifndef PERIODICTABLE_H
#define PERIODICTABLE_H

#include <QFrame>
#include <ui_periodicTable.h>
#include "View3D.h"

namespace Ui {
class PeriodicTable;
}

class PeriodicTable : public QFrame
{
    Q_OBJECT
private:
     View3D  *  view3d;

public:
    explicit PeriodicTable(QWidget *parent = nullptr);
    ~PeriodicTable();

    void init();
    void OnClose();
    void setView3d(View3D * v) {view3d=v;}

private slots:
    void on_element_0_clicked();
    void on_element_00_clicked();
    void on_element_1_clicked();
    void on_element_2_clicked();
    void on_element_3_clicked();
    void on_element_4_clicked();
    void on_element_5_clicked();
    void on_element_6_clicked();
    void on_element_7_clicked();
    void on_element_8_clicked();
    void on_element_9_clicked();
    void on_element_10_clicked();

    void on_element_11_clicked();
    void on_element_12_clicked();
    void on_element_13_clicked();
    void on_element_14_clicked();
    void on_element_15_clicked();
    void on_element_16_clicked();
    void on_element_17_clicked();
    void on_element_18_clicked();
    void on_element_19_clicked();
    void on_element_20_clicked();


    void on_element_21_clicked();
    void on_element_22_clicked();
    void on_element_23_clicked();
    void on_element_24_clicked();
    void on_element_25_clicked();
    void on_element_26_clicked();
    void on_element_27_clicked();
    void on_element_28_clicked();
    void on_element_29_clicked();
    void on_element_30_clicked();

    void on_element_31_clicked();
    void on_element_32_clicked();
    void on_element_33_clicked();
    void on_element_34_clicked();
    void on_element_35_clicked();
    void on_element_36_clicked();
    void on_element_37_clicked();
    void on_element_38_clicked();
    void on_element_39_clicked();
    void on_element_40_clicked();

    void on_element_41_clicked();
    void on_element_42_clicked();
    void on_element_43_clicked();
    void on_element_44_clicked();
    void on_element_45_clicked();
    void on_element_46_clicked();
    void on_element_47_clicked();
    void on_element_48_clicked();
    void on_element_49_clicked();
    void on_element_50_clicked();

    void on_element_51_clicked();
    void on_element_52_clicked();
    void on_element_53_clicked();
    void on_element_54_clicked();
    void on_element_55_clicked();
    void on_element_56_clicked();
    void on_element_57_clicked();
    void on_element_58_clicked();
    void on_element_59_clicked();
    void on_element_60_clicked();

    void on_element_61_clicked();
    void on_element_62_clicked();
    void on_element_63_clicked();
    void on_element_64_clicked();
    void on_element_65_clicked();
    void on_element_66_clicked();
    void on_element_67_clicked();
    void on_element_68_clicked();
    void on_element_69_clicked();
    void on_element_70_clicked();

    void on_element_71_clicked();
    void on_element_72_clicked();
    void on_element_73_clicked();
    void on_element_74_clicked();
    void on_element_75_clicked();
    void on_element_76_clicked();
    void on_element_77_clicked();
    void on_element_78_clicked();
    void on_element_79_clicked();
    void on_element_80_clicked();


    void on_element_81_clicked();
    void on_element_82_clicked();
    void on_element_83_clicked();
    void on_element_84_clicked();
    void on_element_85_clicked();
    void on_element_86_clicked();
    void on_element_87_clicked();
    void on_element_88_clicked();
    void on_element_89_clicked();
    void on_element_90_clicked();

    void on_element_91_clicked();
    void on_element_92_clicked();
    void on_element_93_clicked();
    void on_element_94_clicked();
    void on_element_95_clicked();
    void on_element_96_clicked();
    void on_element_97_clicked();
    void on_element_98_clicked();
    void on_element_99_clicked();

    void on_element_100_clicked();
    void on_element_101_clicked();
    void on_element_102_clicked();
    void on_element_103_clicked();


private:
    Ui::PeriodicTable ui;
    unsigned int idx;
};

#endif // PERIODICTABLE_H
