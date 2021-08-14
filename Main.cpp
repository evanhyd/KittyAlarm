#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include <vector>
#include <ctime>

using namespace std;

class Attendance
{
	string link;
	vector<INPUT> tab_press;

public:

	void LoadInfo_(ifstream& load)
	{
		int tab_times;
		load >> this->link >> tab_times;

		ConvertToKeyCode(this->tab_press, string(tab_times, '\t') + string(2, '\n'));
	}

	void SignUp_()
	{
		//do it twice to avoid stupid problems

		int N = 2;
		while (N--)
		{
			system(("start \"\" \"" + this->link + "\"").c_str());
			Sleep(5000);
			SendInput(tab_press.size(), tab_press.data(), sizeof(INPUT));
		}
		
	}

	static void ConvertToKeyCode(vector<INPUT>& key_press, const string& input)
	{
		INPUT key;
		key.type = INPUT_KEYBOARD;
		key.ki.wScan = 0;
		key.ki.time = 0;
		key.ki.dwExtraInfo = 0;

		for (char letter : input)
		{
			letter = toupper(letter);
			switch (letter)
			{
			case '-':
				key.ki.wVk = VK_OEM_MINUS;
				break;

			case ' ':
				key.ki.wVk = VK_SPACE;
				break;

			case '\t':
				key.ki.wVk = VK_TAB;
				break;

			case '\n':
				key.ki.wVk = VK_RETURN;
				break;

			default:
				key.ki.wVk = letter;
			}

			key.ki.dwFlags = 0;
			key_press.push_back(key);
			key.ki.dwFlags = KEYEVENTF_KEYUP;
			key_press.push_back(key);
		}
	}
};


int main()
{
	int num, hours, mins, expected_time_point;

	ifstream config_file("attendance_config.txt");
	if (!config_file.is_open())
	{
		cerr << "Could not find attendance_config.txt" << endl;
		system("pause");
		return 0;
	}
	config_file >> num >> hours >> mins;
	expected_time_point = hours * 60 + mins;
	config_file.close();


	vector<Attendance> attendances(num);

	for (int i = 0; i < num; ++i)
	{
		string file_name = "attendance#" + to_string(i + 1) + string(".txt");
		ifstream attendance_file(file_name);
		if (attendance_file.is_open())
		{
			cout << "Loading attendance#" << i + 1 << ".txt"<< endl;
			attendances[i].LoadInfo_(attendance_file);
			cout << "Completed!" << endl;
		}
		else
		{
			cerr << "Failed to load attendance#"<< i + 1 <<".txt"<< endl;
			system("pause");
			return 0;
		}
		attendance_file.close();
	}

	cout << "WELL DONE"<<endl;
	cout << "The program will sign up your attendances at " << hours << ":" << mins << " every day :)\n";
	cout << "You can terminate this process through task managern";
	cout << "GUI closes in 10s" << endl;
	Sleep(5000);
	::ShowWindow(::GetConsoleWindow(), false);

	bool done_today = false;
	while (true)
	{
		//get current time
		time_t time_in_int64 = time(NULL);

		//convert to a readable format
		tm current_time;
		localtime_s(&current_time, &time_in_int64);
		int current_time_point = current_time.tm_hour * 60 + current_time.tm_min;

		//5 minutes snooze time
		int time_elapsed = current_time_point - expected_time_point;
		if (time_elapsed >= -3 && time_elapsed <= 3)
		{
			if (!done_today)
			{
				for (Attendance& attendance : attendances)
				{
					Sleep(20000);
					attendance.SignUp_();
				}
				done_today = true;
			}
		}
		else
		{
			//reset the schedule
			done_today = false;
		}

		Sleep(1000);
	}
}

