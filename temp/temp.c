void test_001()
{
    // 传入程序参数为str类型,因为需要转换为int类型使用atoi
    // printf("\tli a0, %d\n",atoi(argv[1]));
}

int test_002(int argc, char *argv[])
{
    if(argc != 2)
    {
        // 异常处理，提示参数数量不对
        // fprintf,格式化文件输出，往文件内写入字符串
        // stderr,异常文件, 用于往屏幕显示异常信息
        fprintf(stderr,"%s: invalid number of arguments\n", argv[0]);
        return 1;    
    }
    // P保存输入的算式的str
    // 不声明const是为了后续的递增操作
    char *P = argv[1];

    // 声明全局mian段，同时也是程序的入口
    printf(".globl main\n");
    // main段标签
    printf("main:\n");

    // li为addi别名指令，加载一个立即数到寄存器中
    // 传入程序的参数位str类型，因为需要转换为long类型
    // strtol为“string to long”
    // 参数为：被转换的str，str除去数字后的剩余部分，进制
    // 传入&P，即char**,是为了修改p的值
    // 这里我们将算式分解为 num op num num ... 的形式
    printf("\tli a0, %ld\n", strtol(P, &P, 10));
    // 解析：$a0为寄存器，strtol将p转换为long类型，并赋值给$a0
    while(*P)
    {
        // 解析op
        if(*P == '+')
        {
            ++P;
            // 解析数字
            // addi rd, rs1, imm 表示rd = rs1 + imm
            printf("\taddi a0, a0 , %ld\n", strtol(P, &P, 10));
            continue;
        }
        if(*P == '-')
        {
            ++P;
            // 解析数字
            // addi中的imm为有符号数 表示rd = rs1 + （-imm）
            printf("\taddi a0, a0 , -%ld\n", strtol(P, &P, 10));
            continue;
        }
        // 如果存在未解析的字符，则报错
        fprintf(stderr, "unexpected character: '%c'\n", *P);
        return 1;
    }  

    // ret为 <jalr x0, x1, 0>别名指令， 用于返回子程序
    printf("\tret\n");
    
    return 0;
}

