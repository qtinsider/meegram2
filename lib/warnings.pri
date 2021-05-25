# Define warning flags for MeeGram

QMAKE_CXXFLAGS_WARN_ON  = -Wall
QMAKE_CXXFLAGS_WARN_ON += -Werror

QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-enum-enum-conversion
QMAKE_CXXFLAGS_WARN_ON += -Wno-literal-suffix
QMAKE_CXXFLAGS_WARN_ON += -Wno-register
QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-local-typedefs
QMAKE_CXXFLAGS_WARN_ON += -Wno-zero-as-null-pointer-constant
