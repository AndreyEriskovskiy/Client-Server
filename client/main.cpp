#include <iostream>
#include <string>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <vector>
#include <algorithm>
#include <fstream>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <unistd.h> 

using namespace std;

// Функция проверки строки на выполнение определенных требований к её длине и её состава

bool valid_stroka(string stroka) 
{
	int count = 0;
	int len = stroka.length();
	for (auto c: stroka)
	{
		if (isdigit(c))
			count++;
		
	}
	if (count == len and len <= 64)
		return true;
	
	else
		return false;
}

// Функция, в соотвествии с которой сортируются символы строки

bool cmp(char a, char b) {
	return a > b;
}

void send_sum(int cl_socket, int sum)
{
	char buff[256];
	string data = to_string(sum);
	strcpy(buff, data.c_str());
	write(cl_socket, buff, 256);
}

void send_end(int cl_socket, string s)
{
	char buff[256];
	strcpy(buff, s.c_str());
	write(cl_socket, buff, 256);
}

void potok_1() 
{
	
	vector <char> mylist;
	string stroka;
	bool flag = true;
	while (flag)
	{
		cout << "Введите строку, содержащую только цифры и размером не более 64 символов: ";
		getline(cin, stroka);
		cout << endl;
		if (!valid_stroka(stroka)) {
			cerr << "Строка либо длиннее 64 символов, либо в ней присутствуют не только цифры" << endl;
			cout << endl;
			continue;
		}
		else
			flag = false;
	}
		
	// посимвольная запись строки в контейнер	
	for (auto c: stroka)
	{
		mylist.push_back(c);
	}
	
	// сортировка символов строки по убыванию
	sort(mylist.begin(), mylist.end(), cmp);
	stroka.clear();
	
	// запись символов в строку в отсортированном порядке
	for (auto c : mylist)
	{
		stroka.push_back(c);
	}

	string stroka2;
	string kb = "KB";
	
	// замена четных цифр в строке сочетанием символов "KB"
	for (auto c: stroka)
	{
		int tmp = c - '0';
		if (tmp % 2 == 0)
			stroka2 += kb;
		else
			stroka2 += c;
	}
	
	// запись модифицированной строки в буфер
	ofstream fout("./buffer.txt");
	fout << stroka2;
	fout.close();
		
}

void potok_2(int cl_socket)
{
	string stroka;
	int sum = 0;
	ifstream fin("./buffer.txt");
	
	// считывание строки из буфера
	fin >> stroka;
	fin.close();
	ofstream clearing("./buffer.txt", ios::trunc);
	clearing.close();
	cout << "Модифицированная строка: " << stroka << endl;
	cout<<endl;
	
	// вычисление суммы цифр в строке
	for (auto c: stroka)
	{
		if (isdigit(c))
			sum += (c - '0');
	}
	cout<<"Сумма: "<<sum<<endl;
	send_sum(cl_socket, sum);
}

void error(const char* issue, const int exit_code = 1)
{
	cerr<<issue<<endl;
	exit(exit_code);
}
	
int main()
{
	setlocale(LC_ALL, "ru");

	
	// структура с адресом программы-сервера
	struct sockaddr_in servaddr;
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(1111);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int cl_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(cl_socket == -1)
		error("Ошибка открытия сокета", 11);
		
		
	
	int rc = connect(cl_socket, (sockaddr*) &servaddr, sizeof(servaddr));
	if(rc == -1)
	{
		close(cl_socket);
		error("Ошибка подключения сокета к программе-серверу", 13);
	
	}
	
	ofstream file = ofstream("buffer.txt");
	bool flag = true;
	while (flag)
	{
		// выполнение двух функций в разных потоках
		thread t1(potok_1);
		t1.join();
		thread t2(potok_2, cl_socket);
		t2.join();
		cout << endl;
		
		cout << "Если вы хотите повторить операцию ещё раз, нажмите 'y' (yes), чтобы завершить программу, нажмите 'n' (no): ";
		string symbol;
		getline(cin, symbol);
		cout << endl;
		if (symbol == "y")
			continue;
		else if (symbol == "n") {
			send_end(cl_socket, symbol);
			flag = false;
			cout << "Программа завершена..." << endl;
		}
		else
		{
			bool t = true;
			// цикл для проверки нужного ввода от пользователя
			while (t)
			{
				cerr << "Вы ввели некорректный символ или последовательность символов, введите (y - yes, n - no): ";
				string s;
				getline(cin, s);
				cout << endl;
				if (s == "y")
				{
					t = false;
				}
				else if (s == "n")
				{
					send_end(cl_socket, s);
					t = false;
					cout << "Программа завершена..." << endl;
					exit(0);
				}
				else
					continue;
			}

		}
	}
	close(cl_socket);
	return 0;
}