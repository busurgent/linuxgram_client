#include <boost/asio.hpp>
#include <cdk.h>
#include <cdk/cdkscreen.h>
#include <cdk/entry.h>
#include <cdk/label.h>
#include <chrono>
#include <iostream>
#include <ncurses.h>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

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

void to_send(tcp::socket socket, std::string message) {
  // nlohmann::json j;
  // j["type"] = "message";
  char reply[1024];
  std::size_t reply_length;
  while (true) {
    // std::getline(std::cin, message);
    //  j["message"] = message;
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

int registration(char *username, char *pass, tcp::socket socket) {
  nlohmann::json query;
  query["type"] = "register";
  query["username"] = username;
  query["password"] = pass;
  boost::asio::write(socket, boost::asio::buffer(query.dump() + '\0'));
  //std::this_thread::sleep_for(std::chrono::milliseconds(500));
  char reply[1024];
  size_t reply_length = socket.read_some(boost::asio::buffer(reply));
  nlohmann::json otvet{nlohmann::json::parse(reply)};
  std::cout << std::string(reply, reply_length) << std::endl;
  std::string token{otvet["token"]};
  std::string status{otvet["status"]};
  /* FILE *f = fopen("answer", "w");
   fprintf(f, "%s\n", reply);
   fclose(f);*/
  if (status == "ok")
    return 1;
  else
    return 0;
}

void login_menu(CDKSCREEN *screen, tcp::socket socket) {
  CDKENTRY *login_entry =
      newCDKEntry(screen, CENTER, CENTER, "</B>Username<!B>", "", A_NORMAL, ' ',
                  vMIXED, 21, 3, 20, FALSE, FALSE);
  CDKENTRY *password_entry =
      newCDKEntry(screen, CENTER, CENTER, "</B>Password<!B>", "", A_NORMAL, ' ',
                  vHMIXED, 21, 3, 20, FALSE, FALSE);
  moveCDKEntry(login_entry, -3, 0, TRUE, TRUE);
  moveCDKEntry(password_entry, 3, 0, TRUE, TRUE);
  const char *s[2] = {"<C></31>Wrong username or password<!31>", NULL};
  CDKLABEL *label =
      newCDKLabel(screen, CENTER, BOTTOM, (char **)s, 1, false, false);
  setCDKEntryHiddenChar(password_entry, '*');
  while (1) {
    drawCDKEntry(login_entry, TRUE);
    char *login = activateCDKEntry(login_entry, NULL);
    drawCDKEntry(password_entry, FALSE);
    char *pass = activateCDKEntry(password_entry, NULL);
    refreshCDKScreen(screen);
    // std::thread send_thread(registration, login, pass, std::move(socket));
    // send_thread.join();
    if (registration(login, pass, std::move(socket)))
      break;
    drawCDKLabel(label, FALSE);
    refreshCDKScreen(screen);
  }
  eraseCDKEntry(login_entry);
  eraseCDKEntry(password_entry);
  eraseCDKLabel(label);
  mvprintw(0, 0, "SUCCESS");
  refreshCDKScreen(screen);
  getch();
  destroyCDKEntry(login_entry);
  destroyCDKEntry(password_entry);
  destroyCDKLabel(label);
}

int main() {
  boost::asio::io_context io_context;
  tcp::socket socket(io_context);
  tcp::resolver resolver(io_context);
  boost::asio::connect(socket, resolver.resolve("77.51.222.0", "53861"));

  CDKSCREEN *screen;
  WINDOW *win = initscr();
  screen = initCDKScreen(win);
  login_menu(screen, std::move(socket));
  // main_window(screen);
  eraseCDKScreen(screen);
  destroyCDKScreen(screen);
  endCDK();

  // int id = enter_page(socket);

  // std::thread send_thread(registration, "viktor", "sukanaxov", std::move(socket));
  // std::thread accept_thread(to_accept, std::move(socket));
  // send_thread.join();
  // accept_thread.join();
  return 0;
}
