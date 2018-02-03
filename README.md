# DUAL_Qt_Demo_src

## Project Description
This Qt project is for demo VISTEKO Optical Tracker (VISTEKO OT). It is based on QTcpServer of Qt5.6 and network library

## File Description
1. myclass.ui, the ui definition file, mainly includes two categories of Widget, "Pust Button" and "Lable"
   * Push Button  
   "1. Connect Tracker" button is to start server listening from the VISTEKO OT client's connection  
   "2. Start Tracking" button is to send STRING "start" to VISTEKO OT to its enable optical tracking funtion
   "3. Stop Tracking" button is to send STRING "stop" to VISTEKO OT to disable tracking, after stop, VISTEKO OT will go to low power mode  
   * Label  
   "label_valx", "label_valy", "label_valz" and "label_vale", report the realtime measurment of origin postion from VISTEKO OT. These four labels were updated in the MyClass::readMessage() of _file_ myclass.cpp. Please note that these four label were in QString type. If users want to know the numerical value of measurment result, QString.toDouble() can be used to do this. The transformation was also implemented in the the MyClass::readMessage() of _file_ myclass.cpp.  
   
   "label_valq0", "label_valqx", "label_valqy" and "label_valqz", report the realtime measurment of the quaternion rotation from VISTEKO OT. These four labels were updated in the MyClass::readMessage() of _file_ myclass.cpp. Please note that these four label were in QString type. If users want to know the numerical value of measurment result, QString.toDouble() can be used to do this. The transformation was also implemented in the the MyClass::readMessage() of _file_ myclass.cpp.
   
1. myclass.cpp, the core function file, handles all the connection and data communication request
   * MyClass::readMessage()  
   read packet from VISTEKO OT client in realtime, then parse the data packet according to the following protocal,  
   > VISTEKO OT packet composed of "vspTool" + "x" + "y" + "z" + "rms" + "q0" + "q1" + "q2" + "q3" + "vep"  
   
   > "vspTool" is the tracking rigid body id, and currently is "2" which represent the NDI Polaris Passive Probe (8700340)  
   
   > "x","y" and "z" are the measurment of the rigid body's origin  
   
   > "rms" is the root mean square error of the measurment of the rigid body's origin  
   
   > "q0","q1","q2" and "q3" are the measurement of the rotaion quaternion of the rigid body  
   
   > HERE is a demo of VISTEKO OT packet "vspTool2x100.00y101.01z1010.10rms0.1234q0-1.0000q10.0000q20.0000q30.0000vep", which means that _vspTool_ is "2", _x_ is "100.00" (in milimeter), _y_ is "101.01", _z_ is "1010.10", _rms_ is "0.1234", _q0_ is "-1.0000", _q1_ is "0.0000", _q2_ is "0.0000", _q3_ is "0.0000". _vep_ is the tag of end of packet.  
   
   
   * MyClass::sendStartTracking()  
   send "start" string to VISTEKO OT to enable tracking
   
   * MyClass::sendStopTracking()
   send "stop" string to VISTEKO OT to disable tracking and let VISTEKO OT go into low power mode
   

## Project symbol link
1. mklink "dst" "src"       --  file link
1. mklink \D "dst" "src"    --  directory link
