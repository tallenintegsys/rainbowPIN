#include <stdio.h>
#include <sqlite3.h>
#include <stdint.h>
#include <thread>
#include <string>
#include <iostream>
#include <sstream>
#include <queue>
#include <cstring>
#include <openssl/evp.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <new>
#include <mutex>

// per the SQLite docs, "Threads are evil. Avoid them."
// PRAGMA synchronous=OFF don't wait for fflush
// INTEGER PRIMARY KEY the PIN 
struct Hash {
	uint8_t		pin[10];
	uint8_t		hash[32];
	friend std::stringstream& operator<< (std::stringstream &ss, const Hash &h);
};

std::stringstream& operator<< (std::stringstream &ss, const Hash &h) {
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
	//std::cout << ss.str() << "\n";
	return ss;
};

std::mutex qmutex;
std::queue<Hash> queue;

void hasher(void) {
	Hash h;
	unsigned int len = 0;

	EVP_MD_CTX *ctx = EVP_MD_CTX_new();
	EVP_MD *sha256 = EVP_MD_fetch(NULL, "SHA256", NULL);

	for (uint32_t i = 0; i < 999999999; i++) {
		sprintf((char *)h.pin, "%9.9u", i);
		//std::cout << h->pin << "\n";
		EVP_DigestInit_ex(ctx, sha256, NULL);
		EVP_DigestUpdate(ctx, h.pin, 9);
		EVP_DigestFinal_ex(ctx, h.hash, &len);
		qmutex.lock();
		queue.emplace(h);
		qmutex.unlock();
		//std::cout << i << "\n";
	}
}

int main(int argc, char **argv) {
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	std::stringstream sql;

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
	sql  << "CREATE TABLE rainbow (pin VARCHAR(9), hash VARCHAR(32));\n";
	sql  << "PRAGMA synchronous=OFF;\n";
	rc = sqlite3_exec(db, sql.str().c_str(), NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		std::cerr << "SQL error: " << zErrMsg << "\n";
		sqlite3_free(zErrMsg);
	}
	//std::cout << sql.str();

	std::thread hasherThread(hasher);
	while (queue.size() < 10);

	bool run = 1;
	while (run) {
		uint8_t count = 0;
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
		//std::cout << sql.str() << "\n";
		rc = sqlite3_exec(db, sql.str().c_str(), NULL, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
//			std::cerr << "SQL error: " << zErrMsg << "\n";
			sqlite3_free(zErrMsg);
		}
		while (queue.size() < 10) {
			std::this_thread::sleep_for(std::chrono::microseconds(1000));
			if (!++count) {
				run = 0;
				break;
			}
		}
	}
	hasherThread.join();
	while (queue.size()) {	// empty the queue
		sql.str("");
		Hash h = queue.front();
		sql << "INSERT INTO rainbow VALUES ";
		sql << h << ";\n";
		queue.pop();
		//std::cout << sql.str() << "\n";
		rc = sqlite3_exec(db, sql.str().c_str(), NULL, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
		}
	}

	sqlite3_close(db);
	return 0;
}
