#include "iGraphics.h" 
#include <stdlib.h> 
#include <stdbool.h> 
#include <windows.h> 
#include <unistd.h> 
#include <time.h> 
#include <bits/stdc++.h> 
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std; 

#define scr_width 800 
#define scr_height 600  

// BMP IMAGES: 
char menu [1][30] = {"images\\menu.bmp"}; 
char high_score [1][30] = {"images\\high_score.bmp"}; 
char settings_bmp [4][30] = {"images\\settings0.bmp", "images\\settings1.bmp", "images\\settings2.bmp", "images\\settings3.bmp"};  
char about_game [2][30] = {"images\\about_game.bmp", "images\\about_game_2.bmp"}; 
	 
// variables for score and life : 
int score = 0;
int life = 3, level; 
int game = -2, timer_index; 

// game = -2 : starting animation 
// game = -1 : end game 
// game = 0 : show menu 
// game = 1 : start game 
// game = 2 : pause game 
// game = 3 : high score 
// game = 4 : settings 
// game = 5 : about game 
// game = 6 : show levels 
// game = 7 : customize level 

class Player {
public:
    int score;
    char name[33];     // 32 chars + null

    // default constructor (needed for global array)
    Player() {
        score = 0;
        memset(name, 0, sizeof(name));
    }

    Player(int s, const char n[]) {
        score = s;
        setName(n);
    }

    void setName(const char n[]) {
        strncpy(name, n, 32);   // copy safely
        name[32] = '\0';        // ensure null termination
    }

    void setScore(int s) {
        score = s;
    }
    
    // For sorting
    bool operator<(const Player& other) const {
        return score > other.score; // descending order
    }
};

vector<Player> players; 
Player currentPlayer;

// main menu buttons : 
int button_width = 220, button_height = 50; 
int start_x = 476, start_y = 423;  
int high_score_x = 476, high_score_y = 348;  
int settings_x = 476, settings_y = 271;
int about_game_x = 476, about_game_y = 196;
int exit_x = 476, exit_y = 120; 

// show level page buttons : 
int level_button_width = 65, level_button_height = 60; 

// return/back button : 
int back_x = 22, back_y = 560; 
int back_width = 35, back_height = 30; 

// pause button : 
int pause_x = 0, pause_y = scr_height - 40; 
int pause_width = 40, pause_height = 40; 
int pause_page_x = 95, pause_page_width = 190, pause_page_height = 75; 

// settings buttons : 
static bool sound = true, music = true; 
int music_x = 483, music_y = 373, sound_y = 262; 
int music_width = 178, music_height = 88; 

// TEXT BOX FOR NAME: 
int text_box_x = 200, text_box_y = 100, text_box_width = 300, text_box_height = 50; 
bool text_box_active = false; 
int player_name_length = 0; 
char player_name[33] = ""; 

// variables for color change : 
int r = 255, g = 0, b = 0; 

void change_color () {
	if (r == 255 && g == 0 && b == 0) { r = 0; g = 255; b = 0; }
	else if (r == 0 && g == 255 && b == 0) { r = 0; g = 0; b = 255; }
	else if (r == 0 && g == 0 && b == 255) { r = 255; g = 0; b = 0; }
}

// variables for the bar : 
int bar_x = scr_width/2 - 75, bar_y = 50; 
int bar_dx = 10;   
int bar_width = 75, bar_height = 10; 

void initialize_bar () { 
	bar_x = scr_width/2 - 75; 
	bar_y = 50; 
	bar_width = 75; 
	bar_height = 10; 
} 

// variable for ball's position change : 
int ball_x = bar_x+20, ball_y = 50, ball_radius = 10;  
int ball_dx = 3, ball_dy = 5;   

void initialize_ball () {
	ball_x = bar_x+20; 
	ball_y = 50; 
	ball_radius = 10; 
	ball_dx = 3; 
	ball_dy = 5;  
} 

// Variables for the bricks : 
typedef struct {
    int index; 
    int life; 
    int x, y; 
} Bricks; 

Bricks brick [10][10]; 
Bricks custom_brick [10][10]; 			// for storing the customized levels bricks data 
int brick_width = 60, brick_height = 20; 
int custom_brick_count = 0; 

void initialize_variables () { 
	score = 0; 
 	life = 3; 
	custom_brick_count = 0; 
}

