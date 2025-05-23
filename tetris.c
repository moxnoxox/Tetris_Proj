#include "tetris.h"

static struct sigaction act, oact;
int timed_out = 0;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	
	createRankList();

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	writeRankFile();
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	RecNode node;
	node.lv=0;
	node.score=score;
	node.f=field;
	recommend(&node);
	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
		for (int idx = 1; idx < 3; idx++) {
			int cb = nextBlock[idx];
			int offsetY = 4 + (idx-1) * 6;
			for (int i = 0; i < 4; i++) {
				move(offsetY + i, WIDTH + 11);
				for (int j = 0; j < 4; j++) {
					if (block[cb][0][i][j] == 1) {
						attron(A_REVERSE);
						printw("  ");
						attroff(A_REVERSE);
					} else {
						printw("  ");
					}
				}
			}
		}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	clear();
	newRank(score);
	clear();
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	
	for (int i=0;i<4;i++){
		for (int j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]==1){
				if(i+blockY >= HEIGHT || j+blockX<0 || j+blockX>=WIDTH){
					return 0;
				}
				if(f[i+blockY][j+blockX]==1) {
					return 0;
				}
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX) {
    // 1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
    int prevBlockRotate = blockRotate;
    int prevBlockY = blockY;
    int prevBlockX = blockX;

    switch (command) {
        case KEY_UP:
            prevBlockRotate = (blockRotate + 3) % 4; // 이전 회전 상태
            break;
        case KEY_DOWN:
            prevBlockY = blockY - 1; // 이전 Y 위치
            break;
        case KEY_LEFT:
            prevBlockX = blockX + 1; // 이전 X 위치
            break;
        case KEY_RIGHT:
            prevBlockX = blockX - 1; // 이전 X 위치
            break;
        default:
            break;
    }

    // 2. 이전 블록 정보를 지운다. DrawBlock 함수 참조
	for(int i=0;i<4;i++)
		for(int j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]==1 && i+prevBlockY>=0){
				move(i+prevBlockY+1,j+prevBlockX+1);
				printw(".");
			}
		}
	DrawField();
    // 3. 새로운 블록 정보를 그린다.
    DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	return;
}

