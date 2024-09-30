#include <iostream>
#include <ncurses.h>
#include <fstream>
#include <filesystem>
#include "linkedList.cpp"
#include "Text.cpp"

typedef DoubleLinkedList<std::string> text_list;

std::queue<std::string> commands;
std::string displayMessage = "";

int y_max,x_max;

void readFile(const std::string fName, Text& text);
void storeFile(const std::string fName, Text& text);
std::vector<int> splitInts(const std::string& str) 
{
    std::vector<int> result;
    std::stringstream ss(str);
    std::string temp;
    
    // Split by ',' for the part after ':'
    while (std::getline(ss, temp, ',')) {
        result.push_back(std::stoi(temp));  // Convert to int and add to result
    }
    
    return result;
}
void displayText(Text& text, unsigned int y_start, unsigned int lines, unsigned int lines_max);

int main(int argc, char* argv[])
{

	initscr();
	noecho();
	Cursor render_cur;
	getmaxyx(stdscr,y_max,x_max);
	x_max--;
	int y_start=0;
	int lines_max = y_max - 7;
	//text_list text;
	Text text(x_max);

	keypad(stdscr,true);

	std::string fn="";

	if(argc >= 2)
	{
		//message
		fn = argv[1];
	}
	const std::string fileName = ((fn == "") ? "a.txt" : fn);

	readFile(fileName, text);


	WINDOW* win = newwin(5,x_max-12,y_max-5,5);
	box(win,0,0);

	//keypad(win,true);

	int c;
	mvwprintw(win,2,2,displayMessage.c_str());
	mvwprintw(win,2,40,"Cursor: %dR:%dC",text.cursor.y,text.cursor.x);
	wrefresh(win);
	//display text
	// {	
	// 	auto n=text.textList.head;
	// 	int i=0;
	// 	for(;n!=nullptr;n=n->next,i++)
	// 		mvprintw(i,0,(n->data).c_str());
	// }

	displayText(text, 0, lines_max, lines_max);
	//std::cerr<<"Max_Lines: "<<lines_max<<"\n";

	move(text.cursor.y, text.cursor.x);
	refresh();

	bool escape=false;
	
	while(true)
	{
		c = getch();
		if(escape)
		{
			if(c == 'q' || c == 'Q')
				break;
			else
			{
				escape = false;
				if(c == ESC)
					c=0;
				displayMessage=fileName;
				wmove(win,2,0);
				box(win,0,0);
				wclrtoeol(win);
				mvwprintw(win,2,2,displayMessage.c_str());
				mvwprintw(win,2,40,"Cursor: %dR:%dC",text.cursor.y,text.cursor.x);
				wrefresh(win);
				move(text.cursor.y,text.cursor.x);
			}
		}
		// if(twokey)
		// {
		// 	twokey = false;
		// 	switch(c)
		// 	{
		// 		case 'A':
		// 			displayMessage = "You pressed UP";
		// 			text.move(UP);
		// 			break;
		// 		case 'B':
		// 			text.move(DOWN);
		// 			break;
		// 		case 'C':
		// 			text.move(RIGHT);
		// 			break;
		// 		case 'D':
		// 			text.move(LEFT);
		// 			break;
		// 	}
		// 	continue;
		// }
		
		// printw("%d", (int)c);
		// if(c!=BS && c!=DEL)
		// 	text.active->data.push_back(c);
		// cursor.x+=1;
		// if(c == int('\n'))
		// {
		// 	text.Insert(text.active, "");
		// 	++text.active;
		// 	cursor.y++;
		// 	cursor.x=0;
		// }
		switch(c)
		{
			case ESC:
				displayMessage = "Press q to Exit";
				escape = true;
				break;
			case ENTER:
				// text.active->data.push_back(c);
				// text.Insert(text.active, "");
				// ++text.active;
				// cursor.y++;
				// cursor.x=0;
				text.AddLine();
				break;
			// case 91:
			// 	c = getch();
			// 	switch(c)
			// 	{
			// 		case 'A':
			// 			//displayMessage = "You pressed UP";
			// 			text.move(UP);
			// 			break;
			// 		case 'B':
			// 			text.move(DOWN);
			// 			break;
			// 		case 'C':
			// 			text.move(RIGHT);
			// 			break;
			// 		case 'D':
			// 			text.move(LEFT);
			// 			break;
			// 	}
			// 	break;
			case KEY_UP:
				text.move(UP);
				break;
			case KEY_DOWN:
				text.move(DOWN);
				break;
			case KEY_LEFT:
				text.move(LEFT);
				break;
			case KEY_RIGHT:
				text.move(RIGHT);
				break;

			case KEY_DC:
				text.move(RIGHT);
			//nothing in between these two
			case KEY_BACKSPACE:
				// if(cursor.x > 0)
				// {
				// 	//cursor.x--;
				// 	mvdelch(cursor.y, cursor.x-1);
				// 	cursor.x--;
				// 	text.active->data.pop_back();
				// 	//mvprintw(cursor.y,0,text.active->data.c_str());
				// }
				// else
				// {
				// 	std::string curr = text.active->data;
				// 	try
				// 	{
				// 		--text.active;
				// 	}
				// 	catch(const int e)
				// 	{
				// 		if(e == OUT_OF_BOUNDS)
				// 			break;
				// 	}
				
				// 	text.Delete(text.active->next);
				// 	text.active->data.pop_back();
				// 	text.active->data += curr;
				// 	mvprintw(cursor.y-1, cursor.x, text.active->data.c_str());
				// 	getyx(stdscr, cursor.y, cursor.x);
				// }
				try
				{
					text.DeleteCharacter();
				}
				catch(const int& e)
				{
					if(e == OUT_OF_BOUNDS)
						break;
				}
				
				break;
			case 0:
				continue;
			default:
				// text.active->data.push_back(c);
				// printw("%c",c);
				// cursor.x++;
				if(text.AddCharacter(c))
				{
					//move(render_cur.y,render_cur.x);
					//insch('0');
					printw("%c",(int)c);
					printw(text.postString.c_str());
				}
				break;
		}
		while(!commands.empty())
		{
			auto q_cmd = commands.front();
			commands.pop();
			int pos = q_cmd.find(':');
			std::string cmd = q_cmd.substr(0,pos);
			std::vector<int> no = splitInts(q_cmd.substr(pos+1));
			//std::cout<<cmd<<no[0]<<((no.size()>1)?no[1]:'-')<<"\n";
			if(cmd == "delline")
			{
				move(no[0] - y_start,0);
				deleteln();
				// displayText(text, no[0],lines_max - no[0] + y_start, lines_max);
				move(0,0);
				displayText(text, 0,-1,lines_max);
				move(text.scr_cur.y - y_start,text.scr_cur.x);
			}
			if(cmd == "display")
			{
				mvprintw(no[0],0,(text.preString + text.postString).c_str());
				move(text.scr_cur.y - y_start,text.scr_cur.x);
			}
			if(cmd == "insertline")
			{
				move(lines_max-1,0);
				clrtoeol();
				move(no[0],0);
				insertln();
				move(text.scr_cur.y - y_start,text.scr_cur.x);
			}
			if(cmd == "clrlineend")
			{
				move(no[0],no[1]);
				clrtoeol();
				move(text.scr_cur.y - y_start,text.scr_cur.x);
			}
			if(cmd == "delch")
			{
				mvdelch(no[0],no[1]);
				move(text.scr_cur.y - y_start,text.scr_cur.x);
			}
			if(cmd == "redisplay")
			{
				move(0,0);
				clrtobot();
				move(0,0);
				displayText(text, y_start, -1, lines_max);
			}
		}
		

		render_cur.y = text.scr_cur.y - y_start;
		render_cur.x = text.scr_cur.x;

		if(render_cur.y >= lines_max)
		{	
			y_start += 5;
			render_cur.y = text.scr_cur.y - y_start;
			move(0,0);
			clrtobot();
			move(0,0);
			displayText(text,y_start,lines_max, lines_max);
			move(render_cur.y, render_cur.x);
			#ifdef DEBUG
			std::cerr<<"Moved Down\n(y,x) = "<<render_cur.y<<" "<<render_cur.x<<"\n";
			#endif
		}
		if(render_cur.y < 0)
		{
			y_start -= 5;
			y_start = std::max(y_start, 0);
			render_cur.y = text.scr_cur.y - y_start;
			move(0,0);
			clrtobot();
			move(0,0);
			displayText(text,y_start,lines_max, lines_max);
			move(render_cur.y, render_cur.x);
			#ifdef DEBUG
			std::cerr<<"Moved Up\n(y,x) = "<<render_cur.y<<" "<<render_cur.x<<"\n";
			#endif
		}
		refresh();

		wmove(win,0,0);
		wclrtobot(win);
		box(win,0,0);
		mvwprintw(win,2,2,displayMessage.c_str());
		mvwprintw(win,2,40,"Cursor: %dR:%dC",text.scr_cur.y,text.scr_cur.x);
		wrefresh(win);

		move(render_cur.y,render_cur.x);
	}

	text.StoreLine();

	refresh();

	delwin(win);
	
	endwin();

	storeFile(fileName, text);
	return 0;
}

