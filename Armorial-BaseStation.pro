TEMPLATE = app
DESTDIR  = ../bin
TARGET   = Armorial-BaseStation
VERSION  = 1.0.0

# Temporary dirs
OBJECTS_DIR = tmp/obj
MOC_DIR = tmp/moc
UI_DIR = tmp/moc
RCC_DIR = tmp/rc

CONFIG += c++17 console
CONFIG -= app_bundle
QT += core network serialport

DEFINES += QT_DEPRECATED_WARNINGS
LIBS += -lQt5Core -lprotobuf -lgrpc -lgrpc++ -lGLU -lfmt -lArmorial

system(echo "Generating service GRPC headers" && cd include/proto/services && protoc --grpc_out=../ --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` *.proto && cd ../../..)
system(echo "Generating service proto headers" && cd include/proto/services && protoc --cpp_out=../ *.proto && cd ../../..)
system(echo "Generating simulation proto headers" && cd include/proto/simulation && protoc --cpp_out=../ *.proto && cd ../../..)

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APP_NAME=\\\"$$TARGET\\\"
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += PROJECT_PATH=\\\"$${PWD}\\\"

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        include/proto/basestationservice.grpc.pb.cc \
        include/proto/basestationservice.pb.cc \
        include/proto/command.pb.cc \
        include/proto/common.pb.cc \
        include/proto/messages.grpc.pb.cc \
        include/proto/messages.pb.cc \
        include/proto/packet.pb.cc \
        include/proto/replacement.pb.cc \
        include/proto/ssl_gc_common.pb.cc \
        include/proto/ssl_simulation_config.pb.cc \
        include/proto/ssl_simulation_control.pb.cc \
        include/proto/ssl_simulation_error.pb.cc \
        include/proto/ssl_simulation_robot_control.pb.cc \
        include/proto/ssl_simulation_robot_feedback.pb.cc \
        include/proto/ssl_simulation_synchronous.pb.cc \
        include/proto/ssl_vision_detection.pb.cc \
        include/proto/ssl_vision_geometry.pb.cc \
        include/proto/visionservice.grpc.pb.cc \
        include/proto/visionservice.pb.cc \
        main.cpp \
        src/basestations/basestation.cpp \
        src/basestations/real/realbasestation.cpp \
        src/basestations/sim/simbasestation.cpp \
        src/manager/basestationmanager.cpp \
        src/service/basestationservice.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/proto/basestationservice.grpc.pb.h \
    include/proto/basestationservice.pb.h \
    include/proto/command.pb.h \
    include/proto/common.pb.h \
    include/proto/messages.grpc.pb.h \
    include/proto/messages.pb.h \
    include/proto/packet.pb.h \
    include/proto/replacement.pb.h \
    include/proto/ssl_gc_common.pb.h \
    include/proto/ssl_simulation_config.pb.h \
    include/proto/ssl_simulation_control.pb.h \
    include/proto/ssl_simulation_error.pb.h \
    include/proto/ssl_simulation_robot_control.pb.h \
    include/proto/ssl_simulation_robot_feedback.pb.h \
    include/proto/ssl_simulation_synchronous.pb.h \
    include/proto/ssl_vision_detection.pb.h \
    include/proto/ssl_vision_geometry.pb.h \
    include/proto/visionservice.grpc.pb.h \
    include/proto/visionservice.pb.h \
    src/basestations/basestation.h \
    src/basestations/real/realbasestation.h \
    src/basestations/sim/simbasestation.h \
    src/manager/basestationmanager.h \
    src/service/basestationservice.h

