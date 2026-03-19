include("/home/wiktor/Desktop/WDS/MazeMotion/build/Desktop_Qt_6_10_2-Debug/.qt/QtDeploySupport.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/MazeMotion-plugins.cmake" OPTIONAL)
set(__QT_DEPLOY_I18N_CATALOGS "qtbase")

qt6_deploy_runtime_dependencies(
    EXECUTABLE "/home/wiktor/Desktop/WDS/MazeMotion/build/Desktop_Qt_6_10_2-Debug/MazeMotion"
    GENERATE_QT_CONF
)
