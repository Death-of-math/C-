#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 10

typedef struct Student {
    char num[15];       // 学号
    char name[15];      // 姓名
    char major[10];     // 专业(computer,software,network)
    int classNo;        // 班级(1-2)
    int score[3];       // 3门课的成绩(0-2)
} STU;

// 函数声明
void Input(STU *p, int n);
void Output(STU *p);
STU Fetch(int studentIndex);
int Max(STU *p, int scoreIndex);
void Sort_select(STU *p);
// 新增函数声明
void Search(STU *p, int classNo, char s, int scoreSum);
void Sort_buble(STU *p, int n);
void ModifyTopStudent(STU *p, int classNo);

int main() {
    STU student[N];
    int choice, index, courseIndex;
    
    // 先初始化数据
    Input(student, N);
    
    while(1) {
        printf("\n学生信息管理系统菜单\n");
        printf("1. 显示所有学生信息\n");
        printf("2. 查看单个学生信息\n");
        printf("3. 查找单科最高分学生\n");
        printf("4. 按平均成绩排序\n");
        printf("5. 按姓名/学号查找学生\n");
        printf("6. 退出系统\n");
        // 新增选项
        printf("7. 班级成绩综合查找\n");
        printf("8. 班级学生冒泡排序\n");
        printf("9. 修改班级最高分学生\n");
        printf("请选择操作: ");
        scanf("%d", &choice);
        
        switch(choice) {
            case 1:  // 显示所有学生
                printf("\n%-16s %-14s %-12s %-7s %-9s %-9s %-8s\n", 
                      "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C");
                for(int i = 0; i < N; i++) {
                    Output(&student[i]);
                }
                break;
                
            case 2:  // 查看单个学生
                printf("请输入学生序号(0-%d): ", N-1);
                scanf("%d", &index);
                if(index < 0 || index >= N) {
                    printf("无效序号!\n");
                    break;
                }
                STU s = Fetch(index);
                printf("\n%-16s %-14s %-12s %-7s %-9s %-9s %-8s\n", 
                      "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C");
                Output(&s);
                break;
                
            case 3:  // 查找单科最高分
                printf("请输入课程编号(0-2): ");
                scanf("%d", &courseIndex);
                if(courseIndex < 0 || courseIndex > 2) {
                    printf("无效课程编号!\n");
                    break;
                }
                int maxIndex = Max(student, courseIndex);
                printf("\n%-16s %-14s %-12s %-7s %-9s %-9s %-8s\n", 
                      "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C");
                Output(&student[maxIndex]);
                printf("课程%d最高分: %d\n", courseIndex, student[maxIndex].score[courseIndex]);
                break;
                
            case 4:  // 按平均成绩排序
                Sort_select(student);
                printf("已按平均成绩排序!\n");
                
                printf("\n%-14s %-8s %-12s %-7s %-8s %-8s %-9s %-8s\n", 
                      "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C", "平均分");
                
                for(int i = 0; i < N; i++) {
                    float avg = (student[i].score[0] + student[i].score[1] + student[i].score[2]) / 3.0;
                    printf("%-12s %-8s %-10s %-6d %-6d %-6d %-6d %-6.1f\n", 
                          student[i].num,      
                          student[i].name,     
                          student[i].major,    
                          student[i].classNo,  
                          student[i].score[0], 
                          student[i].score[1], 
                          student[i].score[2], 
                          avg);                
                }
                break;
            
            case 5: {
                char key[30];  
                printf("请输入要查找的姓名或学号: ");
                scanf("%s", key);
                
                int found = 0; 
                for (int i = 0; i < N; i++) {
                    STU s = Fetch(i);
                    if (strcmp(s.num, key) == 0 || strcmp(s.name, key) == 0) {
                        printf("\n找到匹配学生:\n");
                        printf("\n%-16s %-14s %-12s %-7s %-9s %-9s %-8s\n", 
                              "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C");
                        Output(&s);  
                        found = 1;
                        break;  
                    }
                }
                
                if (!found) {
                    printf("查无此人!\n");
                }
                break;
            }

            case 6:  // 退出
                printf("系统已退出!\n");
                return 0;
                
            // 新增选项7：成绩综合查找
            case 7: {
                int classNo;
                char s;
                int scoreSum;
                printf("请输入班级(1-2): ");
                scanf("%d", &classNo);
                printf("请输入比较符(>): ");
                scanf(" %c", &s);  // 注意空格用于吸收回车
                printf("请输入总分阈值: ");
                scanf("%d", &scoreSum);
                
                if (classNo < 1 || classNo > 2) {
                    printf("无效班级!\n");
                    break;
                }
                Search(student, classNo, s, scoreSum);
                break;
            }
                
            // 新增选项8：班级冒泡排序
            case 8: {
                int classNo;
                printf("请输入要排序的班级(1-2): ");
                scanf("%d", &classNo);
                
                if (classNo < 1 || classNo > 2) {
                    printf("无效班级!\n");
                    break;
                }
                Sort_buble(student, classNo);
                break;
            }
                
            // 新增选项9：修改班级最高分学生
            case 9: {
                int classNo;
                printf("请输入班级(1-2): ");
                scanf("%d", &classNo);
                
                if (classNo < 1 || classNo > 2) {
                    printf("无效班级!\n");
                    break;
                }
                ModifyTopStudent(student, classNo);
                break;
            }
                
            default:
                printf("无效选择!\n");
        }
    }
    return 0;
}

