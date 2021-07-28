/**
 * @file sensor_client.cc
 * @author Jaikrishna Nemallapudi (nemallapudi.jaikrishna@intel.com)
 * @brief
 * @version 1.0
 * @date 2021-07-27
 *
 * Copyright (c) 2021 Intel Corporation

 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <array>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <map>
#include <mutex>
#include <iterator>
#include <string>
#include <thread>
#include "sensor_interface.h"

using namespace std::chrono_literals;
using namespace vhal::client;
using namespace std;

map<int32_t, int32_t> mSensorMap; //<sensorType, Enable/Disable Flag>
mutex sensorMapMutex; // lock to guard mSensorMap

int main(int argc, char** argv)
{
    int instanceId = 0;
    std::unique_ptr<SensorInterface> sensorHALIface;
    SensorInterface::SensorDataPacket event;

    /* Create sensor Interface with LibVHAL */
    sensorHALIface = make_unique<SensorInterface>(instanceId);

    /* Register a callback to receive VHAL sensor config packets */
    sensorHALIface->RegisterCallback([&]
            (const SensorInterface::CtrlPacket& ctrlPkt) {
        std::unique_lock<std::mutex> lck(sensorMapMutex);
        mSensorMap.insert({ctrlPkt.type, ctrlPkt.enabled});
    });

    /* Start a thread to send dummy sensor data for ACCELEROMETER sensor*/
    thread sensor_thread;
    sensor_thread = thread([&event, &sensorHALIface] () {
        while (true) {
            for (auto it : mSensorMap) {
                if (it.first == SENSOR_TYPE_ACCELEROMETER) {
                    if (it.second) {
                        event.type = SENSOR_TYPE_ACCELEROMETER;
                        event.fdata[0] = 1, event.fdata[1] = 2;
                        event.fdata[3] = 3;
                        struct timespec ts;
                        clock_gettime(CLOCK_BOOTTIME, &ts);
                        event.timestamp_ns = (int64_t)ts.tv_sec * 1000000000 + ts.tv_nsec;
                        sensorHALIface->SendDataPacket(&event);
                    }
                }
            }
        }
    });
    sensor_thread.join();

    return 0;
}
