//table.h
int save_to_start_table();
int ai_start_table_init();
int ai_start_table_search(int director);
//player.h
int players_turn(int color);
int initiate_field();
void replay();
//moves.h
int get_valid_moves(int color, bool check_long_chase = 1);
int move(int director);
inline int get_color(int id);
int player_move(int cmd);
int move_with_info(int director);
//learning.h
int learning_gen_data(char* filename);
int one_hot_trans();
int count();
int split();
//display.h
void color_cout(const int color1);
int print_field(const int field[][10]);
int change();
//AI.h
int AI(int color, int test_mode = 1);