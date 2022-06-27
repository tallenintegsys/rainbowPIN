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

// per the SQLite docs, "Threads are evil. Avoid them."
// PRAGMA synchronous=OFF don't wait for fflush
// INTEGER PRIMARY KEY the PIN 

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
	sql  << "CREATE TABLE rainbow (pin VARCHAR(9), hash VARCHAR);\n";
	sql  << "PRAGMA synchronous=OFF;\n";
	rc = sqlite3_exec(db, sql.str().c_str(), NULL, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		std::cerr << "SQL error: " << zErrMsg << "\n";
		sqlite3_free(zErrMsg);
	}
	//std::cout << sql.str();

	char pin[10];
	uint8_t hash[64];
	unsigned int len = 0;

	EVP_MD_CTX *ctx = EVP_MD_CTX_new();
	EVP_MD *sha256 = EVP_MD_fetch(NULL, "SHA256", NULL);

	for (uint32_t i = 0; i < 1000000000; i++) {
		sql.str("");
		sprintf(pin, "%9.9u", i);
		//std::cout << h->pin << "\n";
		EVP_DigestInit_ex(ctx, sha256, NULL);
		EVP_DigestUpdate(ctx, pin, 9);
		EVP_DigestFinal_ex(ctx, hash, &len);
		//std::cout << i << "\n";

		sql << "INSERT INTO rainbow VALUES ";
		sql << "(\"" << pin << "\", \"";
		for (int j=0; j<32; j++)
			sql << std::hex << std::setw(2) << std::setfill('0') << (int)hash[j];
		sql << "\");\n";
		//std::cout << sql.str() << "\n";
		rc = sqlite3_exec(db, sql.str().c_str(), NULL, 0, &zErrMsg);
		if( rc!=SQLITE_OK ){
			fprintf(stderr, "SQL error: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			i--; //retry
		}
	}

	sqlite3_close(db);
	return 0;
}