// function to customize level : (game = 7)
void show_customize_level () { 
	iSetColor (255, 255, 255); 
	iShowBMP (0, 0, "images\\customizable_level_page.bmp");  

	// customizable level (level 5) : 
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			custom_brick[i][j].index = i*10+j; 
			if (custom_brick[i][j].life == 0) { 
				iSetColor (255, 255, 255); 
				iRectangle (custom_brick[i][j].x, custom_brick[i][j].y, brick_width, brick_height); 
			} 
			else if (custom_brick[i][j].life == 1) { 
				iSetColor (255, 255, 255); 
				iFilledRectangle (custom_brick[i][j].x, custom_brick[i][j].y, brick_width, brick_height); 
			} 
			else if (custom_brick[i][j].life == 2) { 
				iSetColor (2, 0, 130); 
				iFilledRectangle (custom_brick[i][j].x, custom_brick[i][j].y, brick_width, brick_height); 
			} 
			else if (custom_brick[i][j].life >= 2) { 
				iSetColor (0, 4+5*custom_brick[i][j].life, 53+5*custom_brick[i][j].life); 
				iFilledRectangle (custom_brick[i][j].x, custom_brick[i][j].y, brick_width, brick_height); 
			} 
		} 
	}
	return; 
}

// initialize the bricks according to the levels : 
void initialize_bricks() { 
    // level 1 : 
    if (level == 1) {
        for (int i = 0; i < 10; i++) { 
            for (int j = 0; j < 10; j++) { 
                // brick index : 
                brick[i][j].index = i*10+j; 
                
                // brick co-ordinates : 
                brick[i][j].x = j*(brick_width+5)+50; 
                brick[i][j].y = scr_height-(i+1)*(brick_height+5); 
                
                // brick life : 
				if (i%2==0) {
					if (j%2!=0) brick[i][j].life = 1; 
					else brick[i][j].life = 2; 
				} 
				else {
					if (j%2!=0) brick[i][j].life = 2; 
					else brick[i][j].life = 1; 
				}  
            }
        }
    } 
    

    // level 2 : 
    else if (level == 2) { 
        for (int i = 0; i < 10; i++) { 
            for (int j = 0; j < 10; j++) { 
                // brick index : 
                brick[i][j].index = i*10+j; 
                
                // brick co-ordinates : 
                brick[i][j].x = j*(brick_width+5)+50; 
                brick[i][j].y = scr_height-(i+1)*(brick_height+5); 
                
                // brick life :  
                int indx = brick[i][j].index;  
                // C : 
                if (indx == 20 || indx == 30 || indx == 40 || indx == 50 || indx == 60 || indx == 21 || indx == 61) {
                    brick[i][j].life = 3; 
                } 
                // S : 
                else if (indx == 23 || indx == 24 || indx == 25 || indx == 33 || indx == 43 || indx == 44 || indx == 45 || indx == 55 || indx == 65 || indx == 64 || indx == 63)  {
                    brick[i][j].life = 3; 
                } 
                // E : 
                else if (indx == 27 || indx == 37 || indx == 47 || indx == 57 || indx == 67 || indx == 28 || indx == 48 || indx == 68) {
                    brick[i][j].life = 3; 
                } 
                else if (indx>=90 && indx <= 99) {
                    brick[i][j].life = 3; 
                } 
                else {
                    brick[i][j].life = 1; 
                }
            }
        } 
    } 

    // level 3 : 
    else if (level == 3) { 
        for (int i = 0; i < 10; i++) { 
            for (int j = 0; j < 10; j++) { 
                // brick index : 
                brick[i][j].index = i*10+j; 
                
                // brick co-ordinates : 
                brick[i][j].x = j*(brick_width+5)+50; 
                brick[i][j].y = scr_height-(i+1)*(brick_height+5); 
                
                // brick life :  
                if (i == j || i+j == 10-1) brick[i][j].life = 3; 
                else brick[i][j].life = 1; 
                if (brick[i][j].index == 44 || brick[i][j].index == 45 || brick[i][j].index == 54 || brick[i][j].index == 55) {
                    brick[i][j].life = 1000; 
                } 
            }
        } 
    } 

	// level 4 : 
    else if (level == 4) { 
        for (int i = 0; i < 10; i++) { 
            for (int j = 0; j < 10; j++) { 
                // brick index : 
                brick[i][j].index = i*10+j; 
                
                // brick co-ordinates : 
                brick[i][j].x = j*(brick_width+5)+50; 
                brick[i][j].y = scr_height-(i+1)*(brick_height+5); 
                
                // brick life :  
                if (i == 0 || j == 0 || i == 9 || j == 9) brick[i][j].life = 3; 
                else brick[i][j].life = 1; 
                if (i == 4 || i == 5 || j == 4 || j == 5) brick[i][j].life = 1000; 
            }
        } 
    } 
	
	// level 5 (customizable level) : 
	else if (level == 5) { 
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				brick[i][j] = custom_brick[i][j]; 			// brick structure = custom_brick structure
			}
		} 
		custom_brick_count = 0; 
    } 
	return; 
} 

