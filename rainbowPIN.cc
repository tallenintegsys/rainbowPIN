#include <stdio.h>
#include <sqlite3.h>
#include <stdint.h>
#include <thread>

// per the SQLite docs, "Threads are evil. Avoid them."
// PRAGMA synchronous=OFF don't wait for fflush
// INTEGER PRIMARY KEY the PIN 
void hasher(void) {
	const uint32_t batch = 10;
	for (uint64_t i = 0; i < 999999999; i += batch) {
		for (uint32_t j = 0; j < batch; j++) {
			// queue
		}
	}
}

void dber(sqlite3 *db) {
	char *zErrMsg = 0;
	char sql[1024];
	uint64_t i; //XXX

	sprintf(sql, "INSERT INTO rainbow VALUES (%ld, \"%lx\");", i, i);
	int rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
}

int main(int argc, char **argv){
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char sql[1024];

	if( argc!=2 ){
		fprintf(stderr, "Usage: %s file.db\n", argv[0]);
		return(1);
	}
	rc = sqlite3_open(argv[1], &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	sprintf(sql, "CREATE TABLE rainbow (pin INTEGER PRIMARY KEY, hash VARCHAR);");
	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sprintf(sql, "PRAGMA synchronous=OFF;");
	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}

	std::thread hasherThread(hasher);
	std::thread dberThread(dber(db));

	sqlite3_close(db);
	return 0;
}
