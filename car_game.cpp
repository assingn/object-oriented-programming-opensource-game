#include <iostream> //스트림을 통한 입출력 기능을 제공하는 헤더 파일
#include <conio.h> //콘솔 입출력 기능을 제공하는 비표준 헤더
#include <direct.h> //디렉터리 조작과 관련된 함수들을 제공하는 헤더 파일
#include <windows.h> //Windows API 함수를 사용하기 위한 헤더 파일
#include <time.h> //시간과 날짜와 관련된 함수들을 제공하는 헤더 파일

using namespace std;

#define SCREEN_WIDTH 90
#define SCREEN_HEIGHT 26
#define WIN_WIDTH 70
double s = 50; // 기본 속도
double defaultSpeed = 50; // 기본 속도 저장
double slowDuration = 2000; // Slow 아이템 효과 지속 시간 (밀리초)
bool isSlowed = false; // Slow 아이템 효과 여부
clock_t slowStartTime; // Slow 아이템 효과 시작 시간

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
// HANDLE : 콘솔 화면 버퍼를 식별하는 데 사용
// GetStdHandle : 표준 장치(입력, 출력, 오류) 중 하나의 핸들을 반환
COORD CursorPosition; //x값과 y값을 멤버로 가지는 구조체 선언

int enemyY[3]; //적 Y좌표
int enemyX[3]; //적 X좌표
int enemyFlag[3]; //적 Flag값 (적 생성되었는지 여부 확인)

int SlowitemX[3]; //SlowItem X좌표
int SlowitemY[3]; //SlowItem Y좌표
int SlowitemFlag[3]; //SlowItem Flag값
int HPitemX[3]; //HPItem X좌표
int HPitemY[3]; //HPItem Y좌표
int HPitemFlag[3]; //HPItem Flag값

int carPos = WIN_WIDTH / 2; //사용자 자동차 위치 가운데로 고정
int max_score = 0; //최고점수 초기화
int score = 0; //score 초기화
int Heart = 5; //사용자 목숨 초기값 : 5
int maxHeart = 5; //사용자 최대 목숨 5개 고정

/*CAR 외형*/
const char car[4][4] = { ' ', '+', '+', ' ',
                        '+', '+', '+', '+',
                        ' ', '+', '+', ' ',
                        '+', '+', '+', '+' };

/*x,y의 좌표를 입력값으로 받아 콘솔의 커서를 입력된 x,y 좌표의 위치로 이동*/
void gotoxy(int x, int y)
{
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

/*커서의 보임 여부와 크기를 설정하는 커서 세팅 함수*/
void setcursor(bool visible, DWORD size)
{
    if (size == 0)
        size = 20;

    CONSOLE_CURSOR_INFO lpCursor;
    lpCursor.bVisible = visible;
    lpCursor.dwSize = size;
    SetConsoleCursorInfo(console, &lpCursor);
}

/*벽을 그리는 함수*/
void drawBorder()
{
    //수평 경계 그림
    for (int i = 0; i < SCREEN_HEIGHT; i++) //화면의 높이(SCREEN_HEIGHT)만큼 반복
    {
        for (int j = 0; j < 17; j++) //경계의 너비를 17로 설정
        {
            //각 행의 시작과 끝 부분에 + 문자를 출력
            gotoxy(0 + j, i);
            cout << "+";
            gotoxy(WIN_WIDTH - j, i);
            cout << "+";
        }
    }
    //수직 경계 그림
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        gotoxy(SCREEN_WIDTH, i);
        cout << "+";
    }
}

//적 자동차의 x좌표를 무작위로 생성하는 함수
void genEnemy(int ind)
{
    //17~49까지의 무작위 x좌표로 적 자동차가 생성되게 세팅
    //17부터 나타나게 하는 것은 게임 화면의 중앙에 나타나게 하기 위함
    enemyX[ind] = 17 + rand() % (33);
}

//적 자동차를 화면에 그리는 함수
void drawEnemy(int ind)
{
    //enemyFlag의 값이 true-> 커서가 y축 + 1 되며 적 자동차의 형태가 출력됨
    if (enemyFlag[ind] == true)
    {
        gotoxy(enemyX[ind], enemyY[ind]);
        cout << "****";
        gotoxy(enemyX[ind], enemyY[ind] + 1);
        cout << " ** ";
        gotoxy(enemyX[ind], enemyY[ind] + 2);
        cout << "****";
        gotoxy(enemyX[ind], enemyY[ind] + 3);
        cout << " ** ";
    }
}