// 从文件读取学生信息
void Input(STU *p, int n) {
    FILE *fp = fopen("studentInit.dat", "rb");
    if(fp == NULL) {
        printf("无法打开文件!\n");
        exit(1);
    }
    fread(p, sizeof(STU), n, fp);
    fclose(fp);
}

// 输出单个学生信息
void Output(STU *p) {
    printf("%-15s%-15s%-12s%-6d%-8d%-8d%-8d\n", 
          p->num, p->name, p->major, p->classNo, 
          p->score[0], p->score[1], p->score[2]);
}

// 从文件随机读取学生信息
STU Fetch(int studentIndex) {
    FILE *fp = fopen("studentInit.dat", "rb");
    if(fp == NULL) {
        printf("无法打开文件!\n");
        exit(1);
    }
    fseek(fp, studentIndex * sizeof(STU), SEEK_SET);
    STU s;
    fread(&s, sizeof(STU), 1, fp);
    fclose(fp);
    return s;
}

// 查找单科最高分学生
int Max(STU *p, int scoreIndex) {
    int maxIndex = 0;
    for(int i = 1; i < N; i++) {
        if(p[i].score[scoreIndex] > p[maxIndex].score[scoreIndex]) {
            maxIndex = i;
        }
    }
    return maxIndex;
}

// 按平均成绩排序
void Sort_select(STU *p) {
    for(int i = 0; i < N-1; i++) {
        int minIndex = i;
        for(int j = i+1; j < N; j++) {
            float avgI = (p[minIndex].score[0] + p[minIndex].score[1] + p[minIndex].score[2]) / 3.0;
            float avgJ = (p[j].score[0] + p[j].score[1] + p[j].score[2]) / 3.0;
            if(avgJ < avgI) {
                minIndex = j;
            }
        }
        if(minIndex != i) {
            STU temp = p[i];
            p[i] = p[minIndex];
            p[minIndex] = temp;
        }
    }
}

// ====================== 新增函数实现 ======================

