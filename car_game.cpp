#include <iostream> //��Ʈ���� ���� ����� ����� �����ϴ� ��� ����
#include <conio.h> //�ܼ� ����� ����� �����ϴ� ��ǥ�� ���
#include <direct.h> //���͸� ���۰� ���õ� �Լ����� �����ϴ� ��� ����
#include <windows.h> //Windows API �Լ��� ����ϱ� ���� ��� ����
#include <time.h> //�ð��� ��¥�� ���õ� �Լ����� �����ϴ� ��� ����

using namespace std;

#define SCREEN_WIDTH 90
#define SCREEN_HEIGHT 26
#define WIN_WIDTH 70
double s = 50; // �⺻ �ӵ�
double defaultSpeed = 50; // �⺻ �ӵ� ����
double slowDuration = 2000; // Slow ������ ȿ�� ���� �ð� (�и���)
bool isSlowed = false; // Slow ������ ȿ�� ����
clock_t slowStartTime; // Slow ������ ȿ�� ���� �ð�

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
// HANDLE : �ܼ� ȭ�� ���۸� �ĺ��ϴ� �� ���
// GetStdHandle : ǥ�� ��ġ(�Է�, ���, ����) �� �ϳ��� �ڵ��� ��ȯ
COORD CursorPosition; //x���� y���� ����� ������ ����ü ����

int enemyY[3]; //�� Y��ǥ
int enemyX[3]; //�� X��ǥ
int enemyFlag[3]; //�� Flag�� (�� �����Ǿ����� ���� Ȯ��)

int SlowitemX[3]; //SlowItem X��ǥ
int SlowitemY[3]; //SlowItem Y��ǥ
int SlowitemFlag[3]; //SlowItem Flag��
int HPitemX[3]; //HPItem X��ǥ
int HPitemY[3]; //HPItem Y��ǥ
int HPitemFlag[3]; //HPItem Flag��

int carPos = WIN_WIDTH / 2; //����� �ڵ��� ��ġ ����� ����
int max_score = 0; //�ְ����� �ʱ�ȭ
int score = 0; //score �ʱ�ȭ
int Heart = 5; //����� ��� �ʱⰪ : 5
int maxHeart = 5; //����� �ִ� ��� 5�� ����

/*CAR ����*/
const char car[4][4] = { ' ', '+', '+', ' ',
                        '+', '+', '+', '+',
                        ' ', '+', '+', ' ',
                        '+', '+', '+', '+' };

/*x,y�� ��ǥ�� �Է°����� �޾� �ܼ��� Ŀ���� �Էµ� x,y ��ǥ�� ��ġ�� �̵�*/
void gotoxy(int x, int y)
{
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

/*Ŀ���� ���� ���ο� ũ�⸦ �����ϴ� Ŀ�� ���� �Լ�*/
void setcursor(bool visible, DWORD size)
{
    if (size == 0)
        size = 20;

    CONSOLE_CURSOR_INFO lpCursor;
    lpCursor.bVisible = visible;
    lpCursor.dwSize = size;
    SetConsoleCursorInfo(console, &lpCursor);
}

/*���� �׸��� �Լ�*/
void drawBorder()
{
    //���� ��� �׸�
    for (int i = 0; i < SCREEN_HEIGHT; i++) //ȭ���� ����(SCREEN_HEIGHT)��ŭ �ݺ�
    {
        for (int j = 0; j < 17; j++) //����� �ʺ� 17�� ����
        {
            //�� ���� ���۰� �� �κп� + ���ڸ� ���
            gotoxy(0 + j, i);
            cout << "+";
            gotoxy(WIN_WIDTH - j, i);
            cout << "+";
        }
    }
    //���� ��� �׸�
    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
        gotoxy(SCREEN_WIDTH, i);
        cout << "+";
    }
}

//�� �ڵ����� x��ǥ�� �������� �����ϴ� �Լ�
void genEnemy(int ind)
{
    //17~49������ ������ x��ǥ�� �� �ڵ����� �����ǰ� ����
    //17���� ��Ÿ���� �ϴ� ���� ���� ȭ���� �߾ӿ� ��Ÿ���� �ϱ� ����
    enemyX[ind] = 17 + rand() % (33);
}

