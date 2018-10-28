#ifndef __LCC__
#define __LCC__
#endif

#include <stdio.h>
#include <mysql.h>

int query(const char *query_cont)
{
    MYSQL mysql_conn; /* Connection handle */
    MYSQL_RES *mysql_result; /* Result handle */
    MYSQL_ROW mysql_row; /* Row data */
    int f1, f2, num_row, num_col;

    if (mysql_init(&mysql_conn) != NULL)
    {
        printf("Init succeeds!\n");
        if (mysql_real_connect(
            &mysql_conn, "localhost", "root",
            "456567lw", "company", MYSQL_PORT,
            NULL, 0) != NULL)
        {
            printf("Connection succeeds!\n");
            //if(mysql_query(&mysql_conn,"set character_set_results=gbk") == 0)
            if(mysql_set_character_set(&mysql_conn, "gbk") == 0)
                printf("Set character_set succeeds!\n");
            if(mysql_query(&mysql_conn, query_cont) == 0)
            {
                mysql_result = mysql_store_result(&mysql_conn);
                num_row = mysql_num_rows(mysql_result);
                num_col = mysql_num_fields(mysql_result);

                printf("Query succeeds!\n");
                printf("=========================RESULT==========================\n");
                for(f1 = 0; f1 < num_row; f1++)
                {
                    mysql_row = mysql_fetch_row(mysql_result);

                    for(f2 = 0; f2 < num_col; f2++)
                        printf("%s ", mysql_row[f2]);
                    putchar('\n');
                }

                /* Free the result to release the heap memory*/
                mysql_free_result(mysql_result);
            }
            else
            {
                printf("Query fails: %s\n", mysql_error(&mysql_conn));
                return -3;
            }
        }
        else
        {
            printf("Connection fails: %s\n", mysql_error(&mysql_conn));
            return -2;
        }
    }
    else
    {
        printf("Init fails: %s\n", mysql_error(&mysql_conn));
        return -1;
    }

    mysql_close(&mysql_conn);

    return 0;
}

int main(int argc, char **argv)
{
    char sql_cont[200];
    int i;

    if(strcmp(argv[1], "company_query") == 0)
    {
        switch(atoi(argv[3]))
        {
        case 1:
            strcpy(sql_cont, "select essn\
 from works_on\
 where pno = '");
            strcat(sql_cont, argv[5]);
            strcat(sql_cont, "'");
            printf("@@@ SQL: %s\n", sql_cont);
            query(sql_cont);
            break;
        case 2:
            strcpy(sql_cont, "select ename\
 from employee\
 where essn in\
 (\
 select essn\
 from works_on, project\
 where works_on.pno = project.pno and pname = '");
            for(i = 5; i < argc; ++i)
            {
                strcat(sql_cont, argv[i]);
                if(i != argc - 1)
                    strcat(sql_cont, " ");
            }
            strcat(sql_cont, "')");
            printf("@@@ SQL: %s\n", sql_cont);
            query(sql_cont);
            break;
        case 3:
            strcpy(sql_cont, "select ename, address\
 from employee, department\
 where employee.dno = department.dno and dname = '");
            for(i = 5; i < argc; ++i)
            {
                strcat(sql_cont, argv[i]);
                if(i != argc - 1)
                    strcat(sql_cont, " ");
            }
            strcat(sql_cont, "'");
            printf("@@@ SQL: %s\n", sql_cont);
            query(sql_cont);
            break;
        case 4:
            strcpy(sql_cont, "select ename, address\
 from employee, department\
 where employee.dno = department.dno and dname = '");
            for(i = 5; i < argc-1; ++i)
            {
                strcat(sql_cont, argv[i]);
                if(i != argc - 2)
                    strcat(sql_cont, " ");
            }
            strcat(sql_cont, "' and salary < ");
            strcat(sql_cont, argv[argc-1]);
            printf("@@@ SQL: %s\n", sql_cont);
            query(sql_cont);
            break;
        case 5:
            strcpy(sql_cont, "select distinct ename\
 from employee\
 where essn not in\
 (\
 select employee.essn\
 from employee, works_on\
 where employee.essn = works_on.essn and pno = '");
            strcat(sql_cont, argv[5]);
            strcat(sql_cont, "')");
            printf("@@@ SQL: %s\n", sql_cont);
            query(sql_cont);
            break;
        case 6:
            strcpy(sql_cont, "select ename, dname\
 from employee, department\
 where employee.dno = department.dno and essn in\
 (\
 select essn\
 from employee\
 where superssn in\
 (\
 select essn\
 from employee\
 where ename = '");
            for(i = 5; i < argc; ++i)
            {
                strcat(sql_cont, argv[i]);
                if(i != argc - 1)
                    strcat(sql_cont, " ");
            }
            strcat(sql_cont, "'))");
            printf("@@@ SQL: %s\n", sql_cont);
            query(sql_cont);
            break;
        case 7:
            strcpy(sql_cont, "select essn\
 from works_on\
 where pno = '");
            strcat(sql_cont, argv[5]);
            strcat(sql_cont, "' and essn in\
 (\
 select essn\
 from works_on\
 where pno = '");
            strcat(sql_cont, argv[6]);
            strcat(sql_cont, "')");
            printf("@@@ SQL: %s\n", sql_cont);
            query(sql_cont);
            break;
        case 8:
            strcpy(sql_cont, "select dname\
 from department\
 where dno in\
 (\
 select dno\
 from employee\
 group by dno\
 having avg(salary) < ");
            strcat(sql_cont, argv[5]);
            strcat(sql_cont, ")");
            printf("@@@ SQL: %s\n", sql_cont);
            query(sql_cont);
            break;
        case 9:
            strcpy(sql_cont, "select ename\
 from employee\
 where essn in\
 (\
 select essn\
 from works_on\
 group by essn\
 having count(pno) >= ");
            strcat(sql_cont, argv[5]);
            strcat(sql_cont, " and sum(hours) <= ");
            strcat(sql_cont, argv[6]);
            strcat(sql_cont, ")");
            printf("@@@ SQL: %s\n", sql_cont);
            query(sql_cont);
            break;
        default:
            printf("***2 Input is not available!\n");
        }
    }
    else
        printf("***1 Input is not available!\n");

    return 0;
}