//적 자동차를 화면에서 지우는 함수
void eraseEnemy(int ind)
{
    //enemyFlag의 값이 true-> 화면에서 적 자동차가 지워짐
    if (enemyFlag[ind] == true)
    {
        gotoxy(enemyX[ind], enemyY[ind]);
        cout << "    ";
        gotoxy(enemyX[ind], enemyY[ind] + 1);
        cout << "    ";
        gotoxy(enemyX[ind], enemyY[ind] + 2);
        cout << "    ";
        gotoxy(enemyX[ind], enemyY[ind] + 3);
        cout << "    ";
    }
}

//적 자동차의 위치를 초기화하는 함수
void resetEnemy(int ind)
{
    eraseEnemy(ind); //eraseEnemy함수로 적 자동차를 함수에서 지움
    enemyY[ind] = 1; // 적 자동차의 y좌표를 1로 세팅
    genEnemy(ind); // genEnemy 함수로 x좌표를 무작위로 생성
}

//자동차를 콘솔화면에 출력하는 함수
void drawCar()
{
    for (int i = 0; i < 4; i++) //자동차의 각 행을 처리
    {
        for (int j = 0; j < 4; j++) //행의 각 열을 처리
        {
            //커서를 자동차의 각 부분이 그려질 위치로 이동
            gotoxy(j + carPos, i + 22);
            //j + carPos : 자동차 화면 중앙 고정
            //i+22 : 22번째 행부터 출력되도록 고정
            cout << car[i][j]; //car 출력
        }
    }
}

//콘솔 화면에서 자동차를 지우는 함수, drawCar과 로직 유사
void eraseCar()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            gotoxy(j + carPos, i + 22);
            cout << " "; //car 지움
        }
    }
}

//자동차와 적과의 충돌을 감지하는 함수
int collision()
{
    for (int i = 0; i < 3; i++) {
        // 적이 활성화되어 있고, 적의 Y 좌표가 23 이상인 경우 (화면 하단에 도달했을 때)
        if (enemyFlag[i] == true && enemyY[i] + 4 >= 23) {
            // 적의 X 좌표가 자동차의 X 좌표 범위 내에 있는지 확인
            if (enemyX[i] + 4 - carPos >= 0 && enemyX[i] + 4 - carPos < 9) {
                return i; // 충돌한 적의 인덱스를 반환
            }
        }
    }
    return -1; // 충돌이 없을 경우 -1 반환
}

//게임 종료를 출력하는 함수
void gameover()
{
    //현재 score가 max_score보다 크면 max_score update
    if (max_score < score)
    {
        max_score = score;
    }
    system("cls"); //게임 진행 콘솔 화면 clear

    /*게임오버, 스코어, 안내메시지 출력*/
    cout << endl;
    cout << "\t\t--------------------------" << endl;
    cout << "\t\t-------- Game Over -------" << endl;
    cout << "\t\t--------------------------" << endl
        << endl;
    cout << "\t\t"
        << "Your Score is: " << score << endl;
    cout << endl
        << "\t\tPress any key to go back to menu.";
    _getch(); //아무 키나 눌러도 함수 종료, 메인메뉴로
}

//콘솔 화면에 게임 점수를 업데이트하는 함수
void updateScore()
{
    gotoxy(WIN_WIDTH + 7, 5);
    cout << "Score: " << score << endl;
}

//사용자에게 게임 진행 방법 표시하는 Instruction 창 출력 함수
void instructions()
{
    system("cls"); //clear
    /*게임 사용 설명서 출력*/
    cout << "Instructions";
    cout << "\n----------------";
    cout << "\n Avoid Cars by moving left or right. ";
    cout << "\n Eat Items to play slowly or have more chance."; //추가설명
    cout << "\n\n Press 'a' to move left";
    cout << "\n Press 'd' to move right";
    cout << "\n Press 'escape' to exit";
    cout << "\n\nPress any key to go back to menu";
    _getch(); //키 입력시 나가짐
}