//�� �ڵ����� ȭ�鿡 �׸��� �Լ�
void drawEnemy(int ind)
{
    //enemyFlag�� ���� true-> Ŀ���� y�� + 1 �Ǹ� �� �ڵ����� ���°� ��µ�
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

//�� �ڵ����� ȭ�鿡�� ����� �Լ�
void eraseEnemy(int ind)
{
    //enemyFlag�� ���� true-> ȭ�鿡�� �� �ڵ����� ������
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

//�� �ڵ����� ��ġ�� �ʱ�ȭ�ϴ� �Լ�
void resetEnemy(int ind)
{
    eraseEnemy(ind); //eraseEnemy�Լ��� �� �ڵ����� �Լ����� ����
    enemyY[ind] = 1; // �� �ڵ����� y��ǥ�� 1�� ����
    genEnemy(ind); // genEnemy �Լ��� x��ǥ�� �������� ����
}

//�ڵ����� �ܼ�ȭ�鿡 ����ϴ� �Լ�
void drawCar()
{
    for (int i = 0; i < 4; i++) //�ڵ����� �� ���� ó��
    {
        for (int j = 0; j < 4; j++) //���� �� ���� ó��
        {
            //Ŀ���� �ڵ����� �� �κ��� �׷��� ��ġ�� �̵�
            gotoxy(j + carPos, i + 22);
            //j + carPos : �ڵ��� ȭ�� �߾� ����
            //i+22 : 22��° ����� ��µǵ��� ����
            cout << car[i][j]; //car ���
        }
    }
}

//�ܼ� ȭ�鿡�� �ڵ����� ����� �Լ�, drawCar�� ���� ����
void eraseCar()
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            gotoxy(j + carPos, i + 22);
            cout << " "; //car ����
        }
    }
}

//�ڵ����� ������ �浹�� �����ϴ� �Լ�
int collision()
{
    for (int i = 0; i < 3; i++) {
        // ���� Ȱ��ȭ�Ǿ� �ְ�, ���� Y ��ǥ�� 23 �̻��� ��� (ȭ�� �ϴܿ� �������� ��)
        if (enemyFlag[i] == true && enemyY[i] + 4 >= 23) {
            // ���� X ��ǥ�� �ڵ����� X ��ǥ ���� ���� �ִ��� Ȯ��
            if (enemyX[i] + 4 - carPos >= 0 && enemyX[i] + 4 - carPos < 9) {
                return i; // �浹�� ���� �ε����� ��ȯ
            }
        }
    }
    return -1; // �浹�� ���� ��� -1 ��ȯ
}

//���� ���Ḧ ����ϴ� �Լ�
void gameover()
{
    //���� score�� max_score���� ũ�� max_score update
    if (max_score < score)
    {
        max_score = score;
    }
    system("cls"); //���� ���� �ܼ� ȭ�� clear

    /*���ӿ���, ���ھ�, �ȳ��޽��� ���*/
    cout << endl;
    cout << "\t\t--------------------------" << endl;
    cout << "\t\t-------- Game Over -------" << endl;
    cout << "\t\t--------------------------" << endl
        << endl;
    cout << "\t\t"
        << "Your Score is: " << score << endl;
    cout << endl
        << "\t\tPress any key to go back to menu.";
    _getch(); //�ƹ� Ű�� ������ �Լ� ����, ���θ޴���
}

//�ܼ� ȭ�鿡 ���� ������ ������Ʈ�ϴ� �Լ�
void updateScore()
{
    gotoxy(WIN_WIDTH + 7, 5);
    cout << "Score: " << score << endl;
}

//����ڿ��� ���� ���� ��� ǥ���ϴ� Instruction â ��� �Լ�
void instructions()
{
    system("cls"); //clear
    /*���� ��� ���� ���*/
    cout << "Instructions";
    cout << "\n----------------";
    cout << "\n Avoid Cars by moving left or right. ";
    cout << "\n Eat Items to play slowly or have more chance."; //�߰�����
    cout << "\n\n Press 'a' to move left";
    cout << "\n Press 'd' to move right";
    cout << "\n Press 'escape' to exit";
    cout << "\n\nPress any key to go back to menu";
    _getch(); //Ű �Է½� ������
}