// function to initialize custom_brick variables when returned to main menu : (game = 0)
void initialize_custom_bricks () {
	for (int i = 0; i < 10; i++) { 
		for (int j = 0; j < 10; j++) { 
			// brick index : 
			custom_brick[i][j].index = i*10+j; 
			
			// brick co-ordinates : 
			custom_brick[i][j].x = j*(brick_width+5)+50; 
			custom_brick[i][j].y = scr_height-(i+1)*(brick_height+5); 
			
			// brick life : 
			custom_brick[i][j].life = 0;   
		}
    } 
	custom_brick_count = 0; 
	return; 
}

// function to show the existing bricks on the screen : 
void show_bricks () { 
    // level 1, 2, 3, 4: 
    if (level == 1 || level == 2 || level == 3 || level == 4) {
        for (int i = 0; i < 10; i++) { 
            for (int j = 0; j < 10; j++) { 
                if (brick[i][j].life == 1) { 
                    iSetColor(255, 215, 0); 
                    iFilledRectangle(brick[i][j].x, brick[i][j].y, brick_width, brick_height); 
                } 
                else if (brick[i][j].life == 3) { 
                    iSetColor(0, 0, 78);    // dark blue 
                    iFilledRectangle(brick[i][j].x, brick[i][j].y, brick_width, brick_height); 
                } 
                else if (brick[i][j].life == 2) { 
                    iSetColor(2, 0, 130);   // light blue 
                    iFilledRectangle(brick[i][j].x, brick[i][j].y, brick_width, brick_height); 
                } 
                else if (brick[i][j].life > 3) {
                    iSetColor (39, 39, 39); 
                    iFilledRectangle(brick[i][j].x, brick[i][j].y, brick_width, brick_height); 
                } 
            
            } 
        } 
    }  
    // customizable level (level 5) : 			// same as show_customize_level mode 
    else if (level == 5) { 
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                brick[i][j].index = i*10+j; 
                if (brick[i][j].life == 1) { 
                    iSetColor (255, 255, 255); 
                    iFilledRectangle (brick[i][j].x, brick[i][j].y, brick_width, brick_height); 
                } 
                else if (brick[i][j].life == 2) { 
                    iSetColor (2, 0, 130); 
                    iFilledRectangle (brick[i][j].x, brick[i][j].y, brick_width, brick_height); 
                } 
				else if (brick[i][j].life >= 2) { 
                    iSetColor (0, 4+5*brick[i][j].life, 53+5*brick[i][j].life); 
                    iFilledRectangle (brick[i][j].x, brick[i][j].y, brick_width, brick_height); 
                } 
            } 
        }
    } 
	return; 
} 

void initialize_all () {
	initialize_variables (); 
	initialize_bricks (); 
	initialize_bar (); 
	initialize_ball (); 
}

void read_score() {
    players.clear();
    ifstream fin("score.txt");
    if (!fin.is_open()) {
        // Create default players if file doesn't exist
        for (int i = 0; i < 3; i++) {
            players.push_back(Player(0, ""));
        }
        cout << "file was empty" << endl; 
        return;
    }

    string line;
    while (getline(fin, line) && players.size() < 3) {
        size_t commaPos = line.find(',');
        if (commaPos != string::npos) {
            int playerScore = stoi(line.substr(0, commaPos));
            string playerName = line.substr(commaPos + 1);
            players.push_back(Player(playerScore, playerName.c_str()));
        }
    }
    fin.close();

    // Ensure we have exactly 3 players
    while (players.size() < 3) {
        players.push_back(Player(0, ""));
    }
    
    // Sort players by score
    sort(players.begin(), players.end());

	cout << "players read successful" << endl; 
}

void write_score() {
    ofstream fout("score.txt");
    if (!fout.is_open()) {
        cout << "file not open" << endl; 
        return;
    }
    
    // Sort before writing
    sort(players.begin(), players.end());
    
    // Keep only top 3
    if (players.size() > 3) {
        players.resize(3);
    }
    
    for (size_t i = 0; i < players.size(); i++) {
        fout << players[i].score << "," << players[i].name << endl;
    }
	cout << "write score successful" << endl; 
    fout.close();
}

