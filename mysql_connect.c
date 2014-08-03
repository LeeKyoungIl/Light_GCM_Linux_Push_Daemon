#include <mysql/mysql.h>

/**
 * MySQL connect info define
 *
 * - MYSQL_HOST : Insert MySQL server ip address or domain address
 * - MYSQL_PORT : Insert MySQL server port (ex 3306)
 * - MYSQL_USER : Insert MySQL user ID to use for server connection
 * - MYSQL_PASS : Insert MySQL user password to use for server connection
 * - MYSQL_DB : Insert MySQL DB name to use for push service
 */
#define MYSQL_HOST "127.0.0.1"
#define MYSQL_PORT 3306
#define MYSQL_USER "test_user"
#define MYSQL_PASS "test_password"
#define MYSQL_DB "test_db"

/**
 * Push Daemon query set
 *
 * - SEND_LIST_QUERY : return list of push queue
 * - SEND_MESSAGE_QUERY : return push message
 * - INSERT_RECORD_QUERY : insert push result
 * - DELETE_QUEUE_QUERY : delete push queue
 */
#define SEND_LIST_QUERY "SELECT queue.no, user.regstration_id, queue.send_group_no, queue.message_no, app.auth_key FROM llgcm_app as app INNER JOIN llgcm_queue as queue ON app.no = queue.app_no INNER JOIN llgcm_user as user ON queue.user_no = user.no  WHERE queue.status = 'pending' and user.status = 'active' ORDER BY send_group_no ASC LIMIT 0, 1000"
#define SEND_MESSAGE_QUERY "SELECT message FROM llgcm_message WHERE no=%s"
#define INSERT_RECORD_QUERY "INSERT INTO llgcm_record SET send_group_no=%s, result_log='%s', total_cnt=%d, success_cnt=%s, faild_cnt=%s"
#define DELETE_QUEUE_QUERY "DELETE FROM llgcm_queue WHERE no=%s"


/**
 * MySQL Connect
 *
 * Connect to MySQL Server
 *
 * return MySQL Object
 */
MYSQL *connectMysql (void) {
	mysql_thread_init();
    
	MYSQL *conn = mysql_init(NULL);
    
	my_bool myb = 1;
	mysql_options(conn, MYSQL_OPT_RECONNECT, &myb);
    
	if (!mysql_real_connect(conn, MYSQL_HOST, MYSQL_USER, MYSQL_PASS, MYSQL_DB, MYSQL_PORT, (char *)NULL, 0)) {
		return NULL;
	}
    
	mysql_set_character_set(conn,  "utf8");
    
	return conn;
}

/**
 * MySQL Connect Check
 *
 * If MySQL Connect Object is null, retrying connect to MySQL 1 second break and up to three times.
 *
 * @param MYSQL *conn : MySQL Object
 *
 * return MySQL Object
 */
MYSQL *connectMysqlCheck (MYSQL *conn) {
    if (conn == NULL) {
        int i=0;
        
        for (i=0; i<3; i++) {
            MYSQL *conn = connectMysql();
            
            if (conn != NULL) {
                return conn;
            }
            
            sleep(1);
        }
    }
    
    return conn;
}

/**
 * MySQL executeQuery
 *
 * @param MYSQL *conn : MySQL Object
 * @param char *query : MySQL Query sentence
 *
 * return MySQL RESULT SET
 */
MYSQL_RES *executeQuery (MYSQL *conn, char *query) {
    if (conn == NULL) {
        if((conn = connectMysqlCheck(conn)) == NULL) {
            return NULL;
        }
    }
    
    mysql_query(conn, query);
    
    return mysql_store_result(conn);
}