void BlockDown(int sig){
	timed_out=0;
	if(CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)){
		blockY++;
		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
	}
	else{
		if(blockY == -1){
			gameOver=1;
		}
		score += AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		score += DeleteLine(field);
		nextBlock[0]=nextBlock[1];
		nextBlock[1]=nextBlock[2];
		nextBlock[2]=rand()%7;
		RecNode node;
		node.lv=0;
		node.score=score;
		node.f=field;
		recommend(&node);
		blockRotate=0;
		blockY=-1;
		blockX=WIDTH/2-2;
		DrawField();
		DrawNextBlock(nextBlock);
		PrintScore(score);
	}
	return;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int touched=0;//블럭이 떨어지고 필드와 맞닿은 면적을 체크하기 위한 변수
	for (int i=0;i<4;i++){
		for (int j=0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]==1){
				if(f[i+blockY+1][j+blockX]==1 || i+blockY==HEIGHT-1){
					touched++;
				}
				f[i+blockY][j+blockX]=1;
			}
		}
	}
	return touched * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	int dline=0;
	for (int i=0;i<HEIGHT;i++){
		int full=1;
		for (int j=0;j<WIDTH;j++){
			if (f[i][j]==0){
				full=0;
				break;
			}
		}
		if (full==1){
			dline++;
			for (int k=i;k>0;k--){
				for (int j=0;j<WIDTH;j++){
					f[k][j]=f[k-1][j];
				}
			}
			for (int j=0;j<WIDTH;j++){
				f[0][j]=0;
			}
		}
	}
	return dline * dline * 100;
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int currentBlock, int blockRotate){
	int shadowY = y;
	while (CheckToMove(field, currentBlock, blockRotate, shadowY + 1, x)) {
		shadowY++;
	}
	DrawBlock(shadowY, x, currentBlock, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int currentBlock, int blockRotate) {
	DrawBlock(y, x, currentBlock, blockRotate, ' ');
	DrawShadow(y, x, currentBlock, blockRotate);
	DrawRecommend(recommendY, recommendX, currentBlock, recommendR);
}

void createRankList(){
    FILE *fp = fopen("rank.txt", "r");
    if (!fp) return;
    int n;
    fscanf(fp, "%d", &n);

    RankNode *tail = NULL;
    rankHead = NULL;
    for (int i = 0; i < n; i++) {
        RankNode *node = (RankNode*)malloc(sizeof(RankNode));
        fscanf(fp, "%s %d", node->name, &node->score);
        node->next = NULL;
        if (rankHead == NULL) {
            rankHead = node;
            tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }
    fclose(fp);
}

void rank(){
	clear();
	int x = 0, y = 0, ch;
	move(0,0);
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	noecho();
	ch = wgetch(stdscr);
	if(ch == '1') {
		printw("X: ");
		echo();
		scanw("%d", &x);
		
		refresh();
		printw("Y: ");
		echo();
		scanw("%d", &y);
		
		refresh();
		if (x == 0 && y == 0) {
			RankNode *node = rankHead;
			while(node != NULL) {
				printw("%s %d\n", node->name, node->score);
				node = node->next;
			}
			refresh();
			getch();
			clear();
		}
		if(y == 0) {
			//x부터 끝까지 출력
			RankNode *node = rankHead;
			int i = 1;
			while (node != NULL) {
				if (i >= x) {
					printw("%s %d\n", node->name, node->score);
				}
				node = node->next;
				i++;
			}
			refresh();
			getch();
			clear();
			return;
		}
		if (rankHead == NULL) {
			printw("No rank found\n");
			refresh();
			getch();
			clear();
			return;
		}

		int i = 1;
		RankNode *node = rankHead;
		int found = 0;
		while (node != NULL) {
			if (i >= x && i <= y) {
				move(i - x + 2, 0);
				printw("%s %d\n", node->name, node->score);
				found = 1;
			}
			if (i > y) break;
			node = node->next;
			i++;
		}
		if (!found) {
			printw("search failure: no rank in the list\n");
		}
	}
	if(ch == '2') {
		char str[NAMELEN+1];
		int check = 0;
		printw("Input the name: ");
		echo();
		scanw("%s", str);
		refresh();
		RankNode *node = rankHead;
		while(node != NULL) {
			if(strcmp(node->name, str) == 0) {
				printw("%s %d\n", node->name, node->score);
				check = 1;
			}
			node = node->next;
		}if(check == 0) {
			printw("search failure: no name in the list\n");
		}
	}
	if(ch == '3') {
		int deleteRank;
		printw("Input the rank: ");
		echo();
		scanw("%d", &deleteRank);
		refresh();
		RankNode *node = rankHead;
		RankNode *prev = NULL;
		int i = 1;
		while (node != NULL) {
			if (i == deleteRank) {
				if (prev == NULL) {
					rankHead = node->next;
				} else {
					prev->next = node->next;
				}
				free(node);
				break;
			}
			prev = node;
			node = node->next;
			i++;
		}
		if (node == NULL) {
			printw("search failure: the rank not in the list\n");
		} else {
			printw("Rank %d deleted\n", deleteRank);
		}
	}
	refresh();
	getch();
	clear();
}

void writeRankFile(){
    FILE *fp = fopen("rank.txt", "w");
    if (!fp) return;

    // 랭크 개수 세기
    int count = 0;
    for (RankNode *node = rankHead; node != NULL; node = node->next)
        count++;
    fprintf(fp, "%d\n", count);

    for (RankNode *node = rankHead; node != NULL; node = node->next)
        fprintf(fp, "%s %d\n", node->name, node->score);

    fclose(fp);
}

void newRank(int score){
    if(score <= 0) return;
    char name[NAMELEN];
    printw("Enter your name: ");
	echo();
    scanw("%s", name);

    RankNode *newNode = (RankNode*)malloc(sizeof(RankNode));
    strcpy(newNode->name, name);
    newNode->score = score;
    newNode->next = NULL;

    // 리스트가 비었거나, 헤드보다 점수가 높으면 맨 앞에 삽입
    if (rankHead == NULL || rankHead->score < score) {
        newNode->next = rankHead;
        rankHead = newNode;
        return;
    }

    // 중간 또는 끝에 삽입
    RankNode *cur = rankHead;
    while (cur->next != NULL && cur->next->score >= score) {
        cur = cur->next;
    }
    newNode->next = cur->next;
    cur->next = newNode;
	clear();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	DrawBlock(y, x, blockID, blockRotate, 'R');
}

int recommend(RecNode *root) {
    int max = 0;
    if (root->lv >= BLOCK_NUM) return root->score;
    int blockID = nextBlock[root->lv];
    
    // for each rotation
    for (int r = 0; r < NUM_OF_ROTATE; r++) {
        // for each possible x position
        for (int x = -BLOCK_WIDTH; x < WIDTH; x++) {
            int y = -1;
            while (CheckToMove(root->f, blockID, r, y + 1, x)) y++;
            if (y < 0) continue;
            // create child node
            RecNode *child = malloc(sizeof(RecNode));
            child->lv = root->lv + 1;
            child->score = root->score;
            child->f = malloc(sizeof(char[HEIGHT][WIDTH]));
            memcpy(child->f, root->f, sizeof(char[HEIGHT][WIDTH]));
            for (int i = 0; i < CHILDREN_MAX; i++) child->c[i] = NULL;
            // apply block drop
            int added = AddBlockToField(child->f, blockID, r, y, x);
            int lines = DeleteLine(child->f);
            child->score += added + lines;
            // recurse
            int s = recommend(child);
            if (s > max) {
                max = s;
                if (root->lv == 0) {
                    recommendR = r;
                    recommendX = x;
                    recommendY = y;
                }
            }
            // we leak child and its f here; acceptable for short-lived tree
        }
    }
    return max;
}

void recommendedPlay(){
	// user code
}
