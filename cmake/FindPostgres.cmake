#  LIBPQ_INCLUDE_DIR - where to find libpq-fe.h, etc.
#  LIBPQ_LIBRARY     - List of libraries when using libpq.
#  LIBPQ_FOUND       - True if libpq found.

#  Read environment variables to find mysql include/lib directories
SET(LIBPQ_INCLUDE_DIR $ENV{LIBPQ_INCLUDE_DIR})
MESSAGE( STATUS "LIBPQ_INCLUDE_DIR:         " ${LIBPQ_INCLUDE_DIR} )
SET(LIBPQ_LIBRARY $ENV{LIBPQ_LIBRARY})
MESSAGE( STATUS "LIBPQ_LIBRARY:         " ${LIBPQ_LIBRARY} )

FIND_PATH(LIBPQ_INCLUDE_DIR libpq-fe.h)

FIND_LIBRARY(LIBPQ_LIBRARY libpq)

# handle the QUIETLY and REQUIRED arguments and set Libmysql_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBPQ DEFAULT_MSG LIBPQ_LIBRARY LIBPQ_INCLUDE_DIR)

MARK_AS_ADVANCED(LIBPQ_LIBRARY LIBPQ_INCLUDE_DIR)
