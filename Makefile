########################################################################################
#   Copyright 2016 Shidfar Hodizoda                                                    #
#   The source code contained or described herein and all documents related to the     #
#   source code ("Material") are owned by Shidfar Hodizoda.                            #
#   The Material contains trade secrets and proprietary and confidential information   #
#   The Material is protected by worldwide copyright and trade secret laws and         #
#   treaty provisions. No part of Material may be used, copied, reproduced,            #
#   modified, published, uploaded, posted, transmitted, distributed, or disclosed      #
#   in any way without prior express written permission by Shidfar Hodizoda.           #
#                                                                                      #
#   No license under any patent, copyright, trade secret or other intellectual         #
#   property right is granted to or conferred upon you by disclosure or delivery of    #
#   the Materials, either expressly, by implication, inducement, estoppel              #
#   or otherwise. Any license under such intellectual property rights must be          #
#   express and approved by Shidfar Hodizoda in writing.                               #
#                                                                                      #
########################################################################################

####################################################
#        Setting up the required flags             #
####################################################
CFLAGS = -O2 -fPIC -ggdb -I. -DPOSIX

.cpp.o: ${CXX} $(CFLAGS) -c $< -o $@


###########################################
#        Main Application Section         #
###########################################
LDFLAGS = -lmosquittopp -ljsoncpp -lpthread -lcurl `mysql_config --cflags --libs`
SOURCES = main.cpp ServerEngine.cpp MqttClient.cpp MySqlAgent.cpp HttpServer.cpp
EXECUTABLE = ServerEgine
OBJECTS = $(SOURCES:.cpp=.o)
$(EXECUTABLE): $(OBJECTS) $(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#####################################
#        Clean Up Section           #
#####################################

clean: rm -f *.o rm -f `find . -name *.o` rm -f *.so rm -f $(EXECUTABLE) rm -f sql

all: $(EXECUTABLE)

remake: clean all