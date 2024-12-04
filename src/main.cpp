#include "../include/Compiler.h"

#include <iostream>

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 3)
        {
            std::cerr << "用法: " << argv[0] << " <输入文件> <输出目录>\n";
            return 1;
        }

        auto result = pl0::Compiler::compileFile(argv[1]);
        pl0::Compiler::outputResults(result, argv[2]);

        if (!result.success)
        {
            std::cerr << "编译失败！\n";
            for (const auto &error : result.errors)
            {
                std::cerr << error << '\n';
            }
            return 1;
        }

        std::cout << "编译成功！\n";
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "错误：" << e.what() << '\n';
        return 1;
    }
}