// 功能1: 班级成绩综合查找
void Search(STU *p, int classNo, char s, int scoreSum) {
    printf("\n满足条件的学生(班级%d且总分%c%d):\n", classNo, s, scoreSum);
    printf("%-14s %-8s %-12s %-7s %-8s %-8s %-9s %-8s\n", 
          "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C", "总分");
    
    int found = 0;
    for (int i = 0; i < N; i++) {
        if (p[i].classNo != classNo) continue;
        
        int total = p[i].score[0] + p[i].score[1] + p[i].score[2];
        
        if (s == '>' && total > scoreSum) {
            printf("%-12s %-8s %-10s %-6d %-6d %-6d %-6d %-6d\n", 
                  p[i].num, p[i].name, p[i].major, p[i].classNo,
                  p[i].score[0], p[i].score[1], p[i].score[2], total);
            found = 1;
        }
        // 可根据需要扩展其他比较符
    }
    
    if (!found) {
        printf("未找到满足条件的学生!\n");
    }
}

// 功能2: 班级冒泡排序
void Sort_buble(STU *p, int classNo) {
    STU stu_class_ave[N]; // 存储该班级学生
    int count = 0;        // 实际元素个数

    // 筛选指定班级的学生
    for (int i = 0; i < N; i++) {
        if (p[i].classNo == classNo) {
            stu_class_ave[count++] = p[i];
        }
    }

    // 冒泡排序（按平均成绩降序）
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            float avg1 = (stu_class_ave[j].score[0] + 
                         stu_class_ave[j].score[1] + 
                         stu_class_ave[j].score[2]) / 3.0;
            
            float avg2 = (stu_class_ave[j+1].score[0] + 
                         stu_class_ave[j+1].score[1] + 
                         stu_class_ave[j+1].score[2]) / 3.0;
            
            if (avg1 < avg2) { // 降序排序
                STU temp = stu_class_ave[j];
                stu_class_ave[j] = stu_class_ave[j+1];
                stu_class_ave[j+1] = temp;
            }
        }
    }

    // 输出排序结果
    printf("\n班级%d按平均成绩降序排序结果:\n", classNo);
    printf("%-14s %-8s %-12s %-7s %-8s %-8s %-9s %-8s\n", 
          "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C", "平均分");
    
    for (int i = 0; i < count; i++) {
        float avg = (stu_class_ave[i].score[0] + 
                   stu_class_ave[i].score[1] + 
                   stu_class_ave[i].score[2]) / 3.0;
        
        printf("%-12s %-8s %-10s %-6d %-6d %-6d %-6d %-6.1f\n", 
              stu_class_ave[i].num, 
              stu_class_ave[i].name, 
              stu_class_ave[i].major, 
              stu_class_ave[i].classNo,
              stu_class_ave[i].score[0],
              stu_class_ave[i].score[1],
              stu_class_ave[i].score[2],
              avg);
    }
}

// 功能3: 修改班级最高分学生
void ModifyTopStudent(STU *p, int classNo) {
    int maxIndex = -1;
    int maxTotal = -1;
    
    // 查找总分最高的学生
    for (int i = 0; i < N; i++) {
        if (p[i].classNo != classNo) continue;
        
        int total = p[i].score[0] + p[i].score[1] + p[i].score[2];
        if (total > maxTotal) {
            maxTotal = total;
            maxIndex = i;
        }
    }
    
    if (maxIndex == -1) {
        printf("该班级无学生!\n");
        return;
    }
    
    // 显示最高分学生信息
    printf("\n班级%d最高分学生(总分%d):\n", classNo, maxTotal);
    printf("%-16s %-14s %-12s %-7s %-9s %-9s %-8s\n", 
          "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C");
    Output(&p[maxIndex]);
    
    // 修改学生成绩
    printf("\n请输入新的成绩(格式: 数学 物理 化学): ");
    scanf("%d %d %d", &p[maxIndex].score[0], &p[maxIndex].score[1], &p[maxIndex].score[2]);
    
    // 另存为新文件
    FILE *fp = fopen("modify.dat", "wb");
    if (fp == NULL) {
        printf("无法创建文件!\n");
        return;
    }
    fwrite(&p[maxIndex], sizeof(STU), 1, fp);
    fclose(fp);
    
    printf("修改已保存到modify.dat文件!\n");
}