// 옵션 3의 Tear화면 구성 함수
void Tear()
{
    string TEAR; //문자열 TEAR 선언

    system("cls"); //화면 클리어

    //max_score이 49이하면 브론즈
    if (max_score <= 49)
    {
        TEAR = "*BRONZE*";
    }
    //max_score이 50~99 이하면 실버
    else if (50 <= max_score && max_score <= 99)
    {
        TEAR = "**SILVER**";
    }
    //max_score이 100~129 이하면 골드
    else if (100 <= max_score && max_score <= 129)
    {
        TEAR = "***GOLD***";
    }
    //max_score이 130~149 이하면 다이아몬드
    else if (130 <= max_score && max_score <= 149)
    {
        TEAR = "****DIAMOND****";
    }
    //max_score이 150 이상이면 마스터
    else if (150 >= max_score)
    {
        TEAR = "*****MASTER*****";
    }
    //현재 티어 출력
    cout << "Your tear is " << endl;
    cout << TEAR << endl;
    //최고 점수 출력
    gotoxy(0, 5);
    cout << "Your heigest score is " << score << endl;

    /*티어 기준표 출력*/
    gotoxy(WIN_WIDTH + 7, 3);
    cout << "   tear" << endl;
    gotoxy(WIN_WIDTH + 7, 5);
    cout << "    150 ~ : *****MASTER*****" << endl;
    gotoxy(WIN_WIDTH + 7, 6);
    cout << "130 ~ 149 :  ****DIAMOND****" << endl;
    gotoxy(WIN_WIDTH + 7, 7);
    cout << "100 ~ 129 :    ***GOLD***" << endl;
    gotoxy(WIN_WIDTH + 7, 8);
    cout << "  50 ~ 99 :    **SILVER**" << endl;
    gotoxy(WIN_WIDTH + 7, 9);
    cout << "   0 ~ 49 :     *BRONZE*" << endl;

    gotoxy(0, 15);
    cout << "\n\nPress any key to go back to menu";
    _getch(); //아무키나 누르면 나가짐
}

class Item { // 아이템 클래스 선언
public:
    virtual void genItem(int ind) = 0; //아이템을 생성하는 genItem 함수를 순수 가상 함수로 선언
    virtual void drawItem(int ind) = 0; //아이템을 그리는 drawItem 함수를 순수 가상 함수로 선언
    virtual void eraseItem(int ind) = 0; //아이템을 지우는 eraseItem 함수를 순수 가상 함수로 선언
    virtual void resetItem(int ind) = 0; //아이템을 리셋하는 resetItem 함수를 순수 가상 함수로 선언
    virtual int itemCollision() = 0; //아이템과 자동차가 충돌했는지 확인하는 itemCollision 함수를 순수 가상 함수로 선언
};

class SlowItem : public Item { //Item 클래스를 public으로 상속받는 SlowItem 클래스 선언
public:
    void genItem(int ind) { //아이템을 생성하는 genItem 함수를 오버라이딩
        SlowitemX[ind] = 17 + rand() % (33); // 아이템이 생성될 x좌표를 랜덤으로 설정
        SlowitemY[ind] = 1; // 아이템이 맨 위에서 생성되도록 초기화
    }

    void drawItem(int ind) { //아이템을 그리는 drawItem 함수를 오버라이딩
        if (SlowitemFlag[ind] == true) { // 아이템이 활성화되어 있을 경우
            gotoxy(SlowitemX[ind], SlowitemY[ind]); // 아이템의 x, y좌표로 이동
            cout << "SL"; // 아이템 그리기
            gotoxy(SlowitemX[ind], SlowitemY[ind] + 1); // y좌표를 증가하여 다음 줄로 이동
            cout << "OW"; // 아이템 그리기
        }
    }

    void eraseItem(int ind) { //아이템을 지우는 eraseItem 함수를 오버라이딩
        if (SlowitemFlag[ind] == true) { // 아이템이 활성화되어 있을 경우
            gotoxy(SlowitemX[ind], SlowitemY[ind]); // 아이템의 x, y좌표로 이동
            cout << "  "; // 공백으로 아이템 지우기
            gotoxy(SlowitemX[ind], SlowitemY[ind] + 1); // y좌료를 증가하여 다음 줄로 이동
            cout << "  "; // 공백으로 아이템 지우기
        }
    }

    void resetItem(int ind) { //아이템을 리셋하는 resetItem 함수를 오버라이딩
        eraseItem(ind); // 아이템 지우기
        SlowitemY[ind] = 1; // 아이템 y좌표 초기화
        genItem(ind); // 아이템 재생성하기 위해 genItem 함수 호출
    }

