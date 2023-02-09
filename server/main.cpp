#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h> 
using namespace std;

void error(const char* issue, const int exit_code = 1)
{
	cerr<<issue<<endl;
	exit(exit_code);
}

void getting_sum(int fd)
{
	char buff[256];
	char ex[] = "n";
	bool flag = true;
	while(flag)
	{
		int rd = read(fd, buff, 256);
		if(rd == -1)
			error("Ошибка чтения принятого сообщения", 15);
		if(strncmp(ex, buff, 1) != 0)
		{
			int sum = atoi(buff);
			if(sum > 99 && sum % 32 == 0)
			{
				cout<<"Данные получены"<<endl;
			}
			else if(sum <= 99 || sum % 32 != 0)
			{
				cout<<"Ошибка получения данных"<<endl;
			}
		}
		else 
		{
			cout<<"Конец работы сервера"<<endl;
			flag = false;
		}
		
	}
}

int main()
{
	setlocale(LC_ALL, "ru");
	
	// создание адресной структуры
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(1111);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	socklen_t addrlen = sizeof(servaddr);
	
	// создание TCP сокета
	int serv_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(serv_socket == -1)
		error("Ошибка открытия сокета", 11);
	
	// привязка сокета к адресу к заданному ip
	int rc = bind(serv_socket, (sockaddr*) &servaddr, sizeof(servaddr));
	if(rc == -1) 
	{
		close(serv_socket);
		error("Ошибка привязки сокета", 12);
	}
	
	int res = listen(serv_socket, 5);
	if(res == -1)
	{
		close(serv_socket);
		error("Ошибка прослушивания", 13);
	}
	
	int fd = accept(serv_socket, (sockaddr*) &servaddr ,&addrlen);
	if(fd == -1)
	{
		close(serv_socket);
		error("Ошибка приёма информации", 14);
	}
	
	getting_sum(fd);
		
	
	close(serv_socket);
	return 0;
}