#include <mysql/mysql.h>

MYSQL* connect_to_database();
int database_read_all_accounts(MYSQL*);
int database_add_new_user(MYSQL*, char*, char*);