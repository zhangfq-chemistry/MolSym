QT     += core gui widgets openglwidgets

requires(qtConfig(filedialog))
requires(qtConfig(treeview))


DEFINES+=DMS_INCLUDE_SOURCE
# OpenMP Å




NCLUDEPATH += .


#INCLUDEPATH    += /usr/local/include  /usr/local/include/vtk-8.2
#DEPENDPATH     += /usr/local/include  /usr/local/include/vtk-8.2
#LIBS += -L/usr/local/lib64 -L/usr/local/lib


LIBS += -L/usr/local/lib64 -L/usr/local/lib -lopenbabel
LIBS += -L/opt/Qt/6.10.1/gcc_64/lib -lQtColorWidgets


#LIBS +=  -lopenbabel -lQtColorWidgets


TARGET = MolSym
TEMPLATE = app


CONFIG -= gnu++11
QMAKE_CXXFLAGS+= -std=c++11


#LIBS += -LD:\openbabel\bin -lopenbabel.dll -lQtColorWidgets2
#LIBS += -LD:\Qt\5.15.2\mingw81_64\bin
#LIBS += -LD:\Qt\5.15.2\mingw81_64\lib

#INCLUDEPATH += D:\Qt
#INCLUDEPATH += D:\Qt\5.15.2\mingw81_64\include
#INCLUDEPATH += D:\openbabel\include\openbabel3
#INCLUDEPATH += E:\zhangfq\2021


INCLUDEPATH    += /usr/include
DEPENDPATH     += /usr/include

INCLUDEPATH    += /usr/local/include/openbabel3

CONFIG(win32, win32|macx){
    VTK_INCLUDE_DIR = D:\VTK\include\vtk-9.0
    VTK_LIB_DIR = D:\VTK\lib
    VTK_LIB_VER = 9.0.dll.a
    VTK_LIB_PREFIX = $${VTK_LIB_VER}
    VTK_LIB_NAME =  \
    vtkCommonColor\
         vtkCommonComputationalGeometry\
         vtkCommonCore\
         vtkCommonDataModel\
         vtkCommonExecutionModel\
         vtkCommonMath\
         vtkCommonMisc\
         vtkCommonSystem\
         vtkCommonTransforms\
         vtkFiltersCore\
         vtkFiltersGeneral\
         vtkFiltersGeneric\
         vtkFiltersGeometry\
         vtkFiltersPoints\
         vtkFiltersSelection\
         vtkFiltersSources\
         vtkGUISupportQt\
         vtkIOCore\
         vtkIOLegacy\
         vtkIOImage\
         vtkImagingColor\
         vtkInteractionStyle\
         vtkInteractionWidgets\
         vtkRenderingCore\
         vtkRenderingFreeType\
         vtkRenderingLabel\
         vtkRenderingOpenGL2\
         vtkRenderingQt\
         vtkRenderingImage\
         vtkViewsCore\
         vtkViewsQt\
         vtkfreetype\
         vtkpng

    for(lib,VTK_LIB_NAME){
        VTK_LIBS += $${VTK_LIB_DIR}\lib$${lib}-$${VTK_LIB_PREFIX}
    }
    INCLUDEPATH += $${VTK_INCLUDE_DIR}
    DEPENDPATH += $${VTK_INCLUDE_DIR}
    LIBS += -L$${VTK_LIB_DIR} $${VTK_LIBS}
}
else{ #LINUX
    VTK_INCLUDE_DIR = /usr/local/include/vtk-9.6
    VTK_LIB_DIR = /usr/local/lib
    VTK_LIB_VER = 9.6
    VTK_LIB_PREFIX = 9.6

    VTK_LIB_NAME =  \
                    vtkCommonCore\
                    vtkCommonDataModel\
                    vtkCommonExecutionModel\
                    vtkCommonMath\
                    vtkCommonSystem\
                    vtkCommonTransforms\
                    vtkCommonComputationalGeometry\
                    vtkFiltersSources\
                    vtkFiltersCore\
                    vtkFiltersExtraction\
                    vtkFiltersGeneral\
                    vtkFiltersGeometry\
                    vtkFiltersSources\
                    vtkFiltersModeling\
                    vtkFiltersProgrammable\
                    vtkGUISupportQt\
                    vtkIOCore\
                    vtkIOGeometry\
                    vtkIOImage\
                    vtkImagingColor\
                    vtkImagingCore\
                    vtkImagingGeneral\
                    vtkImagingMath\
                    vtkImagingSources\
                    vtkImagingStencil\
                    vtkInteractionStyle\
                    vtkInteractionWidgets\
                    vtkRenderingCore\
                    vtkRenderingFreeType\
                    vtkRenderingImage\
                    vtkRenderingLabel\
                    vtkRenderingOpenGL2\
                    vtkRenderingQt\
                    vtkViewsCore\
                    vtkViewsQt\
                    vtkfreetype\
                    vtkpng\
                    vtksys\
                    vtkzlib
    for(lib,VTK_LIB_NAME){
        VTK_LIBS += -l$${lib}-$${VTK_LIB_VER}
    }
    INCLUDEPATH += $${VTK_INCLUDE_DIR}
    DEPENDPATH += $${VTK_INCLUDE_DIR}
    LIBS += -L$${VTK_LIB_DIR} $${VTK_LIBS}
}


