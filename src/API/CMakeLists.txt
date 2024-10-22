set(BOT_LIB_NAME API)
add_subdirectory(SFWhite)

add_library(${BOT_LIB_NAME} STATIC
        DomainSampler.cpp Hybrid.cpp MSBase.cpp MSGrid.cpp MSRelaxation.cpp MergeHubbard.cpp REBase.cpp REDiscard.cpp RELargest.cpp RELargestLM.cpp REMaxElim.cpp RESelect.cpp SEConvex.cpp SEPoint.cpp SESphPt.cpp SFBase.cpp SFMinErr.cpp SFRitter.cpp SFWhite.cpp SOBalance.cpp SOBase.cpp SOPerSphere.cpp SOSimplex.cpp SRBurst.cpp SRComposite.cpp SRExpand.cpp SRGrid.cpp SRMerge.cpp SRQuickBurst.cpp SRSpawn.cpp SRStandOff.cpp SRVoronoi.cpp SSIsohedron.cpp SSRings.cpp STGGeneric.cpp STGHubbard.cpp STGOctree.cpp SurfaceDiv.cpp VFAdaptive.cpp VFGapCross.cpp 
)

target_link_libraries(${BOT_LIB_NAME} SFWhite)
