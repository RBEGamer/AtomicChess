#include <chrono>
#include <thread>

#include "SerialPort.hpp"

using namespace std::chrono_literals;
using namespace mn::CppLinuxSerial;

int main() {
    // This example relies on a serial device which echos serial data at 9600 baud, 8n1.
    std::cout << "FlowControll.cpp::main() called." << std::endl;
    SerialPort serialPort("/dev/ttySKR", BaudRate::B_115200, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE, HardwareFlowControl::ON, SoftwareFlowControl::ON);
    serialPort.SetTimeout(1000); // Block when reading for 1000ms

    serialPort.Open();

    std::this_thread::sleep_for(100ms);

    std::thread t1([&]() {
        // Do Something
        for (int x = 0; x < 10; x++) {
            // std::this_thread::sleep_for(100ms);
            std::cout << "Reading" << std::endl;
            std::string readData;
            serialPort.Read(readData);
            std::cout << "readData: " << readData << std::endl;
        }
    });

    std::thread t2([&]() {
        // Do Something
        std::this_thread::sleep_for(1000ms);
        for (int x = 0; x < 10; x++) {

            serialPort.Write("G1 X10\n");
            std::this_thread::sleep_for(10000ms);
        }
    });

    t1.join();
    t2.join();

    serialPort.Close();
    return 0;
}