HEADERS   = mainwindow.h \
    #DDMatrix/BaseMathOperators.h \
    #DDMatrix/Complex.h \
    #DDMatrix/DenseMatrixBase.h \
    #DDMatrix/DenseMatrixBase.tcc \
    #DDMatrix/MathOperators.h \
    #DDMatrix/Matrix.h \
    #DDMatrix/Matrix.tcc \
    #DDMatrix/MatrixBase.h \
    #DDMatrix/MatrixBase.tcc \
    #DDMatrix/StepVectorRange.h \
    #DDMatrix/SymmMatrixBase.h \
    #DDMatrix/SymmMatrixBase.tcc \
    #DDMatrix/VectorRange.h \
    Mol.h \
    OperatorTree.h \
    aboutDialog.h \
    mTools.h \
    math/symmetry.h \
    math/vector3.h \
    math/matrix3x3.h \
    textEditor.h \
    View3D.h \
    utils.h \
    bondProperty.h \
    templateMol.h \
    templateView3d.h \
    rTemplate.h \
    InteractorT.h \
    atomTemplate.h \
    angleProperty.h \
    torsionProperty.h \
    ringTemplate.h \
    periodicTable.h \
    showTextForm.h \
    vtkbool/AABB.h \
    vtkbool/Decomposer.h \
    vtkbool/Merger.h \
    vtkbool/RmTrivials.h \
    vtkbool/Tools.h \
    vtkbool/Utilities.h \
    vtkbool/VisPoly.h \
    vtkbool/vtkPolyDataBooleanFilter.h \
    vtkbool/vtkPolyDataContactFilter.h \
    AreaPicker.h \
    InteractorArea.h \
    codeEditor.h \
    atomProperty.h \
    ringProperty.h

SOURCES       = main.cpp \
    Mol.cpp \
    OperatorTree.cpp \
    View3D_1.cpp \
    aboutDialog.cpp \
    mTools.cpp \
    mainwindow.cpp \
    math/matrix3x3.cpp \
    math/symmetry.cpp \
    math/vector3.cpp \
    textEditor.cpp \
    View3D.cpp \
    utils.cpp \
    InteractorT.cpp \
    bondProperty.cpp \
    templateMol.cpp \
    templateView3d.cpp \
    rTemplate.cpp \
    atomTemplate.cpp \
    angleProperty.cpp \
    torsionProperty.cpp \
    ringTemplate.cpp \
    periodicTable.cpp \
    showTextForm.cpp \
    vtkbool/Decomposer.cpp \
    vtkbool/Merger.cpp \
    vtkbool/RmTrivials.cpp \
    vtkbool/Tools.cpp \
    vtkbool/Utilities.cpp \
    vtkbool/VisPoly.cpp \
    vtkbool/vtkPolyDataBooleanFilter.cpp \
    vtkbool/vtkPolyDataContactFilter.cpp \
    vtkCapsuleSource.cpp \
    AreaPicker.cpp \
    InteractorArea.cpp \
    codeEditor.cpp \
    atomProperty.cpp \
    ringProperty.cpp







FORMS += \
    UI/about.ui \
    UI/mainwindow.ui \
    UI/angle.ui \
    UI/bond.ui \
    UI/torsion.ui \
    UI/ringTemplate.ui \
    UI/periodicTable.ui \
    UI/showTextForm.ui \
    UI/rTemplate.ui \
    UI/AtomTemplate.ui \
    UI/dialogAtom.ui \
    UI/dialogRing.ui




RESOURCES +=  src.qrc

DISTFILES +=






