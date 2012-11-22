/* sudoku0.cpp */
/* ���j: �ł��邾���V���v���ɏ��Ȃ��s���ŏ��� */

#include <stdio.h>
#include <time.h>
#include <stdlib.h>

const int BoardSize = 9; /* ���Ƃ̃T�C�Y */
const int BoardSizeSub = 3; /* = sqrt(BoardSize) */
typedef int FlagsType; /* long long �ɂ���΁A64x64�܂ł����� */
typedef int CountType; /* long long �ɂ���΁A9.2e+18�܂Ő������� */
const FlagsType MaskFlags = 0x1ff; /* == (1 << BoardSize) - 1 */
const CountType CountTypeMax = 0x7fffffff;

class Board {
public:
    unsigned char num[BoardSize][BoardSize];
    void print() const;
};

class SolveSudoku {
public:
    int init(const Board &quiz);
    CountType solve(Board ans[], CountType limit, CountType offset);
    enum { ErrorInvalidBoard = 1, ErrorInvalidNumber };
protected:
    void storeResult();
    FlagsType checkCell(int x0, int y0) const;
    void count(int x, int y);
    int intLog2(int i) const;
    static const int ErrorCodeReachLimit = 1;
    FlagsType board[BoardSize][BoardSize];
    CountType n, nLimit; /* �����𐔂��� */
    Board *answer; /* �������i�[����z�� */
};

void Board::print() const
{
    for (int y = 0; y < BoardSize; y++) {
        for (int x = 0; x < BoardSize; x++)
            printf("%3d ", num[y][x]);
        printf("\n");
    }
}

int SolveSudoku::init(const Board &quiz)
{
    for (int y = 0; y < BoardSize; y++) {
        for (int x = 0; x < BoardSize; x++)
            board[y][x] = (FlagsType) 0;
    }
    for (int y = 0; y < BoardSize; y++) {
        for (int x = 0; x < BoardSize; x++) {
            FlagsType flags = (FlagsType) 0;
            if (quiz.num[y][x] > 0) {
                if (quiz.num[y][x] > BoardSize)
                    return ErrorInvalidNumber;
                flags = ((FlagsType) 1) << (quiz.num[y][x] - 1);
                if ((checkCell(x, y) & flags) != (FlagsType) 0)
                    return ErrorInvalidBoard;
            }
            board[y][x] = flags;
        }
    }
    return 0;
}

FlagsType SolveSudoku::checkCell(int x0, int y0) const
{
    FlagsType flags = (FlagsType) 0;
    for (int i = 0; i < BoardSize; i++)
        flags |= board[y0][i] | board[i][x0];
    x0 -= x0 % BoardSizeSub;
    y0 -= y0 % BoardSizeSub;
    for (int y = 0; y < BoardSizeSub; y++) {
        for (int x = 0; x < BoardSizeSub; x++)
            flags |= board[y0 + y][x0 + x];
    }
    return flags;
}

CountType SolveSudoku::solve(Board ans[], CountType limit, CountType offset)
/* limit  == -1 �� limit  = CountTypeMax �̑�p */
/* offset == -1 �� offset = CountTypeMax �̑�p */
/* ���ʂ̏ꍇ�Aans��2���p�ӂ��āAlimit=2, offset=0 ... �����1���Ԃ��Ă���ΐ��� */
/* ���̌��𐔂��邾���Ȃ�Alimit=0, offset=-1, r=NULL ��OK */
/* �Ԓl - offset == limit �ł͏I�� */
{
    if (limit  == (CountType) -1) limit  = CountTypeMax;
    if (offset == (CountType) -1) offset = CountTypeMax;
    n = - offset;
    nLimit = limit;
    answer = ans;
    try {
        count(0, 0);
    } catch (int e) {
        if (e != ErrorCodeReachLimit)
            throw e;
        /* ReachLimit�̎��͂Ȃɂ����Ȃ��ŏI�� */
    }
    return n + offset;
}

