SET(CONVERTDB_SOURCES
        main.cpp
        ../server/database.cpp)

ADD_EXECUTABLE(convertdb MACOSX_BUNDLE ${CONVERTDB_SOURCES})
TARGET_LINK_LIBRARIES(convertdb Qt5::Sql)

# Installation

INSTALL(TARGETS convertdb RUNTIME DESTINATION bin COMPONENT applications
                          LIBRARY DESTINATION lib COMPONENT applications
                          BUNDLE  DESTINATION bin COMPONENT applications
                          ARCHIVE DESTINATION lib COMPONENT applications)

# Make convertdb self-contained wrt. Qt
# INSTALL_QT4_EXECUTABLE("bin/convertdb${execext}" "qsqlite" "" "${QT_BINARY_DIR}" "plugins" TRUE applications)
