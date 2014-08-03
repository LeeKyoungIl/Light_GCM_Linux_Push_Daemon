#include <regex.h>
#include <math.h>

struct ResultPush {
    char *successCnt;
    char *faildCnt;
    
    size_t successCntSize;
    size_t faildCntSize;
};

void regexCheck(char *pattern, char *data, struct ResultPush **resultPush) {
    char *patternData = (char *) malloc(strlen(pattern) + 1);
    char *dataValue = (char *) malloc(strlen(data) + 1);
    
    memcpy(patternData, pattern, strlen(pattern) + 1);
    memcpy(dataValue, data, strlen(data) + 1);
    
    regex_t regex;
    
    regmatch_t matches[100];
    int status;
    int numchars;
    
    status = regcomp(&regex, patternData, REG_EXTENDED);
    
    if (status == 0) {
        int i, loopCnt = 0;
    
        status = regexec(&regex, dataValue, 20, matches, 0);
    
        if (status == 0) {
            for (i=0; i<20; i++) {
                if ((int)matches[i].rm_so < 0) {
                    break;
                }
            
                numchars = (int)matches[i].rm_eo - (int)matches[i].rm_so;
            
                char *valueData = (char *) malloc(numchars + 1);
                strncpy(valueData, dataValue+matches[i].rm_so, numchars);
                const size_t valueDataSize = strlen(valueData);            

                if (loopCnt == 1) {
                    (*resultPush)->successCnt = (char *) malloc(valueDataSize + 1);
                    sprintf((*resultPush)->successCnt, "%s", valueData);
                    (*resultPush)->successCntSize = strlen((*resultPush)->successCnt);
                } else if (loopCnt == 2) {
                    (*resultPush)->faildCnt = (char *) malloc(valueDataSize + 1);
                    sprintf((*resultPush)->faildCnt, "%s", valueData);
                    (*resultPush)->faildCntSize = strlen((*resultPush)->faildCnt);
                }
            
                loopCnt++;
            
                free(valueData);
                valueData = NULL;
            }
        }
    }
    
    free(patternData);
    patternData = NULL;
    
    free(dataValue);
    dataValue = NULL;
    
    regfree(&regex);
}

void sendPush (MYSQL *conn, char *authKey, char **sendQuery, char *message, char *sendGroupNo, int totalCnt) {
    char *targetUrl = "https://android.googleapis.com/gcm/send";
    
    char *sendData = "{\"registration_ids\":[%s],\"data\":{\"msg\":\"%s\"}}";
    size_t sendDataSize = strlen(sendData);
    
    const size_t sendGCMSize = sendDataSize + strlen(*sendQuery) + strlen(message) + 1;
    char *sendGCM = (char *) malloc(sendGCMSize);
    sprintf(sendGCM, sendData, *sendQuery, message);
    
    struct string pushCheck = send_post_request(authKey, targetUrl, sendGCM);
    
    struct ResultPush *resultPush;
    bool memoryCheck = true;    

    if ((resultPush = (struct ResultPush *) malloc(sizeof(struct ResultPush))) == NULL) {
        memoryCheck = false;
    }

    if (memoryCheck) {
        resultPush->successCnt = NULL;
        resultPush->faildCnt = NULL;

        regexCheck(".*success\":(.*),\"failure\":(.*),\"canonical_ids.*", pushCheck.ptr, &resultPush);
    
        if (resultPush->successCnt != NULL && resultPush->faildCnt != NULL) {
            const size_t pushResultQueryBodySize = strlen(INSERT_RECORD_QUERY);
            const size_t pushResultQuerySize = pushResultQueryBodySize + strlen(sendGroupNo) + pushCheck.len + (floor(log10(abs(totalCnt))) + 1) + resultPush->successCntSize + resultPush->faildCntSize + 1;
            char *pushResultQuery = (char *) malloc(pushResultQuerySize);
            sprintf(pushResultQuery, INSERT_RECORD_QUERY, sendGroupNo, pushCheck.ptr, totalCnt, resultPush->successCnt, resultPush->faildCnt);
    
            executeQuery(conn, pushResultQuery);
    
            free(resultPush->successCnt);
            resultPush->successCnt = NULL;
    
            free(resultPush->faildCnt);
            resultPush->faildCnt = NULL;

            free(pushResultQuery);
            pushResultQuery = NULL;
        }

        free(resultPush);
        resultPush = NULL;
    }
    
    free(pushCheck.ptr);
    pushCheck.ptr = NULL;
}

char *getSendMessage (MYSQL *conn, size_t messageNoSize, char* messageNo) {
    char *message = NULL;
    
    const size_t messageQuerySize = strlen(SEND_MESSAGE_QUERY) + messageNoSize + 1;
    char *getMessageQuery = (char *) malloc(messageQuerySize);
    sprintf(getMessageQuery, SEND_MESSAGE_QUERY, messageNo);
    
    MYSQL_RES *messageRes = NULL;
    MYSQL_ROW messageRow = NULL;
    
    messageRes = executeQuery(conn, getMessageQuery);
    
    int messageCnt = mysql_num_rows(messageRes);
    
    if (messageCnt > 0) {
        if ((messageRow = mysql_fetch_row(messageRes)) != NULL) {
            const size_t messageSize = strlen(messageRow[0]) + 1;
            message = (char *) malloc(messageSize);
            sprintf(message, "%s", messageRow[0]);
        }
        
        mysql_free_result(messageRes);
    }
    
    return message;
}