void SolveSudoku::count(int x, int y)
/* �q���g�Ƃ��ĒT���n�߂̈ʒu�������Ă��i���̂ق������������j */
{
    while (y < BoardSize && board[y][x] != (FlagsType) 0) {
        x++;
        if (x >= BoardSize) {
            x = 0;
            y++;
        }
    }
    if (y >= BoardSize) { /* �󂫂����ׂĖ��܂����I */
        if (n >= (CountType) 0)
            storeResult();
        n++;
        if (n >= nLimit)
            throw ErrorCodeReachLimit;
        return;
    }
    FlagsType flags = checkCell(x, y) ^ MaskFlags;
    if (flags != (FlagsType) 0) { /* ������u�����Ƃ��ł��� */
        for (FlagsType i = (FlagsType) 1; i < MaskFlags; i <<= 1) {
            if ((flags & i) != (FlagsType) 0) {
                board[y][x] = i;
                count(x, y); /* �ċA�Ăяo�� */
            }
        }
        board[y][x] = (FlagsType) 0;
    }
}

int SolveSudoku::intLog2(int i) const
{
    int n;
    for (n = 0; i > ((FlagsType) 1) << n; n++);
    return n;
}

void SolveSudoku::storeResult()
{
    for (int y = 0; y < BoardSize; y++) {
        for (int x = 0; x < BoardSize; x++)
            answer[n].num[y][x] = intLog2(board[y][x]) + 1;
    }
}

int solver(const char *filepath, int offset)
/* �蔲��: limit��5�Œ�, offset������n��int�Œ� */
/* �t�@�C���ǂݍ��ݕ���������Ă��Ȃ��̂́A�ė��p���鉿�l���Ȃ��ƍl���Ă��邽�� */
{
    FILE *fp = fopen(filepath, "r");
    char buf[1024];
    int i, n, xy, num;
    const int Limit = 5;
    SolveSudoku ss;
    Board board, ans[5];

    /* �t�@�C���ǂݍ��ݕ� */
    if (fp == NULL) {
        fprintf(stderr, "fopen error: %s\n", filepath);
        return 1;
    }
    i = fread(buf, 1, (sizeof buf) - 2, fp);
    fclose(fp);
    buf[i] = '\0';
    for (xy = i = 0; xy < BoardSize * BoardSize; i++) {
        num = -1;
        if (buf[i] == '\0') {
            fprintf(stderr, "load error: %s\n", filepath);
            return 1;
        }
        if (buf[i] == '\n') {
            if (xy % BoardSize != 0) {
                fprintf(stderr, "LF error: %s\n", filepath);
                return 1;
            }
            continue;
        }
        if (buf[i] == ' ') continue;
        if (buf[i] == '-' || buf[i] == '.') num = 0;
        if ('1' <= buf[i] && buf[i] <= '9') num = buf[i] - '0';
        if (num < 0) {
            fprintf(stderr, "charactor error: %s (buf[%d]=0x%02X)\n", filepath, i, buf[i]);
            return 1;
        }
        board.num[xy / BoardSize][xy % BoardSize] = num;
        xy++;
    }

    /* ���������Č��ʂ�\�� */
    if (ss.init(board) != 0) {
        fprintf(stderr, "quiz error: %s\n", filepath);
        return 1;
    }
    if (offset < 0)
        offset = CountTypeMax - Limit;
    n = ss.solve(ans, Limit, offset);
    printf("n=%d%s (%.3f[sec])\n", n,
        (n < offset + Limit) ? "" : "(or over)", clock() / (double) CLOCKS_PER_SEC);
    for (i = offset; i < n; i++) {
        printf("\nans[%d]:\n", i);
        ans[i - offset].print();
    }
    return 0;
}

int main(int argc, const char **argv)
{
    if (argc == 2) return solver(argv[1], 0);
    if (argc == 3) return solver(argv[1], strtol(argv[2], NULL, 0));
    fprintf(stderr, "usage>sudoku quiz-file [offset]\n");
    return 1;
}

/*

solver()�ɂ��āA
>C++�v���O���}�Ƃ��Ă͂��߂�solver����t�@�C���ǂݍ��݂�Ɨ�������Board��load���\�b�h���炢�ɂ��ė~�����c�D
�Ƃ����w�E���󂯂āA���P��������Ă��炢�܂����B���������Ƃ��Ă�151�s�ڂ̗��R�ɂ��A��������̂܂܂ɂ��Ă����܂��B

*/
