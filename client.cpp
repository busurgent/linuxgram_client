#include <boost/asio.hpp>
#include <iostream>
// #include <nlohmann/json.hpp>

using boost::asio::ip::tcp;
// int enter(tcp::socket& socket)
// {
//     nlohmann::json j;
//     char reply[128];
//     int id, reply_lenght;
//     std::string input;
//     std::cout << "Enter you login\n";
//     std::cin >> input;
//     j["type"] = "registration";
//     j["command"] = "get_reg_id";
//     boost::asio::write(socket, boost::asio::buffer(j.dump()));
//     reply_lenght = socket.read_some(boost::asio::buffer(reply));
//     std::cout << std::string(reply, reply_lenght) << std::endl;

//     std::cout << "Enter your password";
//     std::cin >> input;
//     j["type"] = "registration";
//     j["command"] = "send_password";
//     boost::asio::write(socket, boost::asio::buffer(j.dump()));
//     reply_lenght = socket.read_some(boost::asio::buffer(reply));
//     std::cout << std::string(reply, reply_lenght) << std::endl;
//     return id;
// }

// int registration(tcp::socket& socket)
// {
//     int id;
//     std::cout << "Enter you new login\n";
//     // обработка запроса логина
//     std::cout << "Enter your password";
//     // обработка запроса пароля
//     return id;
// }

// int enter_page(tcp::socket& socket)
// {
//     int value, id;
//     std::cout << "1: Enter (you have an account) \n 2: Registration\n";
//     std::cin >> value;

//     if (value == 1)
//         id = enter(socket);

//     if (value == 2)
//         id = registration(socket);
//     return id;
// }

void to_send(tcp::socket socket) 
{
    // nlohmann::json j;
    // j["type"] = "message";
    std::string message;
    char reply[1024];
    std::size_t reply_length;
    while (true) {
        std::getline(std::cin, message);
        // j["message"] = message;
        boost::asio::write(socket, boost::asio::buffer(message + '\0'));
        reply_length = socket.read_some(boost::asio::buffer(reply));
        std::cout << std::string(reply, reply_length) << std::endl;
    }
}

// void to_accept(tcp::socket socket) 
// {
//     char reply[1024];
//     std::size_t reply_length;
//     while (true) {
//         reply_length = socket.read_some(boost::asio::buffer(reply));
//         std::cout << std::string(reply, reply_length) << std::endl;
//     }
// }


int main() 
{
    boost::asio::io_context io_context;
    tcp::socket socket(io_context);
    tcp::resolver resolver(io_context);
    boost::asio::connect(socket, resolver.resolve("95.72.222.82", "63861"));

    // int id = enter_page(socket);

    std::thread send_thread(to_send, std::move(socket));
    // std::thread accept_thread(to_accept, std::move(socket));
    send_thread.join();
    // accept_thread.join();
    return 0;
}