    int itemCollision() { //아이템과 자동차가 충돌했는지 확인하는 itemCollision 함수를 오버라이딩
        for (int i = 0; i < 3; i++) { // 아이템의 개수만큼 반복
            if (SlowitemFlag[i] == true && SlowitemY[i] + 2 >= 23) { // 아이템이 활성화되어 있고, y좌표 + 2가 23 이상일 경우
                if (SlowitemX[i] + 2 - carPos >= 0 && SlowitemX[i] + 2 - carPos < 9) { // 아이템의 x좌표 + 2 - 자동차의 x좌표가 0 이상 9 미만일 경우
                    return i; // 충돌한 아이템의 인덱스를 반환
                }
            }
        }
        return -1; // 충돌이 없을 경우 -1 반환
    }
};

class HPItem : public Item { //Item 클래스를 public으로 상속받는 HPItem 클래스 선언
public:
    void genItem(int ind) { //아이템을 생성하는 genItem 함수를 오버라이딩
        HPitemX[ind] = 17 + rand() % (33); // 아이템이 생성될 x좌표를 랜덤으로 설정
        HPitemY[ind] = 1; // 아이템이 맨 위에서 생성되도록 초기화
    }

    void drawItem(int ind) { //아이템을 그리는 drawItem 함수를 오버라이딩
        if (HPitemFlag[ind] == true) { // 아이템이 활성화되어 있을 경우
            gotoxy(HPitemX[ind], HPitemY[ind]); // 아이템의 x, y좌표로 이동
            cout << "HH"; // 아이템 그리기
            gotoxy(HPitemX[ind], HPitemY[ind] + 1); // y좌표를 증가하여 다음 줄로 이동
            cout << "PP"; // 아이템 그리기
        }
    }

    void eraseItem(int ind) { //아이템을 지우는 eraseItem 함수를 오버라이딩
        if (HPitemFlag[ind] == true) { // 아이템이 활성화되어 있을 경우
            gotoxy(HPitemX[ind], HPitemY[ind]); // 아이템의 x, y좌표로 이동
            cout << "  "; // 공백으로 아이템 지우기
            gotoxy(HPitemX[ind], HPitemY[ind] + 1); // y좌표를 증가하여 다음 줄로 이동
            cout << "  "; // 공백으로 아이템 지우기
        }
    }

    void resetItem(int ind) { //아이템을 리셋하는 resetItem 함수를 오버라이딩
        eraseItem(ind); // 아이템 지우기
        HPitemY[ind] = 1; // 아이템 y좌표 초기화
        genItem(ind); // 아이템 재생성하기 위해 genItem 함수 호출
    }

    int itemCollision() { //아이템과 자동차가 충돌했는지 확인하는 itemCollision 함수를 오버라이딩
        for (int i = 0; i < 3; i++) { // 아이템의 개수만큼 반복
            if (HPitemFlag[i] == true && HPitemY[i] + 2 >= 23) { // 아이템이 활성화되어 있고, y좌표 + 2가 23 이상일 경우
                if (HPitemX[i] + 2 - carPos >= 0 && HPitemX[i] + 2 - carPos < 9) { // 아이템의 x좌표 + 2 - 자동차의 x좌표가 0 이상 9 미만일 경우
                    return i; // 충돌한 아이템의 인덱스를 반환
                }
            }
        }
        return -1; // 충돌이 없을 경우 -1 반환
    }
};

SlowItem Slow; // SlowItem 클래스의 객체 Slow 선언
HPItem HP; // HPItem 클래스의 객체 HP 선언


