#include <stdio.h>
#include <sqlite3.h>
#include <stdint.h>
#include <thread>
#include <deque>
#include <cstring>

// per the SQLite docs, "Threads are evil. Avoid them."
// PRAGMA synchronous=OFF don't wait for fflush
// INTEGER PRIMARY KEY the PIN 
typedef struct {
	uint64_t	pin;
	char		hash[32];
} hash;

std::deque<hash> queue;

void hasher(void) {
	hash h;
	for (uint64_t i = 0; i < 10000; i++) {
		h.pin = i;
		sprintf(h.hash, "1234567890123456789012345678901"); //XXX BS placeholder
		queue.push_back(h);
	}
}

int main(int argc, char **argv) {
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char sql[16384];

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

	uint8_t count = 0;
	while (true) {
		if (queue.size() < 10) {
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			if (++count) 
				continue;
			break;
		}
		sprintf(sql, "INSERT INTO rainbow VALUES");
		hash h = queue.front();
		sprintf(sql + strlen(sql), " (%ld, \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (%ld, \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (%ld, \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (%ld, \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (%ld, \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (%ld, \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (%ld, \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (%ld, \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (%ld, \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		queue.pop_front();
		sprintf(sql + strlen(sql), " (%ld, \"%s\");", h.pin, h.hash);
		int rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
	}
	hasherThread.join();
	while (queue.size()) {	// empty the queue
		queue.pop_front();
		hash h = queue.front();
		sprintf(sql, "INSERT INTO rainbow VALUES (%ld, \"%s\"),", h.pin, h.hash);
	}

	sqlite3_close(db);
	return 0;
}
