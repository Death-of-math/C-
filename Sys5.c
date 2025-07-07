#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "student.h"

char currentFileName[100];  // 全局变量，用于保存当前使用的文件名


// 姓名索引结构
typedef struct {
    char name[15];      // 姓名
    long position;      // 在数据文件中的位置
} NameIndex;

// 函数声明
void Input(STU *p, int n);
int Max(STU *p, int scoreIndex);
void Sort_insert(STU *p, int n, char *major);
 int Input2(STU *p, const char *filename);
void Output(STU *p);
STU Fetch(int studentIndex);
void MaxAll(STU *p, int courseIndex);
void Sort_select(STU *p);
void Search(STU *p, int classNo, char s, int scoreSum);
void Sort_buble(STU *p, int n);
void ModifyTopStudent(STU *p, int classNo);
void CreateNameIndex(const char *filename);
void Sort_insert2(STU *p, int n, char *major, int courseIndex);
void AddStudent(STU *p, int *n);
void DeleteStudent(STU *p, int *n, char *delName);
void Save(STU *p, int n);
void ModifyStudentInfo(STU *p, int n, const char *name);


int main() {
    STU student[N];
    int choice, index, courseIndex;
    int n;

    printf("\n学生信息管理系统\n");
    printf("请输入要打开的学生数据文件名（如 studentInit.dat）：");
    scanf("%s", currentFileName);

    n = Input2(student, currentFileName);
    // 新增：创建姓名索引文件
    CreateNameIndex(currentFileName);

    while(1) {
        printf("\n学生信息管理系统\n");
        printf("1. 显示所有学生信息\n");
        printf("2. 查找单科最高分学生\n");
        printf("3. 所有学生按平均成绩排序\n");
        printf("4. 按姓名查找学生信息\n");
        printf("5. 查询某班级，总分大于某分数线的学生\n");
        printf("6. 某班级学生按平均成绩排序\n");
        printf("7. 查询并修改某班级最高分学生成绩\n");
        printf("8. 按专业+课程成绩排序\n"); 
        printf("9. 增加学生信息\n");
        printf("10. 删除学生信息\n");
        printf("11. 修改学生信息\n");
        printf("12. 将当前数据保存为新文件\n");
        

        printf("0. 退出系统\n");


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

            case 2: {
                int courseInput;
                printf("请输入课程编号（1-数学A，2-物理B，3-化学C）: ");
                scanf("%d", &courseInput);
                int courseIndex = courseInput - 1; // 用户输入1~3，内部转成0~2
                MaxAll(student, courseIndex);
                break;
            }


            case 3:  // 按平均成绩排序
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

            // 修改后的功能5：使用索引文件快速查找（先用索引文件查到序号，再用Fetch获取学生信息）
            case 4: {
                char key[30];  
                printf("请输入要查找的姓名: ");
                scanf("%s", key);

                // 加载姓名索引到内存
                NameIndex indexArr[N];
                FILE *index_fp = fopen("name_index.idx", "rb");
                if (!index_fp) {
                    printf("索引文件不存在！\n");
                    break;
                }
                fread(indexArr, sizeof(NameIndex), N, index_fp);
                fclose(index_fp);

                // 在索引中查找姓名，得到在数据文件中的序号
                int found = 0;
                int stuIndex = -1;
                for (int i = 0; i < N; i++) {
                    if (strcmp(indexArr[i].name, key) == 0) {
                        // 计算学生在数据文件中的序号
                        stuIndex = (int)(indexArr[i].position / sizeof(STU));
                        found = 1;
                        break;
                    }
                }

                if (found && stuIndex >= 0 && stuIndex < N) {
                    STU s = Fetch(stuIndex);
                    printf("\n找到匹配学生:\n");
                    printf("\n%-16s %-14s %-12s %-7s %-9s %-9s %-8s\n", 
                          "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C");
                    Output(&s);  
                } else {
                    printf("查无此人!\n");
                }
                break;
            }
            
            

            case 5: {
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

            case 6: {
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

            case 7: {
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
            
            case 8: {
                char major[10];
                int courseIndex;
                printf("请输入专业(computer/software/network): ");
                scanf("%s", major);
                printf("请输入课程编号(0-数学A, 1-物理B, 2-化学C): ");
                scanf("%d", &courseIndex);
                
                if(courseIndex < 0 || courseIndex > 2) {
                    printf("无效课程编号!\n");
                    break;
                }
                
                // 调用修改后的Sort_insert函数
                Sort_insert2(student, N, major, courseIndex);
                break;
            }

            case 9:{
            AddStudent(student, &n);
            break;
            }

            case 10: {
            char delName[15];
            printf("请输入要删除的学生姓名: ");
            scanf("%s", delName);
            DeleteStudent(student, &n, delName);
            break;
            }

            case 11: {
            char targetName[15];
            printf("请输入要修改信息的学生姓名: ");
            scanf("%s", targetName);
            ModifyStudentInfo(student, n, targetName);
            break;
            }

            case 12:{
            Save(student, n);
            break;
            }
            
            case 0:  // 退出
                printf("系统已退出!\n");
                return 0;

            default:
                printf("无效选择!\n");
                
        }
    }
    return 0;
}

 int Input2(STU *p, const char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printf("无法打开文件: %s\n", filename);
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    rewind(fp);

    int n = filesize / sizeof(STU);
    if (n > N) n = N;  // 防止超限

    fread(p, sizeof(STU), n, fp);
    fclose(fp);
    return n;
}



// 输出单个学生信息
void Output(STU *p) {
    printf("%-15s%-15s%-12s%-6d%-8d%-8d%-8d\n", 
          p->num, p->name, p->major, p->classNo, 
          p->score[0], p->score[1], p->score[2]);
}

extern char currentFileName[100];  // 引用主函数里的全局文件名

STU Fetch(int studentIndex) {
    STU s = {0};
    if(studentIndex < 0 || studentIndex >= N) {
        printf("学生索引超出范围！\n");
        return s;
    }

    FILE *fp = fopen(currentFileName, "rb");
    if(fp == NULL) {
        printf("无法打开文件: %s\n", currentFileName);
        return s;
    }

    fseek(fp, studentIndex * sizeof(STU), SEEK_SET);
    fread(&s, sizeof(STU), 1, fp);
    fclose(fp);
    return s;
}




// 查找单科最高分学生
void MaxAll(STU *p, int scoreIndex) {
    int maxScore = p[0].score[scoreIndex];

    // 1. 找出最大分数
    for (int i = 1; i < N; i++) {
        if (p[i].score[scoreIndex] > maxScore) {
            maxScore = p[i].score[scoreIndex];
        }
    }

    // 2. 输出所有等于最大分数的学生
    printf("\n课程最高分为 %d，获得该分数的学生有：\n", maxScore);
    printf("%-16s %-14s %-12s %-7s %-9s %-9s %-8s\n", 
          "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C");

    for (int i = 0; i < N; i++) {
        if (p[i].score[scoreIndex] == maxScore) {
            Output(&p[i]);
        }
    }
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

void CreateNameIndex(const char *filename) {
    FILE *data_fp = fopen(filename, "rb");
    FILE *index_fp = fopen("name_index.idx", "wb");

    if (!data_fp || !index_fp) {
        printf("无法创建索引文件!\n");
        exit(1);
    }

    STU s;
    NameIndex idx;
    long pos = 0;

    while (fread(&s, sizeof(STU), 1, data_fp) == 1) {
        strcpy(idx.name, s.name);
        idx.position = pos;
        fwrite(&idx, sizeof(NameIndex), 1, index_fp);
        pos = ftell(data_fp);
    }

    fclose(data_fp);
    fclose(index_fp);
}


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

        // 保存排序结果到新文件
    char filename[50];
    sprintf(filename, "class%d_sorted.dat", classNo);
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("无法保存文件 %s！\n", filename);
        return;
    }

    fwrite(stu_class_ave, sizeof(STU), count, fp);
    fclose(fp);
    printf("已将排序结果保存至文件：%s\n", filename);

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

void Sort_insert2(STU *p, int n, char *major, int courseIndex) {
    // 局部变量
    STU stu_class_subject[N]; // 按某门课程成绩排序后的某个专业的学生信息
    int count = 0;            // 实际元素个数
    
    // 1. 筛选指定专业的学生
    for (int i = 0; i < n; i++) {
        if (strcmp(p[i].major, major) == 0) {
            stu_class_subject[count++] = p[i];
        }
    }
    
    if (count == 0) {
        printf("找不到专业为 %s 的学生!\n", major);
        return;
    }
    
    // 2. 直接插入排序（按指定课程成绩升序）
    for (int i = 1; i < count; i++) {
        STU key = stu_class_subject[i];
        int j = i - 1;
        
        // 将比当前元素大的元素向右移动
        while (j >= 0 && stu_class_subject[j].score[courseIndex] > key.score[courseIndex]) {
            stu_class_subject[j + 1] = stu_class_subject[j];
            j--;
        }
        stu_class_subject[j + 1] = key;
    }
    
    // 3. 输出排序结果
    const char *courses[] = {"数学A", "物理B", "化学C"};
    printf("\n专业 '%s' 按 %s 成绩升序排序结果:\n", major, courses[courseIndex]);
    printf("%-16s %-14s %-12s %-7s %-9s %-9s %-8s\n", 
          "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C");
    
    for (int i = 0; i < count; i++) {
        Output(&stu_class_subject[i]);
    }
}

void AddStudent(STU *p, int *n) {
    if (*n >= N) {
        printf("学生数量已满，无法添加！\n");
        return;
    }

    printf("请输入新学生信息（学号 姓名 专业 班级 数学A 物理B 化学C）:\n");
    scanf("%s %s %s %d %d %d %d", 
        p[*n].num, p[*n].name, p[*n].major, &p[*n].classNo,
        &p[*n].score[0], &p[*n].score[1], &p[*n].score[2]);

    (*n)++;
    printf("添加成功！当前总人数: %d\n", *n);
}

void DeleteStudent(STU *p, int *n, char *delName) {
    int found = 0;
    for (int i = 0; i < *n; i++) {
        if (strcmp(p[i].name, delName) == 0) {
            for (int j = i; j < *n - 1; j++) {
                p[j] = p[j + 1];
            }
            (*n)--;
            found = 1;
            printf("学生 %s 已删除！当前总人数: %d\n", delName, *n);
            break;
        }
    }
    if (!found) {
        printf("未找到该学生！\n");
    }
}

void Save(STU *p, int n) {
    char filename[50];
    printf("请输入要保存的新文件名（如：abc.dat）：");
    scanf("%s", filename);

    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        printf("文件创建失败！\n");
        return;
    }

    fwrite(p, sizeof(STU), n, fp);    // 只写学生数据
    fclose(fp);
    printf("数据已保存到 %s 文件中。\n", filename);
}


void ModifyStudentInfo(STU *p, int n, const char *name) {
    int found = 0;
    for (int i = 0; i < n; i++) {
        if (strcmp(p[i].name, name) == 0) {
            printf("\n当前信息为：\n");
            Output(&p[i]);

            printf("\n请输入新的信息（学号 姓名 专业 班级 数学A 物理B 化学C）：\n");
            scanf("%s %s %s %d %d %d %d", 
                p[i].num, p[i].name, p[i].major, &p[i].classNo, 
                &p[i].score[0], &p[i].score[1], &p[i].score[2]);

            printf("信息已更新为：\n");
            Output(&p[i]);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("未找到姓名为 %s 的学生！\n", name);
    }
}

//函数名适配器

// Input：调用原始 Input2 函数，传入全局文件名
void Input(STU *p, int n) {
    extern char currentFileName[100];
    if (Input2(p, currentFileName) != 0) {
        printf("读取文件失败。\n");
    }
}

// Max：包装 MaxAll
int Max(STU *p, int scoreIndex) {
    MaxAll(p, scoreIndex);  // 原函数负责打印
    return 0;  // 默认返回 0 
}

// Sort_insert：将课程索引默认设置为 0，符合要求的接口格式
void Sort_insert(STU *p, int n, char *major) {
    int courseIndex = 0;  // 默认按第0门课程
    Sort_insert2(p, n, major, courseIndex);
}
