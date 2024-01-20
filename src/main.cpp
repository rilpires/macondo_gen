
#include "api.h"
#include <unistd.h>
#include <arpa/inet.h>
#include <csignal>

int server_fd;

int main(int argc, char **argv)
{

  bool is_tcp = false;
  int port = 7776;

  for (int i = 1; i < argc; i++)
  {
    std::string arg = std::string(argv[i]);
    if ((arg == "--tcp") || (arg == "-t"))
    {
      is_tcp = true;
    }
    // Arguments with parameters
    else if (i + 1 < argc)
    {
      if ((arg == "--port") || (arg == "-p"))
      {
        port = std::stoi(argv[i + 1]);
        if (port < 0)
        {
          port = 7776;
        }
        i++;
      }
    }
  }

  std::cout << "Macondogen v0.0" << std::endl;

  // hardcoding a story while we don't have a proper command line interface
  auto api = API();
  auto input_json = Utils::loadJSON("stories/politics.json");
  api.JSON(input_json.dump());

  if (is_tcp)
  {
    std::cout << "Starting TCP server on port " << port << std::endl;
    const u_int32_t buf_size = 1024 * 1024; // 1MB
    char buffer[buf_size];

    // Adress struct
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd <= 0)
    {
      std::cerr << "socket failed" << std::endl;
      exit(EXIT_FAILURE);
    }
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
      std::cerr << "bind failed" << std::endl;
      exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
      std::cerr << "listen failed" << std::endl;
      exit(EXIT_FAILURE);
    }
    auto exitLambda = [](int signum)
    { std::cout << "Exiting..." << std::endl; 
                  close(server_fd); };
    std::signal(SIGTERM, exitLambda);
    std::signal(SIGINT, exitLambda);
    std::signal(SIGQUIT, exitLambda);
    std::signal(SIGKILL, exitLambda);
    std::cout << "Successfully started TCP server" << std::endl;
    while (true)
    {
      int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
      if (new_socket < 0)
      {
        std::cerr << "accept failed" << std::endl;
        exit(EXIT_FAILURE);
      }
      std::cout << "New connection from " << inet_ntoa(address.sin_addr) << ":" << ntohs(address.sin_port) << std::endl;
      while (true)
      {
        int bytes_read = read(new_socket, buffer, buf_size);
        if (bytes_read <= 0)
        {
          std::cout << "Connection closed" << std::endl;
          break;
        }
        std::string line(buffer, bytes_read);
        line = line.substr(0, line.find_first_of('\n'));
        std::string response = api.command(line);
        write(new_socket, response.c_str(), response.size());
      }
    }
  }
  else
  {
    bool quit = false;
    api.onExit([&quit]()
               { quit = true; });
    api.onOutput([](std::string s)
                 { std::cout << s << std::endl; });

    std::cout << "Type HELP for a list of commands" << std::endl;
    while (!quit)
    {
      std::string line;
      std::cout << "> ";
      std::getline(std::cin, line);
      api << line;
    }
  }

  std::cout << "Bye!" << std::endl;

  return 0;
}