void storeFile(const std::string fName, Text &text)
{
	//write to file
	std::ofstream fout;
	fout.open(fName);

	for(auto n=text.textList.head;n!=nullptr;n=n->next)
	{
		while(n->data.back() == '\n')
			n->data.pop_back();
		fout << n->data;
		if(n->next != nullptr) 
			fout << "\n";
	}
	fout.close();
}

void readFile(const std::string fName, Text &text)
{
	int x_max,y_max;
	getmaxyx(stdscr,y_max, x_max);
	x_max=100;
	if(std::filesystem::exists(fName))
	{
		std::ifstream fin(fName);
		if(fin.is_open())
		{
			std::string line;
			bool hasNewLine = true;
			while(getline(fin,line))
			{
				
				if(fin.eof())
				{
					hasNewLine = false;
				}
				text.textList.Insert(text.textList.tail, line);

			}
			if(hasNewLine)
			{
				text.textList.Insert(text.textList.tail, "");
			}
			if(text.textList.size == 0)
			{
				text.textList.Insert(text.textList.tail, "");
			}
			text.textList.active = text.textList.head;
			text.cursor.y = 0;
			text.cursor.x = 0;
			text.LoadLine();
			displayMessage = "Opened File " + fName + "\t " + std::to_string(text.textList.size) + " lines";
			
			return;
		}
	}
	{
		displayMessage = "Created File " + fName;
		text.textList.Insert(text.textList.tail, "");
		text.textList.active = text.textList.tail;
		text.cursor.y = text.cursor.x = 0;
	}
}


