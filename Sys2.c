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

// 新增：姓名索引结构
typedef struct {
    char name[15];      // 姓名
    long position;      // 在数据文件中的位置
} NameIndex;

// 函数声明
void Input(STU *p, int n);
void Output(STU *p);
STU Fetch(int studentIndex);
int Max(STU *p, int scoreIndex);
void Sort_select(STU *p);
void Search(STU *p, int classNo, char s, int scoreSum);
void Sort_buble(STU *p, int n);
void ModifyTopStudent(STU *p, int classNo);
void CreateNameIndex();
void Sort_insert(STU *p, int n, char *major, int courseIndex);

int main() {
    STU student[N];
    int choice, index, courseIndex;

    // 先初始化数据
    Input(student, N);
    // 新增：创建姓名索引文件
    CreateNameIndex();

    while(1) {
        printf("\n学生信息管理系统菜单\n");
        printf("1. 显示所有学生信息\n");
        printf("2. 查看单个学生信息\n");
        printf("3. 查找单科最高分学生\n");
        printf("4. 按平均成绩排序\n");
        printf("5. 按姓名查找学生\n");
        printf("6. 退出系统\n");
        printf("7. 班级成绩综合查找\n");
        printf("8. 班级学生冒泡排序\n");
        printf("9. 修改班级最高分学生\n");
        printf("10. 专业学生课程成绩直接插入排序\n"); // 新增选项
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
                {
                    STU s = Fetch(index);
                    printf("\n%-16s %-14s %-12s %-7s %-9s %-9s %-8s\n", 
                          "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C");
                    Output(&s);
                }
                break;

            case 3:  // 查找单科最高分
                printf("请输入课程编号(0-2): ");
                scanf("%d", &courseIndex);
                if(courseIndex < 0 || courseIndex > 2) {
                    printf("无效课程编号!\n");
                    break;
                }
                {
                    int maxIndex = Max(student, courseIndex);
                    printf("\n%-16s %-14s %-12s %-7s %-9s %-9s %-8s\n", 
                          "学号", "姓名", "专业", "班级", "数学A", "物理B", "化学C");
                    Output(&student[maxIndex]);
                    printf("课程%d最高分: %d\n", courseIndex, student[maxIndex].score[courseIndex]);
                }
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

            // 修改后的功能5：使用索引文件快速查找（先用索引文件查到序号，再用Fetch获取学生信息）
            case 5: {
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
            
            case 6:  // 退出
                printf("系统已退出!\n");
                return 0;

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
            
            case 10: {
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
                Sort_insert(student, N, major, courseIndex);
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

// 新增：创建姓名索引文件
void CreateNameIndex() {
    FILE *data_fp = fopen("studentInit.dat", "rb");
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

void Sort_insert(STU *p, int n, char *major, int courseIndex) {
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