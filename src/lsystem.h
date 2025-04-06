#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <stddef.h>

// 定义L-system符号枚举
typedef enum {
    LSYM_A,       // 符号A
    LSYM_B,       // 符号B
    LSYM_F,       // 向前移动(常用于图形绘制)
    LSYM_PLUS,    // 正旋转
    LSYM_MINUS,   // 负旋转
    LSYM_LBRACKET, // 保存状态 '['
    LSYM_RBRACKET, // 恢复状态 ']'
    LSYM_END      // 结束标记
} LSymbol;

// 产生式规则结构
typedef struct {
    LSymbol predecessor;   // 前驱符号
    LSymbol *successor;    // 后继符号序列
    size_t successor_len;  // 后继序列长度
} ProductionRule;

// L-system系统结构
typedef struct {
    LSymbol *axiom;          // 初始符号序列
    size_t axiom_len;        // 初始序列长度
    ProductionRule *rules;   // 产生式规则数组
    size_t rule_count;       // 规则数量
    LSymbol *current;        // 当前符号序列
    size_t current_len;      // 当前序列长度
    unsigned generation;     // 当前代数
} LSystem;

// 函数声明
LSystem* lsystem_create(const LSymbol *axiom, size_t axiom_len, 
                       const ProductionRule *rules, size_t rule_count);
void lsystem_iterate(LSystem *lsys);
const LSymbol* lsystem_get_current(const LSystem *lsys, size_t *out_len);
unsigned lsystem_get_generation(const LSystem *lsys);
void lsystem_free(LSystem *lsys);
const char* lsymbol_to_string(LSymbol sym);

#endif // LSYSTEM_H