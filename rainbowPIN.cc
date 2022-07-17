#include <cstring>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <new>
#include <openssl/sha.h>
#include <queue>
#include <signal.h>
#include <sqlite3.h>
#include <sstream>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <thread>

// per the SQLite docs, "Threads are evil. Avoid them."
// PRAGMA synchronous=OFF don't wait for fflush
// INTEGER PRIMARY KEY the PIN
struct Hash {
	uint8_t pin[10];
	uint8_t hash[SHA256_DIGEST_LENGTH];
	friend std::stringstream &operator<<(std::stringstream &ss, const Hash &h);
};

std::stringstream &operator<<(std::stringstream &ss, const Hash &h) {
	ss << "(\"";
	ss << h.pin[0] << h.pin[1] << h.pin[2] << h.pin[3] << h.pin[4] << h.pin[5] << h.pin[6] << h.pin[7] << h.pin[8];
	ss << "\", \"" << std::hex;
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[0] << std::setw(2) << std::setfill('0') << (int)h.hash[1];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[2] << std::setw(2) << std::setfill('0') << (int)h.hash[3];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[4] << std::setw(2) << std::setfill('0') << (int)h.hash[5];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[6] << std::setw(2) << std::setfill('0') << (int)h.hash[7];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[8] << std::setw(2) << std::setfill('0') << (int)h.hash[9];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[10] << std::setw(2) << std::setfill('0') << (int)h.hash[11];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[12] << std::setw(2) << std::setfill('0') << (int)h.hash[13];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[14] << std::setw(2) << std::setfill('0') << (int)h.hash[15];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[16] << std::setw(2) << std::setfill('0') << (int)h.hash[17];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[18] << std::setw(2) << std::setfill('0') << (int)h.hash[19];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[20] << std::setw(2) << std::setfill('0') << (int)h.hash[21];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[22] << std::setw(2) << std::setfill('0') << (int)h.hash[23];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[24] << std::setw(2) << std::setfill('0') << (int)h.hash[25];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[26] << std::setw(2) << std::setfill('0') << (int)h.hash[27];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[28] << std::setw(2) << std::setfill('0') << (int)h.hash[29];
	ss << std::setw(2) << std::setfill('0') << (int)h.hash[30] << std::setw(2) << std::setfill('0') << (int)h.hash[31];
	ss << "\")";
	//	std::cout << ss.str() << "\n";
	return ss;
}

std::mutex qmutex;
std::queue<Hash> queue;

void hasher(void) {
	Hash h;
   	SHA256_CTX ctx;
    SHA256_Init(&ctx);

	for (uint32_t i = 0; i < 999999999; i++) {
		sprintf((char *)h.pin, "%9.9u", i);
		SHA256_Update(&ctx, h.pin, 9);
		SHA256_Final(h.hash, &ctx);
		qmutex.lock();
		queue.push(h);
		qmutex.unlock();
		//	std::cout << i << "\n";
		while (queue.size() > 1000) // idle stabilizer
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
	}
}

void sigabort(int sig) {
	std::cerr << "queue size: " << queue.size() << std::endl;
}

int main(int argc, char **argv) {
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	std::stringstream sql;

	signal(SIGABRT, sigabort);

	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << "file.db\n";
		return (1);
	}
	rc = sqlite3_open(argv[1], &db);
	if (rc) {
		std::cerr << "Can't open database: " << sqlite3_errmsg(db) << "\n";
		sqlite3_close(db);
		return (1);
	}
	sql << "CREATE TABLE rainbow (pin VARCHAR(9), hash VARCHAR(32));\n";
	sql << "PRAGMA synchronous=OFF;\n";
	rc = sqlite3_exec(db, sql.str().c_str(), NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		std::cerr << "SQL error: " << zErrMsg << "\n";
		sqlite3_free(zErrMsg);
	}
	//	std::cout << sql.str();

	std::thread hasherThread(hasher);
	while (queue.size() < 10)
		;

	bool run = true;
	uint8_t emptyQueueCount = 0;
	uint32_t count = 0;
	while (run) {
		if (count++ == 1000) {
			count = 0;
			std::cout << queue.size() << "\n";
		}
		sql.str("");
		sql << "INSERT INTO rainbow VALUES ";
		qmutex.lock();
		Hash h = queue.front();
		sql << h << ", \n";
		queue.pop();
		h = queue.front();
		sql << h << ", \n";
		queue.pop();
		h = queue.front();
		sql << h << ", \n";
		queue.pop();
		h = queue.front();
		sql << h << ", \n";
		queue.pop();
		h = queue.front();
		sql << h << ", \n";
		queue.pop();
		h = queue.front();
		sql << h << ", \n";
		queue.pop();
		h = queue.front();
		sql << h << ", \n";
		queue.pop();
		h = queue.front();
		sql << h << ", \n";
		queue.pop();
		h = queue.front();
		sql << h << ", \n";
		queue.pop();
		h = queue.front();
		sql << h << ";\n";
		queue.pop();
		qmutex.unlock();
		//	std::cout << sql.str() << "\n";
		rc = sqlite3_exec(db, sql.str().c_str(), NULL, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			std::cerr << "SQL error: " << zErrMsg << "\n";
			sqlite3_free(zErrMsg);
		}
		if (queue.size() < 10) {
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
			if (!++emptyQueueCount) {
				run = false;
			}
		} else {
			emptyQueueCount = 0;
		}
	} // while
	std::cerr << "finishing\n";
	hasherThread.join();
	while (queue.size()) { // empty the queue
		sql.str("");
		Hash h = queue.front();
		sql << "INSERT INTO rainbow VALUES ";
		sql << h << ";\n";
		queue.pop();
		//	std::cout << sql.str() << "\n";
		rc = sqlite3_exec(db, sql.str().c_str(), NULL, 0, &zErrMsg);
		if (rc != SQLITE_OK) {
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
	}

	sqlite3_close(db);
	return 0;
}