void update_high_scores() {
	cout << "update high scores called for: " << player_name << endl; 
    // Set current player's name before updating
    currentPlayer.setName(player_name);
    currentPlayer.setScore(score);
    
    // Add current player to the list
    players.push_back(currentPlayer);
    
    // Sort and keep only top 3
    sort(players.begin(), players.end());
    if (players.size() > 3) {
        players.resize(3);
    }
    cout << "current player: " << currentPlayer.name << ", score: " << currentPlayer.score << endl; 
    write_score();
}

void reset_score() {
    players.clear();
    for (int i = 0; i < 3; i++) {
        players.push_back(Player(0, ""));
    }
    write_score();
}

// function to play the background music continuously : 
void play_bgm () {
	if (music) {
		PlaySound ("sounds\\bgm_1.wav", NULL, SND_LOOP | SND_ASYNC); 
	} 
	else {
		PlaySound (NULL, 0, 0); 
	} 
}

void show_starting_animation () {
	iShowBMP (0, 0, "images\\start1.bmp"); 
} 

// function to show the menu : (game = 0)
void show_menu () {  
	iShowBMP (0, 0, menu[0]);  
} 

// function to show the level list before starting the game :  (game = 6)
void show_levels () {   
	iShowBMP (0, 0, "images\\select_level.bmp"); 
}

// high score page : (game = 3)
void show_high_score () {    
	iSetColor (255, 255, 255); 
	iShowBMP (0, 0, high_score [0]); 
	 
	// Display top 3 scores
	for (int i = 0; i < 3 && i < players.size(); i++) {
		char show_score[20];
		sprintf(show_score, "%d", players[i].score);
		
		// Position scores based on rank
		int y_pos = 335 - (i * 115); // Adjust Y position for each rank
		
		// Display score
		iText(550, y_pos, show_score, GLUT_BITMAP_TIMES_ROMAN_24);
		
		// Display name
		iText(150, y_pos, players[i].name, GLUT_BITMAP_TIMES_ROMAN_24);
	}
} 

// settings page : (game = 4)
void show_settings () { 
	if (music && sound) {
		iShowBMP (0, 0, settings_bmp [0]); 
	}
	if (!music && sound) {
		iShowBMP (0, 0, settings_bmp [1]); 
	} 
	if (music && !sound) {
		iShowBMP (0, 0, settings_bmp [2]); 
	} 
	if (!music && !sound) {
		iShowBMP (0, 0, settings_bmp [3]); 
	} 
} 

// about game page : (game = 5 || game = 8) 
void show_about_game () { 
	if (game == 5) iShowBMP (0, 0, about_game [0]); 
	else if (game == 8) iShowBMP (0, 0, about_game [1]); 
}

// function to show the live score :  (game = 1)
void show_score () { 
    iSetColor (255, 255, 0); 
    char score_text[20];  
    sprintf (score_text, "Score : %d", score);  
    iText (10, 50, score_text);  
	char life_text[20]; 
	sprintf (life_text, "Life : %d", life); 
	iText (10, 30, life_text); 
} 

// game is paused : (game = 2) 
void show_paused_page () { 
	iShowBMP (0, 0, "images\\pause.bmp"); 
}

// game is started : (game = 1)
void start_game () {   
	iSetColor (r, g, b); 
	iFilledCircle (ball_x, ball_y, ball_radius, 2000);  
	iFilledRectangle (bar_x, bar_y, bar_width, bar_height);   
	
	iSetColor (255, 0, 255);   
	iShowBMP (pause_x, pause_y, "images\\pause_button.bmp"); 
	iFilledRectangle (0, 0, scr_width, 10); 
	if (score < 1000) { 
		show_bricks (); 
	}
	show_score (); 
}

// function to show the final score after the end of the game :  (game = -1)
void show_final_score () {  
	iShowBMP (0, 0, "images\\final_score.bmp");   

	iSetColor (255, 255, 0); 
	char final_score[20]; 
	sprintf (final_score, "%d", score);  
	iText (460, 415, final_score, GLUT_BITMAP_TIMES_ROMAN_24); 
	
	// Show highest score
	if (players.size() > 0) {
		char show_highest_score[20]; 
		sprintf(show_highest_score, "%d", players[0].score);   
		iText(510, 330, show_highest_score, GLUT_BITMAP_TIMES_ROMAN_24);
	}

	// name entering option : 
	iText (10, 100, "ENTER NAME : ", GLUT_BITMAP_TIMES_ROMAN_24); 
	iSetColor (100, 100, 100); 
	iFilledRectangle (text_box_x, text_box_y, text_box_width, text_box_height); 
	iSetColor (0, 255, 200); 
	iRectangle (text_box_x, text_box_y, text_box_width, text_box_height); 
	iSetColor (0, 0, 0); 
	iText (text_box_x + 10, text_box_y + 15, player_name, GLUT_BITMAP_TIMES_ROMAN_24);  
	
	// Show cursor if text box is active
	if (text_box_active) {
		iSetColor(0, 0, 0);
		int cursor_x = text_box_x + 10 + player_name_length * 10; // Approximate width
		iLine(cursor_x, text_box_y + 10, cursor_x, text_box_y + text_box_height - 10);
	}
} 