// �ɼ� 3�� Tearȭ�� ���� �Լ�
void Tear()
{
    string TEAR; //���ڿ� TEAR ����

    system("cls"); //ȭ�� Ŭ����

    //max_score�� 49���ϸ� �����
    if (max_score <= 49)
    {
        TEAR = "*BRONZE*";
    }
    //max_score�� 50~99 ���ϸ� �ǹ�
    else if (50 <= max_score && max_score <= 99)
    {
        TEAR = "**SILVER**";
    }
    //max_score�� 100~129 ���ϸ� ���
    else if (100 <= max_score && max_score <= 129)
    {
        TEAR = "***GOLD***";
    }
    //max_score�� 130~149 ���ϸ� ���̾Ƹ��
    else if (130 <= max_score && max_score <= 149)
    {
        TEAR = "****DIAMOND****";
    }
    //max_score�� 150 �̻��̸� ������
    else if (150 >= max_score)
    {
        TEAR = "*****MASTER*****";
    }
    //���� Ƽ�� ���
    cout << "Your tear is " << endl;
    cout << TEAR << endl;
    //�ְ� ���� ���
    gotoxy(0, 5);
    cout << "Your heigest score is " << score << endl;

    /*Ƽ�� ����ǥ ���*/
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
    _getch(); //�ƹ�Ű�� ������ ������
}

class Item { // ������ Ŭ���� ����
public:
    virtual void genItem(int ind) = 0; //�������� �����ϴ� genItem �Լ��� ���� ���� �Լ��� ����
    virtual void drawItem(int ind) = 0; //�������� �׸��� drawItem �Լ��� ���� ���� �Լ��� ����
    virtual void eraseItem(int ind) = 0; //�������� ����� eraseItem �Լ��� ���� ���� �Լ��� ����
    virtual void resetItem(int ind) = 0; //�������� �����ϴ� resetItem �Լ��� ���� ���� �Լ��� ����
    virtual int itemCollision() = 0; //�����۰� �ڵ����� �浹�ߴ��� Ȯ���ϴ� itemCollision �Լ��� ���� ���� �Լ��� ����
};

class SlowItem : public Item { //Item Ŭ������ public���� ��ӹ޴� SlowItem Ŭ���� ����
public:
    void genItem(int ind) { //�������� �����ϴ� genItem �Լ��� �������̵�
        SlowitemX[ind] = 17 + rand() % (33); // �������� ������ x��ǥ�� �������� ����
        SlowitemY[ind] = 1; // �������� �� ������ �����ǵ��� �ʱ�ȭ
    }

    void drawItem(int ind) { //�������� �׸��� drawItem �Լ��� �������̵�
        if (SlowitemFlag[ind] == true) { // �������� Ȱ��ȭ�Ǿ� ���� ���
            gotoxy(SlowitemX[ind], SlowitemY[ind]); // �������� x, y��ǥ�� �̵�
            cout << "SL"; // ������ �׸���
            gotoxy(SlowitemX[ind], SlowitemY[ind] + 1); // y��ǥ�� �����Ͽ� ���� �ٷ� �̵�
            cout << "OW"; // ������ �׸���
        }
    }

    void eraseItem(int ind) { //�������� ����� eraseItem �Լ��� �������̵�
        if (SlowitemFlag[ind] == true) { // �������� Ȱ��ȭ�Ǿ� ���� ���
            gotoxy(SlowitemX[ind], SlowitemY[ind]); // �������� x, y��ǥ�� �̵�
            cout << "  "; // �������� ������ �����
            gotoxy(SlowitemX[ind], SlowitemY[ind] + 1); // y�·Ḧ �����Ͽ� ���� �ٷ� �̵�
            cout << "  "; // �������� ������ �����
        }
    }

    void resetItem(int ind) { //�������� �����ϴ� resetItem �Լ��� �������̵�
        eraseItem(ind); // ������ �����
        SlowitemY[ind] = 1; // ������ y��ǥ �ʱ�ȭ
        genItem(ind); // ������ ������ϱ� ���� genItem �Լ� ȣ��
    }

