#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define MAXWIDTH 30
#define MAXLINE 30
#define N 11

void map_load();
void save_map( int level );
int load_map();
void get_cur_coordinate();
int get_box_coordinate( int level );
int get_gold_coordinate();
void set_coordinate( int x, int y );
int check_clear(int level);
int getch(void);
void get_cur_map( int level );
void set_cache_map();
void get_cache_map();
void init_cache_map();
void error_check( int n, int m );
void print_help();
void print_map( char ch );
void insert_userName();

int box_count;
int cache_size = -1;
char name[N] = {0,};

char map[5][MAXLINE][MAXWIDTH];
char cur_map[MAXLINE][MAXWIDTH];
char cache_map[5][MAXLINE][MAXWIDTH];

int cur_coord[2];
int box_coord[30][2];
int gold_coord[30][2];



int main()
{
	int i = 0;
	int level = 0;
	int ret = 0;
	int box_cnt = 0;
	int gold_cnt = 0;

	char ch = 0;

	printf("Start...\n");

	map_load();    //맵 파일에서 맵 로드
	get_cur_map(level);  // 맵 정보에서 레벨에 맞는 맵을 가져옴
	get_cur_coordinate(); // 캐릭터의 좌표를 확인한다.

	box_cnt = get_box_coordinate( level ); //상자 갯수 확인 및 좌표확인
	gold_cnt = get_gold_coordinate(); //금 갯수 확인
	error_check( box_cnt, gold_cnt ); //상자의 갯수와 금의 갯수가 다르면 에러띄움

	insert_userName(); //사용자 이름 입력받음

	system("clear");

	print_map(0); // 맵을 화면에 출력하면서 게임 시작한다
	while( ch = getch()) 
	{

		if( ch == 'h' )   //왼쪽으로 이동시킴
		{
			set_coordinate( cur_coord[0], cur_coord[1]-1);
		}
		else if ( ch == 'j' )  //아래로 이동시킴
		{
			set_coordinate( cur_coord[0]+1, cur_coord[1]);
		}
		else if ( ch == 'k' )   // 위로 이동시킴
		{
			set_coordinate( cur_coord[0]-1, cur_coord[1]);
		}
		else if ( ch == 'l' )   //오른쪽으로 이동시킴
		{
			set_coordinate( cur_coord[0], cur_coord[1]+1);
		}
		else if( ch == 'u' )   //캐시에서 가장 최신 정보를 얻어와서 현재 맵을 업데이트(undo)
		{
			get_cache_map();
			system("clear");
			print_map(ch);
			continue;
		}
		else if( ch == 'r' )    //현재 레벨의 초기 맵을 다시 현재 맵으로 업데이트
		{
			get_cur_map( level );
			get_cur_coordinate( level ) ;
			system("clear");
			print_map( ch );
			continue;
		}
		else if( ch == 'n' )  //1레벨의 초기 맵으로 업데이트
		{
			level = 0;
			get_cur_map( level );
			get_cur_coordinate( level );
			system("clear");
			print_map( ch );
			continue;
		}
		else if( ch == 'd' )   //헬프메뉴를 출력
		{
			print_map( ch );
			continue;
		}
		else if( ch == 'e' )  // 게임 종료(종료하기전에 정보 저장해야함)
		{
			system("clear");
			print_map( ch );
			exit(0);
		} 
		else if( ch == 's' ) //현재 게임중인 레벨 정보와 맵 정보를 파일에 저장
		{
			save_map( level );
			system("clear");
			print_map( ch );
			continue;
		} 
		else if( ch == 'f' )  // 파일에 저장된 이전 플레이와 맵을 로드
		{
			level = load_map();
			get_cur_coordinate();
			get_box_coordinate( level );
			system("clear");
			print_map( ch );
			continue;
		}
		else 
		{
			system("clear");
			print_map(0);
			continue;
		}

		system("clear");
		print_map( ch );

		if(level < 4 && !check_clear(level))  //레벨 5이하이고 클리어했다면
		{
			level++;      //레벨을 업데이트하고
			get_cur_map( level );  //새로운 레벨의 맵으로 현재 맵을 업데이트하며
			get_cur_coordinate();  //캐릭터 위치를 새로 읽어온다
			box_cnt = get_box_coordinate( level );  //상자 위치를 새로 읽어온다.
			gold_cnt = get_gold_coordinate();   
			error_check( box_cnt, gold_cnt );  //상자의 갯수와 금의 갯수 다르면 에러를 띄운다

			init_cache_map();  //캐시 맵을 초기화 한다.
			set_cache_map();   // 초기 화면을 캐시에 업데이트한다.
			printf("Level %d Clear!!! \n", level );    //레벨 클리어 시 메시지 출력
			printf("Press any key Go to next level");  

		}
		else if( level == 4 && !check_clear(level) )   //모두 클리어 하면 축하메시지
		{
			printf("congratulation!!!");
			exit(0);
		}
	}

	return 0;
}