/* function iDraw() is called again and again by the system. */ 
void iDraw () { 
	iClear (); 
	if (game == -2) { 
		show_starting_animation ();  
	}
	if (game == 0) { 
		show_menu (); 
	}  
	else if (game == 6) {
		show_levels (); 
	} 
	else if (game == 7) { 
		show_customize_level ();  
	}
	else if (game == 1) { 
		start_game (); 
	} 
	else if (game == 2) { 
		show_paused_page ();  
	} 
	else if (game == -1) { 
		show_final_score (); 
	} 
	else if (game == 3) { 
		show_high_score ();  
	} 
	else if (game == 4) { 
		show_settings (); 
	} 
	else if (game == 5 || game == 8) {
		show_about_game (); 
	}
	
	iSetColor (r, g, b); 
}

// function to move the ball and detect the collisions : 
void ball_move () {
    ball_x += ball_dx; 
    ball_y += ball_dy;  

	// ball's collision with wall : 
    if (ball_x >= (scr_width-ball_radius) || ball_x <= ball_radius) {
        ball_dx *= -1; 
		change_color ();
    } 

	// ball's collision with upper wall : 
    else if (ball_y >= (scr_height-ball_radius)) {
        ball_dy *= -1; 
        change_color ();
    } 

	// ball and bar's collision : 
    else if (ball_y - ball_radius <= bar_y + bar_height && ball_y - ball_radius >= bar_y &&
    	ball_x >= bar_x && ball_x <= bar_x + bar_width) {
		// left side of the bar : 
        if (ball_x>=bar_x && ball_x<=bar_x+bar_width/3) {
			ball_dy = 5; 
			ball_dx = -5; 
		} 
		// middle part of the bar : 
		if (ball_x>bar_x+bar_width/3 && ball_x<=bar_x+bar_width*2/3) {
			ball_dy = 8; 
			if (ball_dx > 0) ball_dx = 1; 
			else ball_dx = -1;  
		} 
		// right part of the bar : 
		else if (ball_x>bar_x+bar_width*2/3 && ball_x<=bar_x+bar_width) {
			ball_dy = 5; 
			ball_dx = 5; 
		} 
        change_color (); 
    } 

	// ball misses the bar : 
    else if (ball_y-ball_radius < 0) { 
		life --; 
        // game ends if life is finished : 
		if (life == 0) {
			game = -1;  // game ends, show the final score 
			iPauseTimer (timer_index); 
			// Reset player name input
			player_name_length = 0;
			memset(player_name, 0, sizeof(player_name));
			text_box_active = true;
			return; 
		}
		// game restarts if more lifes are available : 
		initialize_bar (); 
		initialize_ball (); 
		Sleep(1000);  
    }  

	// Ball collision with bricks
	else if (level >= 1 && level <= 5) {
		int count = 0;

		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if (brick[i][j].life > 0) {
					if (brick[i][j].life < 10) count++; // count remaining bricks
					
					// Check collision with the brick
					bool collisionX = (ball_x + ball_radius >= brick[i][j].x) && (ball_x - ball_radius <= brick[i][j].x + brick_width);
					bool collisionY = (ball_y + ball_radius >= brick[i][j].y) && (ball_y - ball_radius <= brick[i][j].y + brick_height);

					if (collisionX && collisionY) {
						// Determine collision side
						bool hitFromTopOrBottom = (ball_y < brick[i][j].y) || (ball_y > brick[i][j].y + brick_height);
						bool hitFromSide = (ball_x < brick[i][j].x) || (ball_x > brick[i][j].x + brick_width);

						// Reverse direction based on side
						if (hitFromTopOrBottom) ball_dy *= -1;
						if (hitFromSide) ball_dx *= -1;

						// Reduce brick life and update score
						if (brick[i][j].life < 10) brick[i][j].life--;
						if (brick[i][j].life == 0) score += 10;

						break; // one collision per update
					}
				}
			}
		}

		// No bricks left
		if (count == 0) {
			game = -1; // go to final score page
			iPauseTimer(timer_index);
			// Reset player name input
			player_name_length = 0;
			memset(player_name, 0, sizeof(player_name));
			text_box_active = true;
			return;
		}
	}
} 

