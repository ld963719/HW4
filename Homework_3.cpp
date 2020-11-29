#include <iostream>
#include<fstream>
#include <vector>
#include <algorithm>
#include <omp.h>
using namespace std;

// Создание двух глобальных переменных: номера выполняемой задачи и флага для определения выполнения максимального количества задач.
int task_number = 0;
int flag = 0;

/// <summary>
/// Метод записывает идентификаторы книг в отдельный двумерный массив, добавляя к каждой книге информацию о том, какой по счёту книгой с таким идентификатором она является.
/// </summary>
/// <param name="book_ids">Первоначальный массив идентификаторов.</param>
/// <returns>Новый массив идентификаторов с учётом количества повторов.</returns>
vector<vector<int>> findSameIds(vector<int> book_ids)
{
	vector<vector<int>> result;
	int count_of_same = 1;
	result.push_back(vector<int>());
	result[0] = { book_ids[0], 1 };
	for (int i = 1; i < book_ids.size(); i++)
	{
		if (book_ids[i] == book_ids[i - 1])
		{
			count_of_same += 1;
		}
		else
		{
			count_of_same = 1;
		}
		result.push_back(vector<int>());
		result[i] = { book_ids[i], count_of_same };
	}

	return result;
}

/// <summary>
/// Метод, выполняющий запись информации об отдельной книге.
/// </summary>
/// <param name="max_task_number">Максимальное число задач.</param>
/// <param name="thread_number">Номер текущего потока.</param>
/// <param name="library_list">Массив идентификаторов книг.</param>
/// <param name="books">Массив книг.</param>
void write(int max_task_number, int thread_number, vector <vector<int>> library_list, vector <vector<int>> books)
{
	// Выполняется до тех пор, пока флаг не станет равным 1.
	while (flag == 0)
	{
		// Использум критическую секцию для того, чтобы доступ к номеру текущей задачи и к каталогу в определённый момент времени имел только один поток.
		#pragma omp critical
		if (flag != 1)
		{
			// Находим идентификатор книги, которую надо внести в каталог и номер идентификатора, который надо выбрать среди всех найденных идентификаторов.
			int position = library_list[task_number][0];
			int id_number = library_list[task_number][1];
			task_number++;


			// Ищем в массиве книг нужный идентификатор и записываем информацию в каталог (выводим на экран).
			for (int j = 0; j < books.size(); j++)
			{
				if (books[j][0] == position)
				{
					if (id_number == 1)
					{
						cout << "Book_id: " << books[j][0] << " Row: " << books[j][1] << " Bookcase: " << books[j][2] << " Book number: " << books[j][3] << " Thread number: " << thread_number << endl;
						break;
					}
					else
					{
						id_number--;
					}
				}
			}

			// Если номер задачи равен максимальному, флаг равен 1 (все задачи выполнены).
			if (task_number == max_task_number)
			{
				flag = 1;
			}
		}

	}
}

int main()
{

	// Ввод, проверка на корректность ввода и повтор ввода в случае получения некорректных данных.
	int correct_input = 0;
	int M = 0;
	int N = 0;
	int K = 0;
	string input_1, input_2, input_3;
	while (correct_input == 0)
	{
		cout << "Enter M, N, K:" << endl;
		cin >> input_1 >> input_2 >> input_3;

		M = atoi(input_1.c_str());

		N = atoi(input_2.c_str());

		K = atoi(input_3.c_str());

		if (M < 1 | M>100 | N < 1 | N>100 | K < 1 | K>100)
		{
			cout << "Wrong input! M, N, K should be integer numbers from 1 to 100. Try it again, please" << endl;
		}
		else
		{
			correct_input = 1;
		}
	}
	cout << "Starting list of books: " << endl;

	// Создаём векторы для хранения информации о книгах и их идентификаторах.
	vector <vector<int>> books;
	vector <int> book_ids;

	// Заполнение массивов книгами с псевдослучайными идентификаторами, двигаясь по рядам и шкафам от первого к последним.
	for (int i = 0; i < M * N * K; i++)
	{
		int book_id = rand() % 100 + 1;
		book_ids.push_back(book_id);
		books.push_back(vector <int>());
		books[i] = { book_id, i / (N * K) + 1, (i / K) % N + 1, i % K + 1 };
	}

	// Сортировка идентификаторов книг и создание вектора, содержащего информацию об идентификаторах с учётом их повторов.
	sort(book_ids.begin(), book_ids.end());
	vector <vector<int>> library_list = findSameIds(book_ids);

	// Вывод исходных данных о расположении книг.
	for (int i = 0; i < M * N * K; i++)
	{
		cout << "Book_id: " << books[i][0] << " Row: " << books[i][1] << " Bookcase: " << books[i][2] << " Book_number: " << books[i][3] << endl;
	}

	// Начало вывода информации о восстановленном каталоге.
	cout << endl;
	cout << "Catalog after the inventory: " << endl;

	// Устанавливаем число потоков, равное N*M*K.
	omp_set_num_threads(N * M * K);
	while (flag == 0)
	{
		// Распараллеливаем вычисления в цикле.
		#pragma omp parallel for
		for (int i = 0; i < N * M * K; i++)
		{
			if (flag == 1)
			{
				break;
			}

			// Вызов метода, записывающего в каталог информацию об отдельной книге.
			write(N * M * K, omp_get_thread_num() + 1, library_list, books);
			if (flag == 1)
			{
				break;
			}
		}
	}
}
