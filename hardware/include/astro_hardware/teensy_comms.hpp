#ifndef ASTRO_HARDWARE_TEENSY_COMMS_HPP
#define ASTRO_HARDWARE_TEENSY_COMMS_HPP

// #include <cstring>
#include <sstream>
// #include <cstdlib>
#include <libserial/SerialPort.h>
#include <iostream>
#include "rclcpp/rclcpp.hpp"


LibSerial::BaudRate convert_baud_rate(int baud_rate)
{
  // Just handle some common baud rates
  switch (baud_rate)
  {
    case 1200: return LibSerial::BaudRate::BAUD_1200;
    case 1800: return LibSerial::BaudRate::BAUD_1800;
    case 2400: return LibSerial::BaudRate::BAUD_2400;
    case 4800: return LibSerial::BaudRate::BAUD_4800;
    case 9600: return LibSerial::BaudRate::BAUD_9600;
    case 19200: return LibSerial::BaudRate::BAUD_19200;
    case 38400: return LibSerial::BaudRate::BAUD_38400;
    case 57600: return LibSerial::BaudRate::BAUD_57600;
    case 115200: return LibSerial::BaudRate::BAUD_115200;
    case 230400: return LibSerial::BaudRate::BAUD_230400;
    default:
      std::cout << "Error! Baud rate " << baud_rate << " not supported! Default to 57600" << std::endl;
      return LibSerial::BaudRate::BAUD_57600;
  }
}

class TeensyComms
{

public:

  TeensyComms() = default;

  void connect(const std::string &serial_device, int32_t baud_rate, int32_t timeout_ms)
  {  
    timeout_ms_ = timeout_ms;
    serial_conn_.Open(serial_device);
    serial_conn_.SetBaudRate(convert_baud_rate(baud_rate));
  }

  void disconnect()
  {
    serial_conn_.Close();
  }

  bool connected() const
  {
    return serial_conn_.IsOpen();
  }



  std::string read_msg()
  {
    serial_conn_.FlushIOBuffers(); // Just in case

    std::string response = "";
    try
    {
      // Responses end with \r\n so we will read up to (and including) the \n.
      serial_conn_.ReadLine(response, '\n', timeout_ms_);
    }
    catch (const LibSerial::ReadTimeout&)
    {
        std::cerr << "The ReadByte() call has timed out." << std::endl ;
    }


    return response;
  }


  void read_values(double &pos_l, double &pos_r, double &vel_l, double &vel_r, double &eff_l, double &eff_r, double &curr)
  {

   std::string response = read_msg();

    std::stringstream ss(response);

    ss >> pos_l >> pos_r >> vel_l >> vel_r >> eff_l >> eff_r >> curr;

    // If no full line received → DO NOTHING (keep old values)
  }
  void set_motor_values(double vel_l, double vel_r)
  {
    std::stringstream ss;

    ss << "v " << vel_l << " " << vel_r << "\n";

    std::string cmd = ss.str();

    serial_conn_.Write(cmd);  // NON-BLOCKING


    RCLCPP_INFO(rclcpp::get_logger("AstroHardware"),
            "Sending to Teensy: %s",
            cmd.c_str());

  }
  void set_pid_values(int k_p, int k_d, int k_i, int k_o)
  {
    std::stringstream ss;
    ss << "u " << k_p << ":" << k_d << ":" << k_i << ":" << k_o << "\r";
    serial_conn_.Write(ss.str()); 
  }

private:
    LibSerial::SerialPort serial_conn_;
    int timeout_ms_;
    std::string rx_buffer_;
    int max_bytes = 64;
};

#endif // ASTRO_HARDWARE_TEENSY_COMMS_HPP