    int itemCollision() { //�����۰� �ڵ����� �浹�ߴ��� Ȯ���ϴ� itemCollision �Լ��� �������̵�
        for (int i = 0; i < 3; i++) { // �������� ������ŭ �ݺ�
            if (SlowitemFlag[i] == true && SlowitemY[i] + 2 >= 23) { // �������� Ȱ��ȭ�Ǿ� �ְ�, y��ǥ + 2�� 23 �̻��� ���
                if (SlowitemX[i] + 2 - carPos >= 0 && SlowitemX[i] + 2 - carPos < 9) { // �������� x��ǥ + 2 - �ڵ����� x��ǥ�� 0 �̻� 9 �̸��� ���
                    return i; // �浹�� �������� �ε����� ��ȯ
                }
            }
        }
        return -1; // �浹�� ���� ��� -1 ��ȯ
    }
};

class HPItem : public Item { //Item Ŭ������ public���� ��ӹ޴� HPItem Ŭ���� ����
public:
    void genItem(int ind) { //�������� �����ϴ� genItem �Լ��� �������̵�
        HPitemX[ind] = 17 + rand() % (33); // �������� ������ x��ǥ�� �������� ����
        HPitemY[ind] = 1; // �������� �� ������ �����ǵ��� �ʱ�ȭ
    }

    void drawItem(int ind) { //�������� �׸��� drawItem �Լ��� �������̵�
        if (HPitemFlag[ind] == true) { // �������� Ȱ��ȭ�Ǿ� ���� ���
            gotoxy(HPitemX[ind], HPitemY[ind]); // �������� x, y��ǥ�� �̵�
            cout << "HH"; // ������ �׸���
            gotoxy(HPitemX[ind], HPitemY[ind] + 1); // y��ǥ�� �����Ͽ� ���� �ٷ� �̵�
            cout << "PP"; // ������ �׸���
        }
    }

    void eraseItem(int ind) { //�������� ����� eraseItem �Լ��� �������̵�
        if (HPitemFlag[ind] == true) { // �������� Ȱ��ȭ�Ǿ� ���� ���
            gotoxy(HPitemX[ind], HPitemY[ind]); // �������� x, y��ǥ�� �̵�
            cout << "  "; // �������� ������ �����
            gotoxy(HPitemX[ind], HPitemY[ind] + 1); // y��ǥ�� �����Ͽ� ���� �ٷ� �̵�
            cout << "  "; // �������� ������ �����
        }
    }

    void resetItem(int ind) { //�������� �����ϴ� resetItem �Լ��� �������̵�
        eraseItem(ind); // ������ �����
        HPitemY[ind] = 1; // ������ y��ǥ �ʱ�ȭ
        genItem(ind); // ������ ������ϱ� ���� genItem �Լ� ȣ��
    }

    int itemCollision() { //�����۰� �ڵ����� �浹�ߴ��� Ȯ���ϴ� itemCollision �Լ��� �������̵�
        for (int i = 0; i < 3; i++) { // �������� ������ŭ �ݺ�
            if (HPitemFlag[i] == true && HPitemY[i] + 2 >= 23) { // �������� Ȱ��ȭ�Ǿ� �ְ�, y��ǥ + 2�� 23 �̻��� ���
                if (HPitemX[i] + 2 - carPos >= 0 && HPitemX[i] + 2 - carPos < 9) { // �������� x��ǥ + 2 - �ڵ����� x��ǥ�� 0 �̻� 9 �̸��� ���
                    return i; // �浹�� �������� �ε����� ��ȯ
                }
            }
        }
        return -1; // �浹�� ���� ��� -1 ��ȯ
    }
};

SlowItem Slow; // SlowItem Ŭ������ ��ü Slow ����
HPItem HP; // HPItem Ŭ������ ��ü HP ����


