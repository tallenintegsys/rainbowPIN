#include <stdio.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>

int main(void)
{
	EVP_MD_CTX *ctx = NULL;
	EVP_MD *sha256 = NULL;
	const unsigned char msg[] = {
		0x00, 0x01, 0x02, 0x03
	};
	unsigned int len = 0;
	unsigned char *outdigest = NULL;
	int ret = 1;

	/* Create a context for the digest operation */
	ctx = EVP_MD_CTX_new();
	if (ctx == NULL)
		goto err;

	/*
	 * Fetch the SHA256 algorithm implementation for doing the digest. We're
	 * using the "default" library context here (first NULL parameter), and
	 * we're not supplying any particular search criteria for our SHA256
	 * implementation (second NULL parameter). Any SHA256 implementation will
	 * do.
	 */
	sha256 = EVP_MD_fetch(NULL, "SHA256", NULL);
	if (sha256 == NULL)
		goto err;


	/* Initialise the digest operation
	   EVP_DigestInit()
	   Behaves in the same way as EVP_DigestInit_ex2() except it doesn't set any parameters and calls EVP_MD_CTX_reset() so it cannot be used with an type of NULL.

	   int EVP_DigestInit_ex(EVP_MD_CTX *ctx, const EVP_MD *type, ENGINE *impl);
	   */
	if (!EVP_DigestInit_ex(ctx, sha256, NULL))
		goto err;

	/* Pass the message to be digested. This can be passed in over multiple EVP_DigestUpdate calls if necessary
	   EVP_DigestUpdate()
	   Hashes cnt bytes of data at d into the digest context ctx. This function can be called several times on the same ctx to hash additional data.

	   int EVP_DigestUpdate(EVP_MD_CTX *ctx, const void *d, size_t cnt);
	   */
	if (!EVP_DigestUpdate(ctx, msg, sizeof(msg)))
		goto err;

	/* Allocate the output buffer
	   EVP_MD_get_size()
	   Return the size of the message digest when passed an EVP_MD or an EVP_MD_CTX structure, i.e. the size of the hash.

	   int EVP_MD_get_size(const EVP_MD *md);
	   */
	outdigest = OPENSSL_malloc(EVP_MD_get_size(sha256));
	if (outdigest == NULL)
		goto err;

	/* Now calculate the digest itself
	   EVP_DigestFinal()
	   Similar to EVP_DigestFinal_ex() except after computing the digest the digest context ctx is automatically cleaned up with EVP_MD_CTX_reset().

	   int EVP_DigestFinal_ex(EVP_MD_CTX *ctx, unsigned char *md, unsigned int *s);
	   */
	if (!EVP_DigestFinal_ex(ctx, outdigest, &len))
		goto err;

	/* EVP_Digest()
	   A wrapper around the Digest Init_ex, Update and Final_ex functions.  Hashes count bytes of data at data using a digest type from ENGINE impl. The digest value is placed
	   in md and its length is written at size if the pointer is not NULL. At most EVP_MAX_MD_SIZE bytes will be written.  If impl is NULL the default implementation of digest
	   type is used.

	   int EVP_Digest(const void *data, size_t count, unsigned char *md, unsigned int *size, const EVP_MD *type, ENGINE *impl);
	   */

	/* Print out the digest result */
	BIO_dump_fp(stdout, outdigest, len);

	ret = 0;

err:
	/* Clean up all the resources we allocated */
	OPENSSL_free(outdigest);
	EVP_MD_free(sha256);
	EVP_MD_CTX_free(ctx);
	if (ret != 0)
		ERR_print_errors_fp(stderr);
	return ret;
}
