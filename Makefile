

INC_DIR=./include
SRC_DIR=./src
BUILD_DIR=./build
APPNAME=b4mesh
GREEN_BUILDROOT=../greencom/greensoft-sdk-2021-01-25-14ae3/
JSON_INC=./dependencies/
HTTP_SERVER_INC=./dependencies/b4mesh_http/includes/b4mesh_http

DEPS=$(wildcard ${INC_DIR}/*.hpp) 
SRC=$(wildcard ${SRC_DIR}/*.cpp)

GLUE_FILES=$(BUILD_DIR)/consensus_glue.h \
		   $(BUILD_DIR)/state_glue.h

OBJ=$(patsubst ${SRC_DIR}/%.cpp,${BUILD_DIR}/%.o, ${SRC}) 

DEBUG ?=0
ifeq ($(DEBUG),1)
OPTIM= -g3 
else
OPTIM= -O3 
endif

LOCAL ?=0
ifeq ($(LOCAL),1)
CC=g++ 
GREEN_PATH=${GREEN_BUILDROOT}output/host
GREEN_INCLUDE=${GREEN_PATH}/usr/include/dbus-c++-1/
GREEN_LIB=${GREEN_PATH}/usr/lib/
CCPFLAGS=${OPTIM} -std=c++17 -Wall
LDFLAGS= -lpthread -L${GREEN_LIB} -ldbus-c++-1 -ldbus-c++-asio-1
BUILD_DIR=./build_local
else
CC=arm-linux-g++
GREEN_PATH=${GREEN_BUILDROOT}output/target
GREEN_INCLUDE=${GREEN_PATH}/usr/include/dbus-c++-1/
GREEN_LIB=${GREEN_PATH}/usr/lib/
CCPFLAGS=${OPTIM} -fcompare-debug-second -std=c++17 -Wall
LDFLAGS= -lpthread -L${GREEN_LIB} -ldbus-c++-1 -ldbus-c++-asio-1
endif

CCPFLAGS+= -I$(BUILD_DIR) -I${INC_DIR} -I${GREEN_INCLUDE} -I${HTTP_SERVER_INC} -I${JSON_INC}

$(BUILD_DIR)/%.o: ${SRC_DIR}/%.cpp $(DEPS) ${GLUE_FILES}
	$(CC) $(CCPFLAGS) -c -o $@ $< 

$(BUILD_DIR)/${APPNAME}: $(OBJ) $(OBJ_BG) 
	$(CC) $(CCPFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD_DIR)/%_glue.h: ${INC_DIR}/%.xml
	${GREEN_PATH}/../host/bin/dbusxx-xml2cpp $^ --proxy=$@

all: $(BUILD_DIR)/${APPNAME} 

clean:
	rm -f $(BUILD_DIR)/*.o  $(BUILD_DIR)/${APPNAME} 

send1:
	sshpass -p 'b4meshroot' scp $(BUILD_DIR)/${APPNAME} default@${IP}:/var/persistent-data/b4mesh

sendall:
	./script/sendAll $(BUILD_DIR)/${APPNAME}

recoverResult1:
	sshpass -p 'b4meshroot' scp default@${IP}:/var/persistent-data/b4mesh/blockgraph* Results

recoverResultAll:
	./script/recoverResult 



clean_glue_files:
	rm -f ${GLUE_FILES}

build_glue_files: ${GLUE_FILES}

patch-nginx: 
	sshpass -p 'b4meshroot' scp patch/nginx.patch default@${IP}:/var/persistent-data/b4mesh
	sshpass -p 'b4meshroot' ssh default@${IP} sudo -S rwdo patch -N -p0 -i /var/persistent-data/b4mesh/nginx.patch
	sshpass -p 'b4meshroot' ssh default@${IP} sudo -S nginx -s reload


.PHONY: clean all send1 sendall clean_glue_files build_glue_files recoverResult1 recoverResultAll