void map_load()   //맵 파일로부터 맵 읽어들이는 함수
{
	FILE *fd = NULL;
	int level = -1;
	int line = 0;
	int i = 0;

	char ch = 0;
	char tag[MAXWIDTH] = {0,};

	fd = fopen("map.txt", "r");

	do 
	{
		for( i = 0; i < MAXWIDTH; i++ )   //tag배열 초기화
		{
			tag[i] = 0;
		}

		i = 0;
		do 
		{
			fscanf(fd, "%c", &ch);
			tag[i] = ch;
			i++;
		} while( ch  != '\n' );            //맵 파일로부터 한 라인씩 읽어들인다.

		if( !strcmp( tag, "map\r\n"))      //"map" 문자열과 일치하면 맵의 시작
		{
			level++;
			line = 0;
			continue;
		}
		else if(!strcmp(tag,"end\r\n"))  //"end" 문자열과 일치하면 더 이상 읽지 않음 
		{
			break;
		} 
		else 
		{
			strcpy(map[level][line], tag);     //읽어들인 맵을 배열에 저장
			line++;
		}
	} while(1);

	fclose( fd );
}

void save_map(int level)    //현재 게임중인 맵을 sokoban파일에 저장하는 함수.
{
	int i = 0;
	FILE *fd = NULL;

	fd = fopen("sokoban.txt", "w");

	fprintf(fd, "level\n");
	fprintf(fd, "%d\n", level);           // 나중에 읽어들이기 위해서 레벨과 함께 저장
	fprintf(fd, "\n" );
	fprintf(fd, "map\n");

	for( i = 0; i < MAXLINE; i++ )
	{
		fprintf(fd, "%s", cur_map[i] );    //현재 진행중인 맵을 파일에 저장  
	}
	fclose( fd );
}

int load_map()  // 저장된 파일에서 이전에 실행했떤 레벨과 맵을 읽어들이는 함수.
{
	int i = 0;
	int j = 0;
	int level = 0;
	char ch = 0;
	char tag[MAXWIDTH] = {0,};

	FILE *fd = NULL;

	fd = fopen("sokoban.txt", "r");

	if( fd == NULL ) 
	{
		return -1;
	}

	fscanf( fd, "%s\n", tag );
	if(!strcmp( tag, "level" ))             //이전에 플레이하던 레벨을 로드
	{
		fscanf( fd, "%d\n", &level );
		printf("level: %d \n", level );
	}

	for( i = 0; i < MAXLINE; i++ )
	{
		for( j = 0; j < MAXWIDTH; j++ )
		{
			cur_map[i][j] = 0;            //현재 진행중인 맵 정보를 초기화한다.
		}
	}

	i = 0; j = 0;
	fscanf( fd, "%s\n", tag );
	if( !strcmp( tag, "map" ) ) {
		while( fscanf( fd, "%c", &ch ) != EOF )   //이전에 실행했던 맵 정보를 읽어들임
		{
			cur_map[i][j] = ch;
			if( ch != '\n' ) 
			{
				j++;
			}
			else {
				i++;
				j = 0;
			}
		}
	}
	return level;
}

void get_cur_coordinate()     //플리에어('@')의 위치를 찾는 함수
{
	int i = 0;
	int j = 0;

	for( i = 0; i < MAXLINE; i++ )  
	{
		for( j = 0; j < MAXWIDTH; j++ )
		{
			if( cur_map[i][j] == '@' ) 
			{ 
				cur_coord[0] = i; cur_coord[1] = j;
				break;
			}                          //전체 배열을 다 돌면서 '@'문자 정보의 위치를 저장.
		}
	}
}

int get_box_coordinate( int level ) //상자의 위치를 찾는 함수.
{
	int i = 0;
	int j = 0;
	box_count = 0;

	for( i = 0; i < MAXLINE; i++)          //전체 배열을 다 돌면서 박스 정보를 저장한다
	{
		for( j = 0; j < MAXLINE; j++) 
		{
			if( map[level][i][j] == 'O' )
			{
				box_coord[box_count][0] = i;
				box_coord[box_count][1] = j;
				box_count++;
			}
		}
	}
	return box_count;
}