/*���� �÷��� �Լ�*/
void play()
{
    carPos = -1 + WIN_WIDTH / 2; //�ڵ��� �ʱ� ��ǥ ����
    score = 0; //���ھ� �ʱ�ȭ
    Heart = 5; // ���� ���� �� ��� �ʱ�ȭ
    enemyFlag[0] = 1; //�� ��� ���� ǥ�� �ʱ�ȭ, ù��° ���� Flag 1-> �� ����
    enemyFlag[1] = 0; //�� ��� ���� ǥ�� �ʱ�ȭ, �ι�° ���� Flag 0-> �� ����
    enemyY[0] = enemyY[1] = 1; //��� ���� �ʱ� y ��ǥ 1

    SlowitemFlag[0] = 1;
    SlowitemY[0] = 1;
    HPitemFlag[0] = 1;
    HPitemY[0] = 1;
    Slow.genItem(0);
    HP.genItem(0); // HP �����۵� �ʱ�ȭ

    system("cls"); //clear
    drawBorder(); //�÷��� ȭ�� ��� draw
    updateScore(); //���� ���
    genEnemy(0); //ù��° �� �ڵ��� �ʱ� x��ǥ ���� ����
    genEnemy(1); //�ι�° �� �ڵ��� �ʱ� x��ǥ ���� ����

    /*���� �̸�, ���۹� ���*/
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

    /*���� ���� ��� ���*/
    gotoxy(18, 5);
    cout << "Press any key to start";
    _getch();
    gotoxy(18, 5);
    cout << "                      "; //����


    s = defaultSpeed; //�ӵ� �ʱⰪ�� ����Ʈ������ ����
    while (1)
    {
        // Slow ������ ȿ�� ���� �ð� üũ
        if (isSlowed && clock() - slowStartTime > slowDuration)
        {
            s = defaultSpeed; // ���� �ӵ��� ����
            isSlowed = false; // flag ����
        }

        gotoxy(WIN_WIDTH + 2, 16);
        cout << "���� ��� : " << Heart; //�ǽð� ��� ���

        //Ű���� �� �ԷµǸ� ����
        if (_kbhit())
        {
            char ch = _getch(); //Ű���� �Է°� ch�� ����
            if (ch == 'a' || ch == 'A') //Ű���� a or A�̸� ���� �̵�
            {
                if (carPos > 18)
                {
                    carPos -= 4;
                }
            }
            if (ch == 'd' || ch == 'D') //Ű���� d or D�̸� ������ �̵�
            {
                if (carPos < 50)
                {
                    carPos += 4;
                }
            }
            if (ch == 27) //Ű���� Esc�� break
            {
                break;
            }
        }

        drawCar(); //����� �ڵ��� ����
        drawEnemy(0); //1��° �� ����
        drawEnemy(1); //2��° �� ����

        Slow.drawItem(0); //���ο� ������ ����
        HP.drawItem(0); //HP ������ ����

        int enemyIndex = collision(); //������ collision üũ�� ����
        int slowItemIndex = Slow.itemCollision(); //slowItem���� collision üũ�ϴ� ����
        int hpItemIndex = HP.itemCollision(); //hpItem���� collision üũ�ϴ� ����

        if (enemyIndex != -1) //enemyIndex�� -1�� �ƴϸ� �� ���� �浹�ϸ�
        {
            Heart -= 1; //��� �ϳ� ����
            gotoxy(WIN_WIDTH + 2, 17);
            cout << "�ڵ��� �浹 �߻�!  "; //�浹 �޽��� ���
            /*��� 0���̸� ���� ���� �� ����*/
            if (Heart <= 0) {
                gameover();
                return;
            }
            resetEnemy(enemyIndex); // �浹 �� �� ����
        }

        if (slowItemIndex != -1) //slow �����۰� �浹��
        {
            slowStartTime = clock(); // Slow ȿ�� ���� �ð�
            s = 100; // ���� �ӵ��� ����
            isSlowed = true; // Slow ȿ�� Ȱ��ȭ

            gotoxy(WIN_WIDTH + 2, 17);
            cout << "SLOW ������ ȹ��  "; // ������ ȹ�� �޽��� ���

            Slow.resetItem(slowItemIndex); // �浹�� �����۸� ����
        }

        if (hpItemIndex != -1) //HP �����۰� �浹��
        {
            gotoxy(WIN_WIDTH + 2, 17);
            cout << "HP ������ ȹ��    "; // ������ ȹ�� �޽��� ���

            /*����� 5�� �̸��϶� ��� �߰� ����*/
            if (Heart < maxHeart) {
                Heart++;
            }
            HP.resetItem(hpItemIndex); // �浹�� �����۸� ����
        }

        Sleep(s); //���� ���� �ӵ� ����
        eraseCar(); //����� �ڵ��� ���� -> �̵��� ���� ������ ����� ����
        eraseEnemy(0); //ù��° �� ����
        eraseEnemy(1); //�ι�° �� ����
        Slow.eraseItem(0); //SLOW ������ ����
        HP.eraseItem(0); //HP ������ ����

        /* E N E M Y */
        /*�� Y��ǥ 10�Ǹ� �� �ڵ��� ���� ��µǵ��� ��*/
        if (enemyY[0] == 10)
        {
            if (enemyFlag[1] == 0) // �ι�° �� ����(0)���¿��� ����(1)���� �ٲ�
            {
                enemyFlag[1] = 1;
            }
        }

        if (enemyFlag[0] == 1) // ù��° �� ����(1)���¿��� ����(0)���� �ٲ� 
        {
            enemyY[0] += 1; // Y ��ǥ �����Ͽ� flag üũ 
        }

        if (enemyFlag[1] == 1) // �ι�° �� ����(1)���¿��� ����(0)���� �ٲ� 
        {
            enemyY[1] += 1; // Y ��ǥ �����Ͽ� flag üũ 
        }

        /*���� ȹ�� ���� : �� �ڵ��� �÷��� ȭ�鿡�� �������*/
        //ù��° ��
        if (enemyY[0] > SCREEN_HEIGHT - 4) //���� ������ ������
        {
            resetEnemy(0); // �� ����
            score++; //���� ����
            updateScore(); //������ ������Ʈ
        }
        //�ι�° ��
        if (enemyY[1] > SCREEN_HEIGHT - 4)
        {
            resetEnemy(1);
            score++;
            updateScore();
        }

        /* I T E M */
        //SlowItem Flag, Y��ǥ Ȯ��
        if (SlowitemFlag[0] == 1)
        {
            SlowitemY[0] += 1;
        }

        //HPItem Flag, Y��ǥ Ȯ��
        if (HPitemFlag[0] == 1)
        {
            HPitemY[0] += 1;
        }

        //SlowItem�� �÷��� ���� ������ ����
        if (SlowitemY[0] > SCREEN_HEIGHT - 2)
        {
            Slow.resetItem(0);
        }
        //HPItem�� �÷��� ���� ������ ����
        if (HPitemY[0] > SCREEN_HEIGHT - 2)
        {
            HP.resetItem(0);
        }

        if (_kbhit()) {
            gotoxy(WIN_WIDTH + 2, 17);
            cout << "                  ";
        }

        //�ӵ� ����
        if (s >= 25) { //max �ӵ� = 25
            s -= 0.2; // ���� �ӵ� ������
        }

    }
}

