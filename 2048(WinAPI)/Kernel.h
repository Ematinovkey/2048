#pragma once

#include <ctime>
#include <fstream>

using namespace std;

inline int random_index(int x)
{
	int index;
	index = rand() % x;
	return index;
}

class GameAI
{
	int field[4][4];
	int b_field[4][4];
	int win_n = 2048;//only deg of 2;

	void fall(int *line)
	{
		int temp[4] = { 0,0,0,0 }, k = 0;
		for (int i = 0; i < 4; i++)
			if (line[i] != 0)
				temp[k++] = line[i];
		for (int i = 0; i < 4; i++)
			line[i] = temp[i];
	}
	void mov(int *line)
	{
		fall(line);
		for (int i = 0; i < 3; i++)
			if (line[i] == line[i + 1])
			{
				line[i] *= 2;
				line[i + 1] = 0;
				fall(line);
			}
	}
	int max_n()
	{
		int max = 0;
		for (int i = 0; i<4; i++)
			for (int j = 0; j<4; j++)
				if (field[i][j]>max)
					max = field[i][j];
		return max;
	}
public:

	GameAI()
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				field[i][j] = 0;
	}

	void generateNew()
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				field[i][j] = 0;
		srand(time(NULL));
		int i = 0, j = 0;
		i = random_index(4);
		j = random_index(4);
		field[i][j] = 2;
		i = random_index(4);
		j = random_index(4);
		field[i][j] = 2;

	}
	bool win()
	{
		if (max_n() == win_n)
			return true;
		return false;
	}

	void left()
	{
		int temp[4];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				temp[j] = field[j][i];
			mov(temp);
			for (int j = 0; j < 4; j++)
				field[j][i] = temp[j];
		}
	}
	void right()
	{
		int temp[4];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				temp[j] = field[3 - j][i];
			mov(temp);
			for (int j = 0; j < 4; j++)
				field[3 - j][i] = temp[j];
		}
	}
	void up()
	{
		for (int i = 0; i < 4; i++)
			mov(field[i]);
	}
	void down()
	{
		int temp[4];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				temp[j] = field[i][3 - j];
			mov(temp);
			for (int j = 0; j < 4; j++)
				field[i][3 - j] = temp[j];
		}
	}
	void random_spawn()
	{
		int i, j, k;
		i = random_index(4);
		j = random_index(4);
		k = random_index(10);
		if (field[i][j] == 0)
		{
			if (k == 2)
				field[i][j] = 4;
			else
				field[i][j] = 2;
		}
		else
			random_spawn();
	}
	bool is_pseudo_full()
	{
		bool check = true;
		for (int i = 0; i<4; i++)
			for (int j = 0; j<4; j++)
				if (field[i][j] == 0)
					check = false;
		return check;
	}
	bool is_full()
	{
		bool check = is_pseudo_full();
		if (check)
		{
			int ch_field[4][4];
			memcpy(ch_field, field, 64);
			up();
			down();
			left();
			right();
			if (memcmp(ch_field, field, 64))
			{
				check = false;
				memcpy(field, ch_field, 64);
			}
		}
		return check;
	}

	int get_tile_value(int st_ind, int col_ind)
	{
		return field[st_ind][col_ind];
	}
	void save(ofstream &savef)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
				savef << field[i][j] << " ";
			savef << endl;
		}
	}
	void load(ifstream &savef)
	{
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				savef >> field[i][j];
	}

				
};

