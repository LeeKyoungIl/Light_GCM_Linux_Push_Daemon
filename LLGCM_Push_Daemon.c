#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

#define PUSH_DEBUG "true"

#include "http_connect.c"
#include "mysql_connect.c"
#include "push_util.c"

int main(void) {
    pid_t pid, sid;
    
    pid = fork();
    
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    
    umask(0);
    
    sid = setsid();
    
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }
    
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    while (1) {
        MYSQL *conn = connectMysql();
        
        MYSQL_RES *queueListRes = NULL;
        MYSQL_ROW queueListRow = NULL;
        
        queueListRes = executeQuery(conn, SEND_LIST_QUERY);
        
        int queueCnt = mysql_num_rows(queueListRes);
        
        char *quotationMark = "\"";
        size_t quotationMarkSize = strlen(quotationMark);
        
        char *comma = ",";
        size_t commaSize = strlen(comma);
        
        char *sendQuery = NULL;
        char *tmpGroupNo = NULL;
        char *message = NULL;
        char *authKey = NULL;
        
        int pushQueueCnt = 0;
        
        while ((queueListRow = mysql_fetch_row(queueListRes)) != NULL) {
            char *queueNo = queueListRow[0];
            char *registrationId = queueListRow[1];
            char *sendGroupNo = queueListRow[2];
            char *messageNo = queueListRow[3];

            const size_t queueNoSize = strlen(queueNo);
            const size_t registrationIdSize = strlen(registrationId);
            const size_t sendGroupNoSize = strlen(sendGroupNo);
            const size_t messageNoSize = strlen(messageNo);

            if (tmpGroupNo != NULL && sendQuery != NULL && atoi(tmpGroupNo) != atoi(sendGroupNo)) {
                message = getSendMessage(conn, messageNoSize, messageNo);
                authKey = queueListRow[4];

                sendPush(conn, authKey, &sendQuery, message, tmpGroupNo, pushQueueCnt);
                
                pushQueueCnt = 0;
                
                free(sendQuery);
                sendQuery = NULL;
            } else {
                tmpGroupNo = sendGroupNo;
                
                message = getSendMessage(conn, messageNoSize, messageNo);
                authKey = queueListRow[4];
            }
            
            size_t oneRegistrationIdSize = (quotationMarkSize * 2) + registrationIdSize + 1;
            
            char *oneRegistrationId = (char *) malloc(oneRegistrationIdSize);
            sprintf(oneRegistrationId, "%s%s%s",  quotationMark, registrationId, quotationMark);
            
            if (sendQuery == NULL) {
                sendQuery = (char *) malloc(oneRegistrationIdSize + 1);
                sprintf(sendQuery, "%s", oneRegistrationId);
                
                pushQueueCnt++;
            } else {
                const size_t sendQuerySize = strlen(sendQuery) + oneRegistrationIdSize + commaSize + 1;
                sendQuery = (char *) realloc(sendQuery, sendQuerySize);
                strcat(sendQuery, comma);
                strcat(sendQuery, oneRegistrationId);
                
                pushQueueCnt++;
            }

            const size_t deleteQuerySize = strlen(DELETE_QUEUE_QUERY);
            char *deleteQuery = (char *) malloc(deleteQuerySize + queueNoSize + 1);
            sprintf(deleteQuery, DELETE_QUEUE_QUERY, queueNo);

            executeQuery(conn, deleteQuery);

            free(deleteQuery);
            deleteQuery = NULL;
            
            free(oneRegistrationId);
            oneRegistrationId = NULL;
        }
        
        if (sendQuery != NULL) {
            sendPush(conn, authKey, &sendQuery, message, tmpGroupNo, pushQueueCnt);
            
            pushQueueCnt = 0;
            
            free(sendQuery);
            sendQuery = NULL;
        }
        
        if (message != NULL) {
            free(message);
            message = NULL;
        }
        
        mysql_free_result(queueListRes);
        
        if (conn != NULL) {
            mysql_close(conn);
            conn = NULL;
            
            mysql_thread_end();
        }
    }
    
    sleep(5);
    
    exit(EXIT_SUCCESS);
}
