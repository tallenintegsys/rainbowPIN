#include <stdio.h>
#include <sqlite3.h>
#include <stdint.h>
#include <thread>
#include <deque>
#include <cstring>
#include <openssl/evp.h>

// per the SQLite docs, "Threads are evil. Avoid them."
// PRAGMA synchronous=OFF don't wait for fflush
// INTEGER PRIMARY KEY the PIN 
typedef struct {
	uint8_t		pin[10];
	uint8_t		hash[32];
} hash;

std::deque<hash> queue;

void hasher(void) {
	hash h;
	unsigned int len = 0;

	EVP_MD_CTX *ctx = EVP_MD_CTX_new();
	EVP_MD *sha256 = EVP_MD_fetch(NULL, "SHA256", NULL);

	for (uint32_t i = 0; i < 9996; i++) {
		sprintf((char *)h.pin, "%9.9u", i);
		EVP_DigestInit_ex(ctx, sha256, NULL);
		EVP_DigestUpdate(ctx, h.pin, 9);
		EVP_DigestFinal_ex(ctx, h.hash, &len);
		queue.push_back(h);
		/*
		printf("%s, ", h.pin);
		for (int i = 0; i < 32; i++)
			printf("%2.2x", h.hash[i]);
		printf("\n");
		*/
	}
}

int main(int argc, char **argv) {
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char sql[32768];

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
	sprintf(sql, "CREATE TABLE rainbow (pin VARCHAR(9), hash VARCHAR);");
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
	while (queue.size() < 10);

	bool run = 1;
	while (run) {
		uint8_t count = 0;
		sprintf(sql, "INSERT INTO rainbow VALUES");
		hash h = queue.front();
		sprintf(sql + strlen(sql), " (\"%s\", \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (\"%s\", \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (\"%s\", \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (\"%s\", \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (\"%s\", \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (\"%s\", \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (\"%s\", \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (\"%s\", \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (\"%s\", \"%s\"),", h.pin, h.hash);
		queue.pop_front();
		h = queue.front();
		sprintf(sql + strlen(sql), " (\"%s\", \"%s\");", h.pin, h.hash);
		queue.pop_front();
		rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
		while (queue.size() < 10) {
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			if (!++count) {
				run = 0;
				break;
			}
		}
	}
	hasherThread.join();
	while (queue.size()) {	// empty the queue
		hash h = queue.front();
		sprintf(sql, "INSERT INTO rainbow VALUES (\"%s\", \"%s\");", h.pin, h.hash);
		queue.pop_front();
		rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
	}

	sqlite3_close(db);
	return 0;
}
