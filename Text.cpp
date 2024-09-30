#include "linkedList.cpp"
#include <stack>
#include <iostream>
#include <queue>
#include <cmath>
#include <ncurses.h>

#define ESC 27
#define ENTER 10
#define DEL 127
#define UP 0
#define DOWN 1
#define RIGHT 2
#define LEFT 3

struct Cursor
{
	int x, y;
};

extern std::queue<std::string> commands;
extern std::string displayMessage;

class Text
{
public:
	DoubleLinkedList<std::string> textList;
	Cursor cursor;
	Cursor scr_cur;
	std::string preString, postString;
	const int x_max;
	Text(int x_max = 0) : x_max(x_max)
	{
#ifdef DEBUG
		std::cerr << "Xmax: " << x_max << "\n";
#endif
	}

	bool AddCharacter(char c)
	{
		if (c == '\t')
		{
			cursor.x += 4;
			scr_cur.x += 4;
			preString.push_back(' ');
			preString.push_back(' ');
			preString.push_back(' ');
			preString.push_back(' ');
		}
		else
		{
			preString.push_back(c);
			cursor.x++;
			scr_cur.x++;
		}
		if (scr_cur.x + postString.size() > x_max)
		{
			StoreLine();
			commands.push("redisplay:");
		}

		if (scr_cur.x > x_max)
		{
			// StoreLine();
			// commands.push("redisplay:");
			scr_cur.y++;
			scr_cur.x -= x_max;
		}

		return true;
	}
	void DeleteCharacter()
	{
		if (preString.empty())
		{
			std::string line_text = postString;
			try
			{
				--textList.active;
			}
			catch (const unsigned int &e)
			{
				if (e == OUT_OF_BOUNDS)
					return;
			}
			// preString = textList.active->data;
			// preString.pop_back();
			cursor.x = -1;
			LoadLine();
			postString = line_text;
			StoreLine();
			textList.Delete(textList.active->next);
			//commands.push("delline:" + std::to_string(cursor.y));
			cursor.y--;
			scr_cur.y--;
			//commands.push("display:" + std::to_string(cursor.y));
			commands.push("redisplay:");
			// displayMessage = "Deleted Empty Line";
		}
		else
		{
			preString.pop_back();
			cursor.x--;
			scr_cur.x--;
			if(scr_cur.x == 0 && !preString.empty())
			{
				scr_cur.y--;
				scr_cur.x = x_max;
			}
			//move(LEFT);
			if(scr_cur.x == x_max)
			{
				commands.push("delch:" + std::to_string(scr_cur.y+1) + "," + std::to_string(0));
			}
			else
				commands.push("delch:" + std::to_string(scr_cur.y) + "," + std::to_string(scr_cur.x));
			if(preString.size() + postString.size() > x_max)
				{
					StoreLine();
					commands.push("redisplay:");
				}
		}
		// displayMessage = "string: " + preString + ":" + postString;
	}

