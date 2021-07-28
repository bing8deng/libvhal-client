# VHAL Client library 

libvhal-client is a library written in C++17 for Camera, Audio and Sensor modules. Currently only Camera is
supported. Using this library, a client can interact with Camera Vhal without worrying about socket connection,
data structure, command details. Instead, the library exposes simple and powerful API that simplifies the life of
VHAL client modules such as CG-Proxy, Streamer, etc. Any internal changes in the Camera VHAL and the library
is abstracted and hence it won’t affect client code. New features or improvements can be provided via APIs
rather than new socket commands, and other details.

## Build steps

```
shakthi@mypc build (master) mkdir build
shakthi@mypc build (master) cd build
shakthi@mypc build (master) $ cmake ..

-- The CXX compiler identification is GNU 7.5.0
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Project name: vhal-client
-- Looking for C++ include pthread.h
-- Looking for C++ include pthread.h - found
-- Looking for pthread_create
-- Looking for pthread_create - not found
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - found
-- Found Threads: TRUE
-- Configuring done
-- Generating done
-- Build files have been written to: /home/shakthi/gitlab/libvhal-client/build

shakthi@Lenovo-Yoga-C740-14IML build (master) $ cmake --build .

Scanning dependencies of target vhal-client
[ 14%] Building CXX object source/CMakeFiles/vhal-client.dir/unix_stream_socket_client.cc.o
[ 28%] Building CXX object source/CMakeFiles/vhal-client.dir/vhal_video_sink.cc.o
[ 42%] Linking CXX shared library ../../libs/linux/libvhal-client.so
[ 42%] Built target vhal-client
Scanning dependencies of target camera_socket_client
[ 57%] Building CXX object examples/CMakeFiles/camera_socket_client.dir/camera_socket_client.cc.o
[ 71%] Linking CXX executable ../../bins/linux/camera_socket_client
[ 71%] Built target camera_socket_client
Scanning dependencies of target camera_client
[ 85%] Building CXX object examples/CMakeFiles/camera_client.dir/camera_client.cc.o
[100%] Linking CXX executable ../../bins/linux/camera_client
[100%] Built target camera_client
```
```
### To install/copy libvhal-clinet.so in to particular directory
libvhal-client$mkdir build
libvhal-client$cd build
libvhal-client$cmake .. -DCMAKE_INSTALL_PREFIX=<Install folder name>
libvhal-client$make
libvhal-client$make install
```

## Camera

Camera VHal runs socket server (UNIX, VSock are supported). VHAL Client library shall connect to socket server path or address/port endpoint.

### Architecture
![image](https://github.com/intel-sandbox/libvhal-client/assets/26615772/66a89e80-d316-11eb-814f-c4e75902441c)


### Camera API to interact with VHal
Camera VHal client might do following steps.

Create a socket client (say, UNIX):
```cpp
auto unix_sock_client = make_unique<vhal::client::UnixStreamSocketClient>(move(socket_path));
```

Handover socket client to `vhal::client::VideoSink` that talks to Camera VHal.
```cpp
vhal::client::VideoSink video_sink(move(unix_sock_client));
```

Register with `vhal::client::VideoSink` for Camera open/close callbacks as below:

![image](https://github.com/intel-sandbox/libvhal-client/assets/26615772/2269ce00-d317-11eb-89fe-94e28ae73eb1)

## Audio
TODO

## Sensor
Sensor VHAL runs socket server for UNIX and INET domains. VHAL Client library(libvhal-client.so) shall connect to socket server path.

### Architecture
Here shown the E2E block diagram of sensors and API sequence to communicate with libVHAL Client

![image](docs/sensor-e2e-block-diagram.JPG)

![image](docs/sensor-api-sequence-flow.JPG)

### Steps to interact with libVHAL-Client from Streamer

1. Create an object to libVHAL's SensorInterface class and pass client's instance ID.
Then libVHAL will prepare and connect to Sensor server using UNIX domain sockets.
```cpp
sensorHALIface = make_unique<SensorInterface>(mInstanceId);
```

2. Register a callback func to get sensor control packets upon received from Sensor VHAL server. CtrlPacket contains respective sensor's enable/disable info and sampling period.
```cpp
    sensorHALIface->RegisterCallback([&](const SensorInterface::CtrlPacket& ctrlPkt){});
```

3. Use below api to know list of sensors supported by libVHAL-client or AIC. This API returns supported sensor list in bitmap format.
```cpp
uint64_t GetSupportedSensorList();
```

4. Fill the available sensor data into structure sensorDataPacket. Send the data to libVHAL-client like below
```cpp
sensorHALIface->SendDataPacket(&event);
```

Example implementation to interact with libVHAL-client is present in examples/sensor_client.cc