//assume cursor is set
//preserves text

void displayText(Text& text, unsigned int y_start, unsigned int lines, unsigned int lines_max)
{
#ifdef DEBUG
	std::cerr<<"x_max in DisplayText: "<<x_max<<"\n";
#endif
	if(lines + y_start >= text.textList.size )
		lines = -1;
	auto prev_active = text.textList.active;
	auto prev_cursor = text.cursor;
	auto prev_scr = text.scr_cur;
	text.cursor.x = 0;

	try{
		// text.LoadLine(y_start);
		// for(int i=0;((i<lines) || (lines == -1 && text.textList.active != nullptr)) &&  text.textList.active != nullptr;i++)
		// {
		// 	text.LoadLine();
		// 	printw(text.textList.active->data.c_str());
		// 	printw("\n");
		// 	++text.textList.active;
		// }
		//text.LoadLine(y_start);
		text.LoadLine(0);
		int num_lines = 0;
		while(num_lines < y_start)
		{
			text.move(DOWN);
			num_lines++;
		}
		if(text.scr_cur.x == 1)
			text.move(LEFT);		

		int line_count = 0;
		bool first = true;
		for(;(line_count < lines_max && text.textList.active!=nullptr) && (lines == -1 || line_count < lines);line_count++ )
		{
			text.LoadLine();
			std::string line = (first) ? text.postString : text.textList.active->data;
			first = false;
			int s = line.size();
			int i=0;
			bool terminate = false;
			while(s > x_max && !terminate)
			{
				printw(line.substr(i*x_max,x_max).c_str());
				printw("\n");
				s -= x_max;
				i++;
				line_count++;
				if(line_count == lines_max || line_count == lines)
				{
					terminate = true;
					break;
				}
			}
			if(terminate)
				break;
			printw(line.substr(i*x_max).c_str());
			printw("\n");
			try{
				++text.textList.active;
			}
			catch(int e){
				if(e == OUT_OF_BOUNDS)
					line_count = lines_max;
			}

		}
	}
	catch(int e)
	{
		#ifdef DEBUG
		if(e == OUT_OF_BOUNDS)
			std::cerr<<"OUT OF BOUND in displayText";
		#endif
	}

	text.textList.active = prev_active;
	text.cursor = prev_cursor;
	text.scr_cur = prev_scr;
	text.LoadLine();
}