	void move(int dir)
	{
		switch (dir)
		{
		case UP:
			StoreLine();

			if (preString.size() > x_max)
			{
				scr_cur.y--;
				cursor.x -= x_max;
				#ifdef DEBUG
					std::cerr << "Moving up same line";
				#endif
				LoadLine();
			}
			else
			{
				try
				{
					cursor.y--;
					scr_cur.y--;
					--textList.active;
					
				}
				catch (const int &e)
				{
					// std::cerr<<"Error OOB";
					cursor.y++;
					scr_cur.y++;
					break;
				}
				cursor.x = std::min(scr_cur.x, int(textList.active->data.size()));
				LoadLine();
				scr_cur.x = cursor.x;
				#ifdef DEBUG
					std::cerr<<"Screen Cursor: "<<scr_cur.y<<" "<<scr_cur.x<<"\n";
				#endif

				if(textList.active->data.size() > x_max)
				{
					int lines_rendered = std::ceil(float(textList.active->data.size()) / x_max);
					cursor.x += (lines_rendered - 1) * x_max;
					if(scr_cur.x == 0)
						cursor.x++;
					LoadLine();
					#ifdef DEBUG
						std::cerr<<"Moving up: Rendered Lines:" <<lines_rendered<<"\n";
						std::cerr<<"Cursor: "<<cursor.y<<" "<<cursor.x<<"\n";
						std::cerr<<"Screen Cursor: "<<scr_cur.y<<" "<<scr_cur.x<<"\n";
					#endif 
				}
			}			
			if(scr_cur.x == 0 && !preString.empty())
			{
				cursor.x++;
				scr_cur.x++;
				char x = postString.front();
				postString.erase(0,1);
				preString.push_back(x);
			}
			break;
		case DOWN:
			StoreLine();

			if (scr_cur.x + postString.size() > x_max)
			{
				#ifdef DEBUG
					std::cerr << "From "<<scr_cur.x<<" "<<scr_cur.y<<" ";
				#endif
				cursor.x += x_max;
				//scr_cur.x += x_max;
				if (scr_cur.x == 0)
				{
					scr_cur.x++;
					cursor.x++;
				}
				scr_cur.y++;
				#ifdef DEBUG
					std::cerr << "To "<<scr_cur.x<<" "<<scr_cur.y<<"\n";
				#endif
			}
			else
			{
				try
				{
					cursor.y++;
					scr_cur.y++;
					++textList.active;
					cursor.x = std::min(scr_cur.x, int(textList.active->data.size()));
					scr_cur.x = cursor.x;
				}
				catch (const int &e)
				{
					// std::cerr<<"Error OOB";
					cursor.y--;
					scr_cur.y--;
					break;
				}
			}
			LoadLine();
			break;
		case LEFT:
			if (!preString.empty())
			{
				cursor.x--;
				scr_cur.x--;
				char x = preString.back();
				preString.pop_back();
				postString = x + postString;
				if (scr_cur.x <= 0 && !preString.empty())
				{
					scr_cur.y--;
					scr_cur.x += x_max;
				}
			}
			break;
		case RIGHT:
			if (!postString.empty() && !(postString == "\n"))
			{
				cursor.x++;
				scr_cur.x++;
				char x = postString.front();
				postString.erase(0, 1);
				preString.push_back(x);
				if (scr_cur.x > x_max)
				{
					scr_cur.y++;
					scr_cur.x -= x_max;
				}
			}
			break;
		}
		displayMessage = "string: " + preString + ":" + postString;
	}

	void AddLine(bool newLine = true)
	{
		textList.active->data = preString;
		textList.active->newLine = newLine;
		// commands.push("display:" + std::to_string(cursor.y));
		commands.push("clrlineend:" + std::to_string(scr_cur.y) + "," + std::to_string(scr_cur.x));
		textList.Insert(textList.active, postString);
		++textList.active;
		preString = "";
		cursor.y++;
		scr_cur.y++;
		cursor.x = 0;
		scr_cur.x=0;
		//commands.push("insertline:" + std::to_string(scr_cur.y));
		// if(postString.size() > x_max)
		commands.push("redisplay:");
		// else
		// commands.push("display:" + std::to_string(scr_cur.y));
	}

	void LoadLine(int x)
	{
		if (x == -1)
		{
			textList.active = textList.tail;
			cursor.y = textList.size - 1;
		}
		else
		{
			auto n = textList.head;
			for (; n != nullptr, x > 0; x--, n = n->next)
			{
				//x -= ceil(float(n->data.size()) / x_max)-1;
			}
			if (x > 0)
				throw OUT_OF_BOUNDS;
			else
			{
				textList.active = n;
			}
			cursor.y = x;
		}
		LoadLine();
		displayMessage = "string: " + preString + ":" + postString;
	}

	void LoadLine()
	{
		if (textList.active == nullptr)
			return;
		int strS = textList.active->data.size();
		//	bool nl=false;
		if (strS == 0)
		{
			preString = postString = "";
			cursor.x = 0;
			scr_cur.x = 0;
			displayMessage = "Empty Line";
			return;
		}
		else if (textList.active->data[strS - 1] == '\n')
		{
			strS--;
			// nl=true;
		}

		if (strS == 0)
		{
			preString = postString = "";
			cursor.x = 0;
			scr_cur.x = 0;
			displayMessage = "Empty Line";
			return;
		}

		cursor.x = std::min((unsigned int)cursor.x, (unsigned int)strS);
		scr_cur.x = cursor.x % x_max;
		if(cursor.x % x_max == 0 && cursor.x != 0)
			scr_cur.x = x_max;
		preString = textList.active->data.substr(0, cursor.x);
		postString = (cursor.x < strS) ? textList.active->data.substr(cursor.x, strS - cursor.x) : "";
		// if(nl)
		// 	postString += "\n";
	}

	void StoreLine()
	{
		textList.active->data = preString + postString;
		// displayMessage +="\nStored "+preString + ":" + postString;
	}
};