int get_gold_coordinate()     //금의 위치를 찾는 함수
{
	int i = 0;
	int j = 0;
	int gold_count = 0;

	for( i = 0; i < MAXLINE; i++)           //전체 배열을 다 돌면서 금의 정보를 저장한다
	{
		for( j = 0; j < MAXLINE; j++)
		{
			if( cur_map[i][j] == '$' )
			{
				gold_coord[gold_count][0] = i;
				gold_coord[gold_count][1] = j;
				gold_count++;
			}
		}
	}
	return gold_count;
}

void set_coordinate( int x, int y ) //캐릭터가 이동할 때마다 현재 맵에 새로 그려주는 함수.
{
	int i = 0;
	int j = 0;

	int new_x = x, old_x = cur_coord[0];    // 이전 위치 정보를 저장한다.
	int new_y = y, old_y = cur_coord[1];

	if( cur_map[x][y] != '#' && cur_map[x][y] != '$')  //이동 할때 벽이나 금이 없다면 캐릭터를 이동시킨다
	{
		set_cache_map();  //맵 정보를 업데이트 하기전에 미리 캐시한다. (?)

		cur_map[old_x][old_y] = ' '; //캐릭터를 이동시킨다
		cur_map[new_x][new_y] = '@';

		cur_coord[0] = new_x;  //새로 이동한 좌표를 업데이트한다
		cur_coord[1] = new_y;

	} 
	else if ( cur_map[x][y] == '$' )  //이동할 위치에 금이 있다면 함께 이동한다
	{
		if( new_x < old_x ) 
		{

			if( cur_map[x-1][y] != '$' && cur_map[x-1][y] != '#' ) //위쪽 방향으로 이동중이었다면
			{
				set_cache_map();
				cur_map[new_x-1][new_y] = '$';     //금을 위로 한 칸 이동
				cur_map[new_x][new_y] = '@';
				cur_map[old_x][old_y] = ' ';
				cur_coord[0] = new_x;
				cur_coord[1] = new_y;
			}

		}
		else if( new_x > old_x )  //아랫쪽 방향으로 이동중이었다면
		{
			if( cur_map[x+1][y] != '$' && cur_map[x+1][y] != '#' )
			{
				set_cache_map();
				cur_map[new_x+1][new_y] = '$';    //금을 아래로 한 칸 이동
				cur_map[new_x][new_y] = '@';
				cur_map[old_x][old_y] = ' ';
				cur_coord[0] = new_x;
				cur_coord[1] = new_y;
			}

		}
		else if( new_y < old_y )      //왼쪽 방향으로 이동중이었다면
		{
			if( cur_map[x][y-1] != '$' && cur_map[x][y-1] != '#' )
			{
				set_cache_map();
				cur_map[new_x][new_y-1] = '$';  // 금을 왼쪽으로 한 칸 이동
				cur_map[new_x][new_y] = '@';
				cur_map[old_x][old_y] = ' ';
				cur_coord[0] = new_x;
				cur_coord[1] = new_y;
			}

		}
		else if( new_y > old_y )   //오른쪽 방향으로 이동중이었다면
		{
			if( cur_map[x][y+1] != '$' && cur_map[x][y+1] != '#' )
			{
				set_cache_map();
				cur_map[new_x][new_y+1] = '$';   //금을 오른쪽으로 한 칸 이동
				cur_map[new_x][new_y] = '@';
				cur_map[old_x][old_y] = ' ';
				cur_coord[0] = new_x;
				cur_coord[1] = new_y;
			}
		}
	}

	for( i = 0; i < box_count; i++ )   //박스를 새로 그려준다
	{
		if (cur_map[ box_coord[i][0] ][ box_coord[i][1] ] != '@' && cur_map[ box_coord[i][0] ][ box_coord[i][1] ] != '$') 
		{
			cur_map[ box_coord[i][0] ][ box_coord[i][1] ] = 'O';
		}
	}
}

int check_clear(int level)    //모든 박스의 위치에 금이 있다면 게임을 클리어하는 함수.
{
	int i = 0;

	for( i = 0; i < box_count; i++ )
	{
		if( cur_map[ box_coord[i][0] ][ box_coord[i][1] ] != '$' )
		{
			return -1; 
		}
	}
	return 0;
}

