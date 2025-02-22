#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "autocomplete.h"

// compare terms and order lexicographically
int compare_terms(const void *a, const void *b) {
    const term *term_a = (const term *)a;
    const term *term_b = (const term *)b;
    return strcmp(term_a->term, term_b->term);
}

// 比较函数用于按权重升序排序
int compare_weight(const void *a, const void *b) {
    const term *ta = (const term *)a;
    const term *tb = (const term *)b;
    if (ta->weight < tb->weight) return 1;
    if (ta->weight > tb->weight) return -1;
    return 0;
}

// part 1
void read_in_terms(term **terms, int *pnterms, char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening file");
        exit(1);
    }

    // 读取术语数量
    int num_terms;
    if (fscanf(fp, "%d\n", &num_terms) != 1) {
        fprintf(stderr, "Error reading number of terms\n");
        fclose(fp);
        exit(1);
    }

    // 分配内存
    *terms = (term *)malloc(num_terms * sizeof(term));
    if (*terms == NULL) {
        perror("Memory allocation failed");
        fclose(fp);
        exit(1);
    }

    char buffer[1024]; // 足够大的缓冲区处理每行数据
    for (int i = 0; i < num_terms; i++) {
        if (fgets(buffer, sizeof(buffer), fp) == NULL) {
            fprintf(stderr, "Error reading line %d\n", i + 1);
            free(*terms);
            fclose(fp);
            exit(1);
        }

        // 处理行数据
        char *start = buffer;
        // 跳过前导空白
        while (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r') {
            start++;
        }

        // 解析权重
        char *end;
        double weight = strtod(start, &end);
        if (end == start) {
            fprintf(stderr, "Error parsing weight in line %d\n", i + 1);
            free(*terms);
            fclose(fp);
            exit(1);
        }

        // 跳过权重后的空白
        while (*end == ' ' || *end == '\t') {
            end++;
        }

        // 处理术语（去除换行符）
        char *term_str = end;
        size_t term_len = strlen(term_str);
        while (term_len > 0 && (term_str[term_len - 1] == '\n' || term_str[term_len - 1] == '\r')) {
            term_str[--term_len] = '\0';
        }

        // 存储到结构体
        strncpy((*terms)[i].term, term_str, sizeof((*terms)[i].term) - 1);
        (*terms)[i].term[sizeof((*terms)[i].term) - 1] = '\0'; // 确保终止
        (*terms)[i].weight = weight;
    }

    fclose(fp);

    // 按术语字典序排序
    qsort(*terms, num_terms, sizeof(term), compare_terms);
    *pnterms = num_terms;
}

//part 2
int lowest_match(term *terms, int nterms, char *substr) {
    int low = 0;
    int high = nterms - 1;
    int ans = -1;
    size_t len = strlen(substr);

    // 处理空字符串的特殊情况
    if (len == 0) {
        return -1;
    }

    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = strncmp(terms[mid].term, substr, len);

        if (cmp == 0) {
            ans = mid;
            high = mid - 1; // 继续向左寻找更小索引
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return ans;
}

// part 3
int highest_match(term *terms, int nterms, char *substr) {
    int low = 0;
    int high = nterms - 1;
    int ans = -1;
    size_t len = strlen(substr);

    // 处理空字符串的特殊情况
    if (len == 0) {
        return high;
    }

    while (low <= high) {
        int mid = low + (high - low) / 2;
        int cmp = strncmp(terms[mid].term, substr, len);

        if (cmp == 0) {
            ans = mid;
            low = mid + 1; // 继续向右寻找更大索引
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return ans;
}

// part 4
void autocomplete(term **answer, int *n_answer, term *terms, int nterms, char *substr) {
    int low = lowest_match(terms, nterms, substr);
    int high = highest_match(terms, nterms, substr);

    // 处理无匹配情况
    if (low == -1 || high == -1 || low > high) {
        *n_answer = 0;
        *answer = NULL;
        return;
    }

    // 计算匹配数量并分配内存
    int count = high - low + 1;
    *answer = (term *)malloc(count * sizeof(term));
    if (*answer == NULL) {
        perror("Memory allocation failed");
        exit(1);
    }

    // 复制匹配项
    memcpy(*answer, &terms[low], count * sizeof(term));

    // 按权重排序
    qsort(*answer, count, sizeof(term), compare_weight);
    *n_answer = count;
}


// int main(void)
// {
//     struct term *terms;
//     int nterms;
//     read_in_terms(&terms, &nterms, "cities.txt");
//     lowest_match(terms, nterms, "Tor");
//     highest_match(terms, nterms, "Tor");
    
//     struct term *answer;
//     int n_answer;
//     autocomplete(&answer, &n_answer, terms, nterms, "Tor");
//     //free allocated blocks here -- not required for the project, but good practice
//     return 0;
// }
