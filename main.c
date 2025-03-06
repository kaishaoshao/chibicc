#include <stdio.h>
#include <stdlib.h>

int main(int argc,char ** argv)
{
    if(argc != 2)
    {
        // 异常处理，提示参数数量不对
        // fprintf,格式化文件输出，往文件内写入字符串
        // stderr,异常文件, 用于往屏幕显示异常信息
        fprintf(stderr,"%s: invalid number of arguments\n", argv[0]);
        return 1;    
    }
    
    // 声明全局mian段，同时也是程序的入口
    printf(".globl main\n");
    // main段标签
    printf("main:\n");
    // li为addi别名指令，加载一个立即数到寄存器中
    // 传入程序参数为str类型,因为需要转换为int类型使用atoi
    printf("    li a0, %d\n",atoi(argv[1]));
    // ret为jalr x0, x1, 0别名指令， 用于返回子程序
    printf("    ret\n");
    
    return 0;
}