/*게임 플레이 함수*/
void play()
{
    carPos = -1 + WIN_WIDTH / 2; //자동차 초기 좌표 고정
    score = 0; //스코어 초기화
    Heart = 5; // 게임 시작 시 목숨 초기화
    enemyFlag[0] = 1; //적 출력 상태 표시 초기화, 첫번째 적의 Flag 1-> 적 존재
    enemyFlag[1] = 0; //적 출력 상태 표시 초기화, 두번째 적의 Flag 0-> 적 없음
    enemyY[0] = enemyY[1] = 1; //모든 적의 초기 y 좌표 1

    SlowitemFlag[0] = 1;
    SlowitemY[0] = 1;
    HPitemFlag[0] = 1;
    HPitemY[0] = 1;
    Slow.genItem(0);
    HP.genItem(0); // HP 아이템도 초기화

    system("cls"); //clear
    drawBorder(); //플레이 화면 경계 draw
    updateScore(); //점수 출력
    genEnemy(0); //첫번째 적 자동차 초기 x좌표 랜덤 설정
    genEnemy(1); //두번째 적 자동차 초기 x좌표 랜덤 설정

    /*게임 이름, 동작법 출력*/
    gotoxy(WIN_WIDTH + 7, 2);
    cout << "Car Game";
    gotoxy(WIN_WIDTH + 6, 4);
    cout << "----------";
    gotoxy(WIN_WIDTH + 6, 6);
    cout << "----------";
    gotoxy(WIN_WIDTH + 7, 12);
    cout << "Control ";
    gotoxy(WIN_WIDTH + 7, 13);
    cout << "-------- ";
    gotoxy(WIN_WIDTH + 2, 14);
    cout << " A Key - Left";
    gotoxy(WIN_WIDTH + 2, 15);
    cout << " D Key - Right";

    /*게임 시작 방법 출력*/
    gotoxy(18, 5);
    cout << "Press any key to start";
    _getch();
    gotoxy(18, 5);
    cout << "                      "; //지움


    s = defaultSpeed; //속도 초기값을 디폴트값으로 설정
    while (1)
    {
        // Slow 아이템 효과 지속 시간 체크
        if (isSlowed && clock() - slowStartTime > slowDuration)
        {
            s = defaultSpeed; // 원래 속도로 복원
            isSlowed = false; // flag 내림
        }

        gotoxy(WIN_WIDTH + 2, 16);
        cout << "남은 목숨 : " << Heart; //실시간 목숨 출력

        //키보드 값 입력되면 시작
        if (_kbhit())
        {
            char ch = _getch(); //키보드 입력값 ch로 받음
            if (ch == 'a' || ch == 'A') //키보드 a or A이면 왼쪽 이동
            {
                if (carPos > 18)
                {
                    carPos -= 4;
                }
            }
            if (ch == 'd' || ch == 'D') //키보드 d or D이면 오른쪽 이동
            {
                if (carPos < 50)
                {
                    carPos += 4;
                }
            }
            if (ch == 27) //키보드 Esc면 break
            {
                break;
            }
        }

        drawCar(); //사용자 자동차 생성
        drawEnemy(0); //1번째 적 생성
        drawEnemy(1); //2번째 적 생성

        Slow.drawItem(0); //슬로우 아이템 생성
        HP.drawItem(0); //HP 아이템 생성

        int enemyIndex = collision(); //적과의 collision 체크할 변수
        int slowItemIndex = Slow.itemCollision(); //slowItem과의 collision 체크하는 변수
        int hpItemIndex = HP.itemCollision(); //hpItem과의 collision 체크하는 변수

        if (enemyIndex != -1) //enemyIndex가 -1이 아니면 즉 적과 충돌하면
        {
            Heart -= 1; //목숨 하나 감소
            gotoxy(WIN_WIDTH + 2, 17);
            cout << "자동차 충돌 발생!  "; //충돌 메시지 출력
            /*목숨 0개이면 게임 오버 및 리턴*/
            if (Heart <= 0) {
                gameover();
                return;
            }
            resetEnemy(enemyIndex); // 충돌 시 적 리셋
        }

        if (slowItemIndex != -1) //slow 아이템과 충돌시
        {
            slowStartTime = clock(); // Slow 효과 시작 시간
            s = 100; // 느린 속도로 설정
            isSlowed = true; // Slow 효과 활성화

            gotoxy(WIN_WIDTH + 2, 17);
            cout << "SLOW 아이템 획득  "; // 아이템 획득 메시지 출력

            Slow.resetItem(slowItemIndex); // 충돌한 아이템만 리셋
        }

        if (hpItemIndex != -1) //HP 아이템과 충돌시
        {
            gotoxy(WIN_WIDTH + 2, 17);
            cout << "HP 아이템 획득    "; // 아이템 획득 메시지 출력

            /*목숨이 5개 미만일때 목숨 추가 가능*/
            if (Heart < maxHeart) {
                Heart++;
            }
            HP.resetItem(hpItemIndex); // 충돌한 아이템만 리셋
        }

        Sleep(s); //게임 진행 속도 증가
        eraseCar(); //사용자 자동차 지움 -> 이동에 따른 프레임 지우기 위해
        eraseEnemy(0); //첫번째 적 지움
        eraseEnemy(1); //두번째 적 지움
        Slow.eraseItem(0); //SLOW 아이템 지움
        HP.eraseItem(0); //HP 아이템 지움

        /* E N E M Y */
        /*적 Y좌표 10되면 적 자동차 새로 출력되도록 함*/
        if (enemyY[0] == 10)
        {
            if (enemyFlag[1] == 0) // 두번째 적 없음(0)상태에서 있음(1)으로 바꿈
            {
                enemyFlag[1] = 1;
            }
        }

        if (enemyFlag[0] == 1) // 첫번째 적 있음(1)상태에서 없음(0)으로 바꿈 
        {
            enemyY[0] += 1; // Y 좌표 증가하여 flag 체크 
        }

        if (enemyFlag[1] == 1) // 두번째 적 있음(1)상태에서 없음(0)으로 바꿈 
        {
            enemyY[1] += 1; // Y 좌표 증가하여 flag 체크 
        }

        /*점수 획득 기준 : 적 자동차 플레이 화면에서 사라질때*/
        //첫번째 적
        if (enemyY[0] > SCREEN_HEIGHT - 4) //기준 영역을 넘을때
        {
            resetEnemy(0); // 적 리셋
            score++; //점수 증가
            updateScore(); //점수판 업데이트
        }
        //두번째 적
        if (enemyY[1] > SCREEN_HEIGHT - 4)
        {
            resetEnemy(1);
            score++;
            updateScore();
        }

        /* I T E M */
        //SlowItem Flag, Y좌표 확인
        if (SlowitemFlag[0] == 1)
        {
            SlowitemY[0] += 1;
        }

        //HPItem Flag, Y좌표 확인
        if (HPitemFlag[0] == 1)
        {
            HPitemY[0] += 1;
        }

        //SlowItem이 플레이 영역 넘으면 리셋
        if (SlowitemY[0] > SCREEN_HEIGHT - 2)
        {
            Slow.resetItem(0);
        }
        //HPItem이 플레이 영역 넘으면 리셋
        if (HPitemY[0] > SCREEN_HEIGHT - 2)
        {
            HP.resetItem(0);
        }

        if (_kbhit()) {
            gotoxy(WIN_WIDTH + 2, 17);
            cout << "                  ";
        }

        //속도 제어
        if (s >= 25) { //max 속도 = 25
            s -= 0.2; // 점점 속도 빨라짐
        }

    }
}