void iMouseMove(int mx, int my) {
	// control the bar using mouse : 
	bar_x = mx - bar_width / 2;
	if (bar_x < 0) bar_x = 0;
	if (bar_x + bar_width > scr_width) bar_x = scr_width - bar_width;
   
	if (game == 7) { 
		// select the bricks using mouse : 
		for (int i = 0; i < 10; i++) {
			for (int j = 0; j < 10; j++) {
				if (mx >= custom_brick[i][j].x && mx <= custom_brick[i][j].x+brick_width &&
					my >= custom_brick[i][j].y && my <= custom_brick[i][j].y+brick_height) { 
					if (custom_brick[i][j].life == 0 && custom_brick_count < 100) {
						custom_brick_count ++; 
						custom_brick[i][j].life ++; 
					} 
					if (custom_brick_count >= 100) break; 
				}
			} 
			// all bricks are selected
			if (custom_brick_count >= 100) {  
				break; 
			} 
		} 
		if (custom_brick_count >= 100) { 
			initialize_all (); 
			Sleep(1000); 
			game = 1; 
			timer_index = iSetTimer (5, ball_move); 
		} 
	}
}

void iMouse(int button, int state, int mx, int my) { 
	// left button : 
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) { 
		// go to main menu from the starting animation page : 
		if (game == -2) { 
			game = 0; 	// main menu 
		}

		// main menu : 
		else if (game == 0 && mx >= start_x && mx <= start_x+button_width) { 
			if (sound && !music) {  
				PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC);  
			} 
			// show levels after clicking the start game button  : 
			if (my >= start_y && my <= start_y+button_height) { 
				game = 6;  
			} 
			// high score : 
			else if (my >= high_score_y && my <= high_score_y+button_height) {
				game = 3; 
			} 
			// settings : 
			else if (my >= settings_y && my <= settings_y+button_height) {
				game = 4; 
			} 
			// about game : 
			else if (my >= about_game_y && my <= about_game_y+button_height) {
				game = 5; 
			} 
			// exit : 
			else if (my >= exit_y && my <= exit_y+button_height) {
				write_score();
				exit (0); 
			} 
		} 

		// show level page : 
		else if (game == 6) { 
			// level 1 selected : 
			if (mx >= 560 && mx <= 560+level_button_width && my >= 385 && my <= 385+level_button_height) {
				level = 1; 
				game = 1; 
				initialize_all (); 
				timer_index = iSetTimer (5, ball_move);  
			}
			// level 2 selected : 
			else if (mx >= 580 && mx <= 580+level_button_width && my >= 310 && my <= 310+level_button_height) {
				level = 2; 
				game = 1; 
				initialize_all (); 
				timer_index = iSetTimer (5, ball_move);  
			} 
			// level 3 selected : 
			else if (mx >= 600 && mx <= 600+level_button_width && my >= 245 && my <= 245+level_button_height) {
				level = 3; 
				game = 1; 
				initialize_all (); 
				timer_index = iSetTimer (5, ball_move);  
			} 
			// level 4 selected : 
			else if (mx >= 620 && mx <= 620+level_button_width && my >= 180 && my <= 180+level_button_height) {
				level = 4; 
				game = 1; 
				initialize_all (); 
				timer_index = iSetTimer (5, ball_move);   
			} 
			// level 5 (customizable level) selected : 
			else if (mx >= 640 && mx <= 640+level_button_width && my >= 115 && my <= 115+level_button_height) {
				level = 5; 
				game = 7; 		// customizable mode on  
				initialize_custom_bricks (); 
			} 
		} 

		// customizable level page : 
		else if (game == 7) {
			// back/return button : 
			if (mx >= back_x && mx <= back_x+back_width && my >= back_y && my <= back_y+back_height) {
				if (sound && !music) {  
					PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC); 
				}
				game = 6; 	// show level page 
			} 
			
			// start game button : 
			if (mx >= 680 && mx <= 680+110 && my >= 10 && my <= 10+50 || custom_brick_count >= 100) {
				if (sound && !music) {  
					PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC); 
				}
				game = 1; 	// start game 
				initialize_all ();  
				Sleep(1000); 
				timer_index = iSetTimer (5, ball_move); 
			} 

			// select the bricks using mouse : 
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					if (mx>=custom_brick[i][j].x && mx<=custom_brick[i][j].x+brick_width &&
					my>=custom_brick[i][j].y && my<=custom_brick[i][j].y+brick_height) { 
						if (custom_brick[i][j].life==0 && custom_brick_count < 100) custom_brick_count++; 
						custom_brick[i][j].life ++;  
						if (custom_brick_count >= 100) break; 
					}
				} 
				// all bricks are selected
				if (custom_brick_count >= 100) { 
					game = 1; 
					initialize_all (); 
					Sleep(1000); 
					break; 
				} 
			}
		}

		// return/back to main menu : 
		if (game != 0 && game != 1 && game != 2) { 
			if (mx >= back_x && mx <= back_x+back_width && my >= back_y && my <= back_y+back_height) {
				if (sound && !music) {  
					PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC); 
				} 
				if (game == 4 && score > 0) game = 2; 	// return to the paused game page 
				if (game == 7) game = 6; 
				else game = 0; 	// main menu 
			} 
		} 

		// show crdits and about game page : 
		if (game == 5) { 
			if (mx>=275 && mx<=520 && my>=55 && my<=125) { 
				if (sound && !music) {
					PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC);  
				}  
				game = 8; 			// show about game 2 page 
			}
		}

		// settings : 
		else if (game == 4) { 
			if (mx >= music_x && mx <= music_x+music_width) { 
				if (sound && !music) {
					PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC);  
				} 
				if (my >= music_y && my <= music_y+music_height) { 
					music = !music; 
					play_bgm ();  
				} 
				if (my >= sound_y && my <= sound_y+music_height) { 
					sound = !sound; 
				} 
			} 
		} 

		// high score reset : 
		else if (game == 3) {
			if (mx >= 270 && mx <= 490 && my >= 30 && my <= 75) { 
				if (sound && !music) {  
					PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC);  
				} 
				reset_score(); 
			} 
		}
		
		// home button in final score page : 
		if (game == -1) { 
			if (mx >= 5 && mx <= 5+35 && my >= 560 && my <= 560+35) { 
				if (sound && !music) {  
					PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC);  
					play_bgm (); 
				} 
				iPauseTimer (timer_index); 
				text_box_active = false; 
			    update_high_scores();
				initialize_all (); 
				game = 0; 	// return to main menu 
			} 
			// restart button : 
			else if (mx >= 750 && mx <= 785 && my >= 560 && my <= 560+35) { 
				if (sound && !music) {  
					PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC);  
					play_bgm (); 
				} 
				text_box_active = false; 
			    update_high_scores();
				initialize_all (); 
				game = 1; 	// restart the game 
				timer_index = iSetTimer (5, ball_move); 
			} 
			// player name input is active : 
			else if (mx >= text_box_x && mx <= text_box_x + text_box_width && my >= text_box_y && my <=  text_box_y + text_box_height) {
				text_box_active = true; 
			} 
			// if clicked elsewhere, deactivate the player name input mode : 
			// else {
			// 	text_box_active = false; 
			// }
		} 

		// pause button : 
		if (game == 1) {  
			if (mx >= pause_x && mx <= pause_x+pause_width && my >= pause_y && my <= pause_y+pause_height) { 
				if (sound && !music) {  
					PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC);  
				} 
				game = 2; 	// game is paused 
				iPauseTimer (timer_index);   
			} 	
		} 

		// paused game page buttons : 
		if (game == 2) { 
			if (mx>=pause_page_x && mx <= pause_page_x+pause_page_width) { 
				// resume the game : 
				if (my>=500 && my<=500+pause_page_height) {
					game = 1; 
					iResumeTimer (timer_index); 
					if (sound && !music) {  
						PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC);  
					} 
				} 
				// restart : 
				else if (my>=405 && my<=405+pause_page_height) { 
					initialize_all (); 
					iResumeTimer (timer_index); 
					game = 1; 
					if (sound && !music) {  
						PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC);  
					} 					
				} 
				// show settings : 
				else if (my>=315 && my<=315+pause_page_height) {  
					game = 4; 
					if (sound && !music) {  
						PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC);  
					} 				
				} 
				// go to main menu : 
				else if (my>=230 && my<=230+pause_page_height) { 
					// delete the previously customized level : 
					initialize_custom_bricks ();			
					initialize_all (); 
					game = 0; 
					if (sound && !music) {  
						PlaySound(TEXT("sounds\\mouse_click.wav"), NULL, SND_ASYNC);  
					} 				
				} 
			} 
		} 
	} 
	
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) { 
		if (game == 1) { 
			game = 2; 	// game paused 
			iPauseTimer (timer_index); 
		} 
		else if (game == 2) { 
			game = 1;  // game restarts 
			iResumeTimer (timer_index);  
		} 
		if (game == 7) {
			// select the bricks using mouse : 
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) { 
					if (mx>=custom_brick[i][j].x && mx<=custom_brick[i][j].x+brick_width &&
					my>=custom_brick[i][j].y && my<=custom_brick[i][j].y+brick_height) { 
						if (custom_brick[i][j].life==0 && custom_brick_count > 0) custom_brick_count--; 
						if (custom_brick[i][j].life > 0) {
							custom_brick[i][j].life --; 
							if (custom_brick[i][j].life == 0 && custom_brick_count > 0) custom_brick_count--; 
						}
					}
				} 
			}
		}
	} 
} 