/* MAIN�� */
int main()
{
    setcursor(0, 0); //Ŀ�� ȭ�鿡 ǥ�� ���ϰ� ��
    srand((unsigned)time(NULL)); //�õ尪 �ʱ�ȭ

    do
    {
        system("cls"); //ȭ�� Ŭ�����ϰ� ����
        /*�ƽ�Ű�ڵ� ��Ʈ - �ڵ��� �׸� ���*/
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
        /* �����̸� ��� */
        gotoxy(10, 9);
        cout << " ============================================== ";
        gotoxy(10, 10);
        cout << " |||||||           �丮���� ����        |||||||";
        gotoxy(10, 11);
        cout << " ==============================================";
        /*�ɼ� ��ȣ ���*/
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

        char op = _getche(); //Ű���尪 op�� ����

        if (op == '1') //op 1�̸� play(), ���� ȭ�� ����
            play();
        else if (op == '2') //op 2�̸� instructions(), ���� �÷��̹�� �ȳ�ȭ�� ����
            instructions();
        else if (op == '3') //op 3�̸� Tear(), ���� ���� Ƽ�� �� Ƽ�� ���� �ȳ� ȭ�� ����
            Tear();
        else if (op == '4') //op 4�̸� ���α׷� ����
            exit(0);

    } while (1);

    return 0;
}