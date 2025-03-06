#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// 为每个终结符设置种类
typedef enum 
{
    TK_PUNCT, // 操作符
    TK_NUM,   // 数字
    TK_EOF,   // 文件结束符
} TokenKind;

// 终结符结构体
typedef struct Token Token;
struct Token
{
    TokenKind kind;   // 终结符种类
    Token *Next;      // 下一个Token
    int Val;          // 终结符的值
    char *Loc;        // 终结符的位置
    int Len;          // 终结符的长度
};

/// @brief 输出错误信息
/// static表示该函数为静态函数，只在当前文件可见
/// @param Fmt 传入字符串
/// @param ... 可变参数，表示Fmt后面死哦用参数
static void error(char *Fmt, ...) 
{
    // 定一个va_list变量
    va_list VA;
    // VA获取Fmt后面的所有参数
    va_start(VA, Fmt);
    // vfprintf可以输出va_list类型参数
    vfprintf(stderr, Fmt, VA);
    // 结束va_start
    va_end(VA);
    exit(1);
}

// 生成新的Token
static Token *new_token(TokenKind King, char *Start, char *End)
{
    // 分配1个Token的内存空间
    Token *Tok = calloc(1, sizeof(Token));
    Tok->kind = King;
    Tok->Loc = Start;
    Tok->Len = End - Start;
    return Tok;
}

// 终结符解析
static Token *tokenize(char *P)
{
    Token Head = {};
    Token *Cur = &Head;

    while(*P)
    {
        // 跳过所有空白、回车
        if(isspace(*P))
        {
            ++P;
            continue;
        }

        // 解析数字
        if(isdigit(*P))
        {
           // 初始化 Token结构体
           // 不使用Head来存储信息，仅用来表示链表入口，这样每次都是存储在Cur->Next中
           // 否则下述操作将使第一个Token的地址不在Head中
           Cur->Next = new_token(TK_NUM, P, P);
           // 指针前进
           Cur = Cur->Next;
           const char *oldP = P;
           Cur->Val = strtoul(P, &P, 10);
           Cur->Len = P - oldP;
           continue;
        }

        // 解析操作符
        if(*P == '+' || *P == '-')
        {
            // 操作符长度为1
            Cur->Next = new_token(TK_PUNCT, P, P + 1);
            Cur = Cur->Next;
            ++P;
            continue;
        }

        // 无法处理的字符
        error("invalid character: '%c'\n", *P);
    }
    // 解析结束，增加一个EOF,表示终止符
    Cur->Next = new_token(TK_EOF, P, P);
    // 返回头结点
    return Head.Next;
}

// 返回TK_NUM的值
static int getNumber(Token *Tok)
{
    if(Tok->kind != TK_NUM)
        error("not a number");
    return Tok->Val;
}

// 判断Tok的值是否等与指定的值
static bool equal(Token *Tok, char *Str)
{
    // 比较字符串LHS(左)，RHS(右)的前N位，S2的长度应大于N
    // 比较按照字典序，LHS<RHS回负值，LHS=RHS返回0，LHS>RHS返回正值
    // 同时确保，此处的Op位数=N
    return memcmp(Tok->Loc, Str, Tok->Len) == 0 && Str[Tok->Len] == '\0';
}

// 跳过指定的str
static Token *skip(Token *Tok, char *Str)
{
    if (!equal(Tok, Str))
        error("expect '%s' ",Str);
    return Tok->Next;
}

int main(int argc,char ** argv)
{
    if(argc != 2)
    {
        // 异常处理，提示参数数量不对
        // fprintf,格式化文件输出，往文件内写入字符串
        // stderr,异常文件, 用于往屏幕显示异常信息
       error("%s: invalid number of arguments",argv[0]);
    }

    // 解析Argv[1]
    Token *Tok = tokenize(argv[1]);

    // 声明全局mian段，同时也是程序的入口
    printf(".globl main\n");
    // main段标签
    printf("main:\n");


    // 这里我们将算式分解为 num op num num ... 的形式
    printf("\tli a0, %d\n", getNumber(Tok));
    Tok = Tok->Next;

    // 解析 （op num）
    while(Tok->kind != TK_EOF)
    {
        if(equal(Tok, "+"))
        {
            Tok = Tok->Next;
            printf("\tadd a0, a0, %d\n", getNumber(Tok));
            Tok = Tok->Next;
            continue;
        }

        // 不是 + 则 判断为 - 
        // 没有subi 但是addi支持有符号数，所以直接对num取反
        Tok = skip(Tok, "-");
        printf("\taddi a0, a0, -%d\n", getNumber(Tok));
        Tok = Tok->Next;
    }  

    // ret为 <jalr x0, x1, 0>别名指令， 用于返回子程序
    printf("\tret\n");
    
    return 0;
}