void iKeyboard(unsigned char key) { 
	// exit the game : 
	if (key == 'q') { 
		exit(0); 
	} 
	// pause the running game :   
	else if (key == ' ' && game == 1) { 
		game = 2;       // game paused  
		iPauseTimer (timer_index);  
	} 
	// resume the paused game : 
	else if (key == ' ' && game == 2) { 
		game = 1;  		// game on again 
		iResumeTimer (timer_index); 
	} 
	// select options from main menu using keyboard : 
	if (game == 0) { 
		if (key == 's') game = 6; 	// show_levels 
		else if (key == 'h') game = 3;  // show_high_score 
		else if (key == 'a') game = 5; 	// show_about_game 
		else if (key == 'x') game = 4; // show_settings 
	} 


	// show levels : 
	if (game == 6) { 
		if (key == '1') { 
			level = 1; 
			game = 1; 	// game starts 
			initialize_all ();  
			timer_index = iSetTimer (5, ball_move); 
		} 
		else if (key == '2') { 
			level = 2; 
			game = 1; 	// game starts 
			initialize_all (); 
			timer_index = iSetTimer (5, ball_move); 
		} 
		else if (key == '3') { 
			level = 3; 
			game = 1; 	// game starts 
			initialize_all (); 
			timer_index = iSetTimer (5, ball_move);
		} 
		else if (key == '4') { 
			level = 4; 
			game = 1; 	// game starts  
			initialize_all (); 
			timer_index = iSetTimer (5, ball_move); 
		}  
		else if (key == '5') { 
			level = 5; 
			game = 7; 	// customize mode on    
			initialize_custom_bricks (); 
		}
	} 

	// customize level mode : 
	else if (game == 7) {
		if (key == '\r') { 
			level = 5; 
			game = 1; 	// game starts 
			initialize_all (); 
			timer_index = iSetTimer (5, ball_move); 
		}
	}

	// Player name input handling
	if (game == -1 && text_box_active) { 
		// enter button : save score and return to menu
		if (key == '\r') {
			text_box_active = false; 
			update_high_scores();
			game = 0; // Return to main menu
			initialize_all();
		} 
		// backspace button : 
		else if (key == '\b') { 
			if (player_name_length > 0) {
				player_name_length--; 
				player_name[player_name_length] = '\0'; 
			} 
		} 
		// Regular character input (alphanumeric and space)
		else if (key >= 32 && key <= 126 && player_name_length < 32) {
			player_name[player_name_length] = key; 
			player_name_length++; 
			player_name[player_name_length] = '\0'; 
		}
	} 
} 

void iSpecialKeyboard(unsigned char key) {
	if (key == GLUT_KEY_END) {
		exit(0);
	}
	//place your codes for other keys here 
	if (key == GLUT_KEY_UP) {
		music = true; 
		play_bgm (); 
	} 
	if (key == GLUT_KEY_DOWN) {
		music = false; 
		play_bgm (); 
	} 
	if (game == 1 && key == GLUT_KEY_RIGHT) {
		bar_x += 20; 
	} 
	if (game == 1 && key == GLUT_KEY_LEFT) {
		bar_x -= 20; 
	} 
	if (key == GLUT_KEY_HOME) {
		r = 255; 
		g = 255; 
		b = 255; 
	}
}

int main() { 
	srand (time (0)); 
	play_bgm();
	initialize_all (); 
	read_score(); 
	iInitialize(scr_width, scr_height, "DX BALL"); 
	return 0; 
} 