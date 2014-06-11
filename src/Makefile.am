SUBDIRS = API Base BBox Exceptions Geometry LinearAlgebra MedialAxis MinMax SphereTree Storage Surface Export 

noinst_LTLIBRARIES = libSTG.la
libSTG_la_SOURCES = null.c
libSTG_la_LIBADD = API/libAPI.la Base/libBase.la BBox/libBBox.la Exceptions/libExceptions.la Geometry/libGeometry.la LinearAlgebra/libMatrix.la MedialAxis/libMedialAxis.la MinMax/libMinMax.la SphereTree/libSphereTree.la Storage/libStorage.la Surface/libSurface.la Export/libExport.la

bin_PROGRAMS=makeTreeMedial makeTreeHubbard makeTreeOctree makeTreeGrid makeTreeSpawn

makeTreeMedial_SOURCES=makeTreeMedial.cpp DecodeParam.cpp DecodeParam.h EvalTree.cpp EvalTree.h VerifyModel.cpp VerifyModel.h
makeTreeMedial_LDADD=libSTG.la

makeTreeHubbard_SOURCES=makeTreeHubbard.cpp VerifyModel.cpp VerifyModel.h DecodeParam.cpp DecodeParam.h
makeTreeHubbard_LDADD=libSTG.la

makeTreeOctree_SOURCES=makeTreeOctree.cpp DecodeParam.cpp DecodeParam.h
makeTreeOctree_LDADD=libSTG.la

makeTreeGrid_SOURCES=makeTreeGrid.cpp VerifyModel.cpp VerifyModel.h EvalTree.cpp EvalTree.h DecodeParam.cpp DecodeParam.h
makeTreeGrid_LDADD=libSTG.la

makeTreeSpawn_SOURCES=makeTreeSpawn.cpp VerifyModel.cpp VerifyModel.h EvalTree.cpp EvalTree.h DecodeParam.cpp DecodeParam.h
makeTreeSpawn_LDADD=libSTG.la