void get_cur_map( int level )  //현재 맵의 정보를 얻어온다.
{
	int i = 0;
	int j = 0;

	for( i = 0; i < MAXLINE; i++ )    //맵을 초기화한다.
	{
		for( j = 0; j < MAXWIDTH; j++ ) 
		{
			cur_map[i][j] = 0;
		}
	}

	for( i = 0; i < MAXLINE; i++ )   //레벨에 맞는 맵을 가져온다.
	{
		strcpy( cur_map[i], map[level][i] );
	}
}

void set_cache_map()        //최대 5개까지 캐시 맵에 업데이트 한다.(최신순으로)
{
	int i = 0;
	int j = 0;

	cache_size++;

	if( cache_size < 5 )   //캐시 크기가 5보다 작으면 캐시에 저장한다
	{
		for( i = 0; i < MAXLINE; i++ )
		{
			strcpy( cache_map[cache_size][i], cur_map[i] );
		}
	} 
	else   //캐시 크기가 5라면 더 이상 저장할 곳이 없으므로 캐시를 비워준다.
	{
		cache_size = 4;
		for( i = 0; i < cache_size; i++ )
		{
			for( j = 0; j < MAXLINE; j++ )
			{
				strcpy( cache_map[i][j], cache_map[i+1][j] );
			}
		}

		for( i = 0; i < MAXLINE; i++ )    //캐시를 비운뒤 맵정보를 저장한다.
		{
			strcpy( cache_map[cache_size][i], cur_map[i] );
		}
	}
}

void get_cache_map()        // 캐시에서 이전 맵 정보를 가져온다.
{
	int i = 0;

	if( cache_size > 4 )    //캐시 사이즈가 4보다 커도의미없으므로 최댓값은 4로 유지
	{
		cache_size = 4;
	}

	if( cache_size < 0 )  //캐시가 비어있다면 가져올 정보가 없으므로 패스한다
	{
		cache_size = -1;
		return;
	}

	for( i = 0; i < MAXLINE; i++ )  //가장 최신 맵 정보를 현재 맵으로 가져온다
	{
		strcpy( cur_map[i], cache_map[ cache_size ][i] );
	}
	get_cur_coordinate();   // 캐릭터의 위치를 새로 업데이트한다
	cache_size--;       //캐시의 크기를 줄인다.
}

void init_cache_map()   //맵을 캐시할 배열을 초기화한다
{
	int i = 0;
	int j = 0;
	int k = 0;

	for( i = 0; i < 5; i++ ) 
	{
		for( j = 0; j < MAXLINE; j++ ) 
		{
			for( k = 0; k < MAXWIDTH; k++ ) 
			{
				cache_map[i][j][k] = 0;
			}
		}
	}
}

void error_check( int n, int m )        //박스의 갯수와 금의 갯수가 다르면 에러를 띄운다.
{
	if( n != m ) {
		printf("map error...a number of '$' and 'O'is not equal.. \n");
		exit(0);
	}
}

void print_help()    //헬프 메뉴를 보여준다.
{
	system("clear");
	printf("h, j, k, l \n");
	printf("u(undo) \n");
	printf("r(replay) \n");
	printf("n(new) \n");
	printf("e(exit) \n");
	printf("s(save) \n");
	printf("f(file upload \n");
	printf("d(display help \n");
	printf("t(top) \n");
	printf("\n");
}

void print_map( char ch )  //맵 정보를 화면에 출력해준다.
{
	int i = 0;

	printf("  Hello %s \n\n\n", name );

	if( ch != 'd' )
	{
		for( i = 0; i < MAXLINE; i++ )
		{
			printf("%s", cur_map[i] );
		}
	}
	else
	{
		print_help();
	}

	if( ch != 'h' && ch != 'j' && ch != 'k' && ch != 'l' ) 
	{
		printf("(Command) %c \n", ch);
	}
	else
	{
		printf("(Command) \n");
	}
}

int getch(void)   //한 글자를 읽어들이는 함수(에코없이)
{
	int ch;
	struct termios buf;
	struct termios save;
	tcgetattr(0,&save);
	buf = save;
	buf.c_lflag&=~(ICANON|ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	tcsetattr(0,TCSAFLUSH,&buf);
	ch = getchar();
	tcsetattr(0,TCSAFLUSH,&save);
	return ch;
}

void insert_userName()  //사용자 이름을 저장한다.
{
	printf("input your name: ");
	scanf("%s", name );
}
