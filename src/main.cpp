#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

struct Person {
  char name[50];
  int age;
  float height;

  // Serialize the struct to a byte array
  void serialize(char *buffer) const { memcpy(buffer, this, sizeof(Person)); }

  // Deserialize from a byte array to the struct
  void deserialize(const char *buffer) { memcpy(this, buffer, sizeof(Person)); }
};

#define PORT 8080

int main() {
  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[sizeof(Person)];

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }

  // Receive Person struct from client
  recv(new_socket, buffer, sizeof(Person), 0);
  Person receivedPerson;
  receivedPerson.deserialize(buffer);

  std::cout << "Received Person:" << std::endl;
  std::cout << "Name: " << receivedPerson.name << std::endl;
  std::cout << "Age: " << receivedPerson.age << std::endl;
  std::cout << "Height: " << receivedPerson.height << std::endl;

  // Modify the received Person and send it back
  strcpy(receivedPerson.name, "Server Modified");
  receivedPerson.age += 1;
  receivedPerson.height += 0.1f;

  receivedPerson.serialize(buffer);
  send(new_socket, buffer, sizeof(Person), 0);
  std::cout << "Modified Person sent back to client" << std::endl;

  close(new_socket);
  close(server_fd);
  return 0;
}