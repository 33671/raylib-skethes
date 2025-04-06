#include "lsystem.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// 创建L-system系统
LSystem* lsystem_create(const LSymbol *axiom, size_t axiom_len, 
                       const ProductionRule *rules, size_t rule_count) {
    LSystem *lsys = malloc(sizeof(LSystem));
    if (!lsys) return NULL;
    
    // 复制公理
    lsys->axiom = malloc(axiom_len * sizeof(LSymbol));
    if (!lsys->axiom) {
        free(lsys);
        return NULL;
    }
    memcpy(lsys->axiom, axiom, axiom_len * sizeof(LSymbol));
    lsys->axiom_len = axiom_len;
    
    // 复制当前状态
    lsys->current = malloc(axiom_len * sizeof(LSymbol));
    if (!lsys->current) {
        free(lsys->axiom);
        free(lsys);
        return NULL;
    }
    memcpy(lsys->current, axiom, axiom_len * sizeof(LSymbol));
    lsys->current_len = axiom_len;
    
    // 复制规则
    lsys->rules = malloc(rule_count * sizeof(ProductionRule));
    if (!lsys->rules) {
        free(lsys->axiom);
        free(lsys->current);
        free(lsys);
        return NULL;
    }
    
    for (size_t i = 0; i < rule_count; i++) {
        lsys->rules[i].predecessor = rules[i].predecessor;
        lsys->rules[i].successor_len = rules[i].successor_len;
        
        lsys->rules[i].successor = malloc(rules[i].successor_len * sizeof(LSymbol));
        if (!lsys->rules[i].successor) {
            // 清理已分配的内存
            for (size_t j = 0; j < i; j++) {
                free(lsys->rules[j].successor);
            }
            free(lsys->rules);
            free(lsys->axiom);
            free(lsys->current);
            free(lsys);
            return NULL;
        }
        memcpy(lsys->rules[i].successor, rules[i].successor, 
               rules[i].successor_len * sizeof(LSymbol));
    }
    
    lsys->rule_count = rule_count;
    lsys->generation = 0;
    
    return lsys;
}

// 执行一次迭代
void lsystem_iterate(LSystem *lsys) {
    // 首先计算新字符串需要的空间
    size_t new_len = 0;
    
    for (size_t i = 0; i < lsys->current_len; i++) {
        LSymbol current_sym = lsys->current[i];
        int found_rule = 0;
        
        for (size_t j = 0; j < lsys->rule_count; j++) {
            if (lsys->rules[j].predecessor == current_sym) {
                new_len += lsys->rules[j].successor_len;
                found_rule = 1;
                break;
            }
        }
        
        if (!found_rule) {
            new_len += 1; // 没有规则，保留原符号
        }
    }
    
    // 分配新空间
    LSymbol *new_current = malloc(new_len * sizeof(LSymbol));
    if (!new_current) return;
    
    // 构建新字符串
    size_t pos = 0;
    for (size_t i = 0; i < lsys->current_len; i++) {
        LSymbol current_sym = lsys->current[i];
        int found_rule = 0;
        
        for (size_t j = 0; j < lsys->rule_count; j++) {
            if (lsys->rules[j].predecessor == current_sym) {
                memcpy(new_current + pos, lsys->rules[j].successor,
                       lsys->rules[j].successor_len * sizeof(LSymbol));
                pos += lsys->rules[j].successor_len;
                found_rule = 1;
                break;
            }
        }
        
        if (!found_rule) {
            new_current[pos++] = current_sym;
        }
    }
    
    // 替换当前字符串
    free(lsys->current);
    lsys->current = new_current;
    lsys->current_len = new_len;
    lsys->generation++;
}

// 获取当前符号序列
const LSymbol* lsystem_get_current(const LSystem *lsys, size_t *out_len) {
    if (out_len) {
        *out_len = lsys->current_len;
    }
    return lsys->current;
}

// 获取当前代数
unsigned lsystem_get_generation(const LSystem *lsys) {
    return lsys->generation;
}

// 释放L-system系统
void lsystem_free(LSystem *lsys) {
    free(lsys->axiom);
    free(lsys->current);
    
    for (size_t i = 0; i < lsys->rule_count; i++) {
        free(lsys->rules[i].successor);
    }
    
    free(lsys->rules);
    free(lsys);
}

// 将枚举转换为可读字符串
const char* lsymbol_to_string(LSymbol sym) {
    static const char* strings[] = {
        "A", "B", "F", "+", "-", "[", "]", "END"
    };
    
    if (sym >= LSYM_A && sym <= LSYM_END) {
        return strings[sym];
    }
    return "UNKNOWN";
}