/* MAIN문 */
int main()
{
    setcursor(0, 0); //커서 화면에 표시 안하게 함
    srand((unsigned)time(NULL)); //시드값 초기화

    do
    {
        system("cls"); //화면 클리어하고 시작
        /*아스키코드 아트 - 자동차 그림 출력*/
        gotoxy(10, 2);
        cout << "   ___________";
        gotoxy(10, 3);
        cout << "  -  ----// --|||-- \\         ";
        gotoxy(10, 4);
        cout << " ---- __//____|||____\\____   ";
        gotoxy(10, 5);
        cout << "     | _|    \" | \"   --_  ||";
        gotoxy(10, 6);
        cout << " ----|/ \\______|______/ \\_||";
        gotoxy(10, 7);
        cout << " ______\\_/_____________\\_/_______";
        /* 게임이름 출력 */
        gotoxy(10, 9);
        cout << " ============================================== ";
        gotoxy(10, 10);
        cout << " |||||||           요리조리 차차        |||||||";
        gotoxy(10, 11);
        cout << " ==============================================";
        /*옵션 번호 출력*/
        gotoxy(10, 13);
        cout << "1. Start Game";
        gotoxy(10, 14);
        cout << "2. Instructions";
        gotoxy(10, 15);
        cout << "3. Tear";
        gotoxy(10, 16);
        cout << "4. Quit";
        gotoxy(10, 18);
        cout << "Select option: ";

        char op = _getche(); //키보드값 op로 받음

        if (op == '1') //op 1이면 play(), 게임 화면 시작
            play();
        else if (op == '2') //op 2이면 instructions(), 게임 플레이방법 안내화면 시작
            instructions();
        else if (op == '3') //op 3이면 Tear(), 현재 게임 티어 및 티어 기준 안내 화면 시작
            Tear();
        else if (op == '4') //op 4이면 프로그램 종료
            exit(0);

    } while (1);

    return 0;
}