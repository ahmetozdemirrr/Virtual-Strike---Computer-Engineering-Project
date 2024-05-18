#include <thread>
#include <chrono>
#include <cstring>
#include <fcntl.h>    /* File control definitions */
#include <iostream>
#include <unistd.h>   /* UNIX standard function definitions */
#include <termios.h>  /* POSIX terminal control definitions */
#include <wiringPi.h>
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#define X_PIN 3
#define Y_PIN 2
#define SWITCH_PIN 12

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;

int main(void) 
{
    try 
    {
        int serial_port = open("/dev/ttyUSB0", O_RDWR);

        if (serial_port < 0) 
        {
            std::cerr << "Error opening serial port" << std::endl;
            return EXIT_FAILURE;
        }

        /* Configure termios structure */
        struct termios tty;

        if(tcgetattr(serial_port, &tty) != 0) 
        {
            std::cerr << "Error from tcgetattr: " << strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }

        /* Set baud rate */
        cfsetispeed(&tty, B9600);
        cfsetospeed(&tty, B9600);

        /* Apply settings */
        if (tcsetattr(serial_port, TCSANOW, &tty) != 0) 
        {
            std::cerr << "Error from tcsetattr: " << strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }

        std::string const host = "45.55.49.156";
        std::string const port = "8080";
        net::io_context ioc;

        /* DNS resolution */
        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(host, port);

        /* Connect to WebSocket */
        websocket::stream<tcp::socket> ws(ioc);
        net::connect(ws.next_layer(), results.begin(), results.end());
        ws.handshake(host, "/");

        if (wiringPiSetup() == -1) 
        {
            std::cerr << "WiringPi initialization failed!" << std::endl;
            return 1;
        }
        pinMode(X_PIN, INPUT);
        pinMode(Y_PIN, INPUT);
        pinMode(SWITCH_PIN, INPUT);

        char read_buf[256];
        std::string message;

        while (true) 
        {
            memset(read_buf, 0, sizeof(read_buf));
            int num_bytes = read(serial_port, read_buf, sizeof(read_buf) - 1);  /* Leave space for null terminator */

            if (num_bytes > 0) 
            {
                read_buf[num_bytes] = '\0';  /* Null-terminate the string */
                std::cout << read_buf;
                message = std::string(read_buf);

                try 
                {
                    ws.write(net::buffer(message));
                } 

                catch(const websocket::error& e) 
                {
                    std::cerr << "WebSocket error: " << e.what() << std::endl;
                } 

                catch(const std::exception& e) 
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        close(serial_port);
    } 
    catch (const std::exception &e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
