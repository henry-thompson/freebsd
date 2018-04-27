#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>

#include <atf-c.h>

#define PAGE_SIZE (getpagesize())
#define GET_PAGE_ADDR(addr) (((size_t) addr) - (((size_t) addr) % PAGE_SIZE))

ATF_TC_WITHOUT_HEAD(mwritten__returns_einval_when_len_zero);
ATF_TC_BODY(mwritten__returns_einval_when_len_zero, tc) {
	char *heap = malloc(sizeof(char) * 1024);

	size_t naddr = 3;
	uintptr_t buf[naddr];
	size_t gran = 0;

	int res = mwritten(heap, 0, 0, buf, &naddr, &gran);

	ATF_CHECK_MSG(res == EINVAL,
		"EINVAL expected, but mwritten outputted %d instead", res);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__returns_einval_when_addr_illegal);
ATF_TC_BODY(mwritten__returns_einval_when_addr_illegal, tc) {
	char *heap = malloc(PAGE_SIZE * 3);

	size_t naddr = 3;
	uintptr_t buf[naddr];
	size_t gran = 0;

	int res = mwritten((void*)((uintptr_t) heap * 1000), PAGE_SIZE, 0, buf, &naddr, &gran);

	ATF_CHECK_MSG(res == EINVAL,
		"EINVAL expected, but mwritten outputted %d instead", res);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__returns_einval_when_len_beyond_range);
ATF_TC_BODY(mwritten__returns_einval_when_len_beyond_range, tc) {
	char *heap = malloc(PAGE_SIZE * 3);

	size_t naddr = 3;
	uintptr_t buf[naddr];
	size_t gran = 0;

	int res = mwritten(heap, PAGE_SIZE * 1000000, 0, buf, &naddr, &gran);

	ATF_CHECK_MSG(res == EINVAL,
		"EINVAL expected, but mwritten outputted %d instead", res);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__returns_efault_when_bad_buffer_start_passed_in);
ATF_TC_BODY(mwritten__returns_efault_when_bad_buffer_start_passed_in, tc) {
	char *heap = malloc(PAGE_SIZE * 3);

	size_t naddr = 3;
	size_t gran = 0;

	int res = mwritten(heap, PAGE_SIZE, 0, (void*)3, &naddr, &gran);

	ATF_CHECK_MSG(res == EFAULT,
		"EFAULT expected, but mwritten outputted %d instead", res);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__returns_efault_when_bad_gran_addr_passed_in);
ATF_TC_BODY(mwritten__returns_efault_when_bad_gran_addr_passed_in, tc) {
	char *heap = malloc(PAGE_SIZE * 3);

	size_t naddr = 3;
	uintptr_t buf[naddr];

	int res = mwritten(heap, PAGE_SIZE, 0, buf, &naddr, NULL);

	ATF_CHECK_MSG(res == EFAULT,
		"EFAULT expected, but mwritten outputted %d instead", res);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__returns_efault_when_zero_naddr_passed_in);
ATF_TC_BODY(mwritten__returns_efault_when_zero_naddr_passed_in, tc) {
	char *heap = malloc(PAGE_SIZE * 3);

	size_t naddr = 0;
	uintptr_t buf[3];
	size_t gran = 0;

	int res = mwritten(heap, PAGE_SIZE, 0, buf, &naddr, &gran);

	ATF_CHECK_MSG(res == EFAULT,
		"EFAULT expected, but mwritten outputted %d instead", res);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__returns_efault_when_too_large_naddr_passed_in);
ATF_TC_BODY(mwritten__returns_efault_when_too_large_naddr_passed_in, tc) {
	size_t len = sizeof(char) * PAGE_SIZE * 2048;
	char *heap = malloc(len);

	// Have to fill the buffer for it to go wrong and try to write beyond
	// allocated memory.
	for (int i = 0; i < 2048; i++) {
		heap[(i + 13) * PAGE_SIZE + 123] ^= 'a';
	}

	uintptr_t buf[2];
	size_t gran = 0;
	size_t naddr = 2048;

	int res = mwritten(heap, PAGE_SIZE, 0, buf, &naddr, &gran);

	ATF_CHECK_MSG(res == EFAULT,
		"EFAULT expected, but mwritten outputted %d instead", res);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__returns_einval_when_naddr_null_and_clear_flag_unset);
ATF_TC_BODY(mwritten__returns_einval_when_naddr_null_and_clear_flag_unset, tc) {
	char *heap = malloc(PAGE_SIZE * 3);

	size_t naddr = 3;
	uintptr_t buf[naddr];
	size_t gran = 0;

	int res = mwritten(heap, PAGE_SIZE, 0, buf, NULL, &gran);

	ATF_CHECK_MSG(res == EINVAL,
		"EINVAL expected, but mwritten outputted %d instead", res);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__returns_einval_when_buf_null_and_clear_flag_unset);
ATF_TC_BODY(mwritten__returns_einval_when_buf_null_and_clear_flag_unset, tc) {
	char *heap = malloc(PAGE_SIZE * 3);

	size_t naddr = 3;
	size_t gran = 0;

	int res = mwritten(heap, PAGE_SIZE, 0, NULL, &naddr, &gran);

	ATF_CHECK_MSG(res == EINVAL,
		"EINVAL expected, but mwritten outputted %d instead", res);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__sets_naddr_zero_with_no_writes);
ATF_TC_BODY(mwritten__sets_naddr_zero_with_no_writes, tc) {
	size_t len = sizeof(char) * 1024;
	char *heap = malloc(len);

	size_t naddr = 3;
	uintptr_t buf[naddr];
	size_t gran = 0;

	int res = mwritten(heap, len, 0, buf, &naddr, &gran);

	ATF_CHECK_MSG(res == 0,
		"mwritten failed with error code %d", res);
	ATF_CHECK_MSG(naddr == 0,
		"naddr = 0 expected, but %zu was returned", naddr);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__returns_einval_if_given_illegal_addr);
ATF_TC_BODY(mwritten__returns_einval_if_given_illegal_addr, tc) {
	int len = PAGE_SIZE * 3;
	char *heap = malloc(len);

	size_t naddr = 1;
	uintptr_t buf[naddr];
	size_t gran = 0;

	int res = mwritten((void*)((uintptr_t) heap + PAGE_SIZE * 100000000), len, 0, buf, &naddr, &gran);

	ATF_CHECK_MSG(res == EINVAL,
		"EFAULT expected, but mwritten outputted %d instead", res);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__correctly_detects_single_modified_address);
ATF_TC_BODY(mwritten__correctly_detects_single_modified_address, tc) {
	size_t len = sizeof(char) * PAGE_SIZE * 5;
	char *heap = malloc(len);

	int modifiedIndex = PAGE_SIZE * 3 + 123;
	heap[modifiedIndex] ^= 'a';

	size_t naddr = 3;
	uintptr_t buf[naddr];
	size_t gran = 0;

	int res = mwritten(heap, len, 0, buf, &naddr, &gran);

	void *modified = (void *) buf[0];
	uintptr_t actualModifiedAddress = GET_PAGE_ADDR((void*)((uintptr_t) heap + modifiedIndex));

	ATF_CHECK_MSG(res == 0,
		"mwritten failed with error code %d", res);
	ATF_CHECK_MSG(naddr == 1,
		"naddr = 1 expected, but %zu was returned", naddr);
	ATF_CHECK_MSG(modified == (void*)actualModifiedAddress,
		"address of modified page is incorrect");
	ATF_CHECK_MSG((int)gran == PAGE_SIZE,
		"returned page size, %zu, was incorrect", gran);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__correctly_detects_many_modified_addresses);
ATF_TC_BODY(mwritten__correctly_detects_many_modified_addresses, tc) {
	size_t len = PAGE_SIZE * 1024;
	char *heap = malloc(len);

	// Modify every other page in the heap, starting from the
	// 13th page in the heap
	for (int i = 0; i < 128; i++) {
		heap[(i * 2 + 13) * PAGE_SIZE + 123] ^= 'a';
	}

	size_t naddr = 512;
	uintptr_t buf[naddr];
	size_t gran = 0;

	int res = mwritten(heap, len, 0, buf, &naddr, &gran);

	ATF_CHECK_MSG(res == 0,
		"mwritten failed with error code %d", res);
	ATF_CHECK_MSG(naddr == 128,
		"naddr = 128 expected, but %zu was returned", naddr);
	ATF_CHECK_MSG((int)gran == PAGE_SIZE,
		"returned page size, %zu, was incorrect", gran);

	// Make sure the right address was detected
	for (int i = 0; i < 128; i++) {
		uintptr_t expectedAddr = GET_PAGE_ADDR(heap) +
			(i * 2 + 13) * PAGE_SIZE;

		ATF_CHECK_MSG(buf[i] == expectedAddr,
			"Incorrect address(es) returned: address number %d in "
			"buffer had value %p; expecting %p. Aborting checking the "
			"rest of the addresses.",
			i, (void*)buf[i], (void*)expectedAddr);
			break;
	}

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__fills_buffer_and_returns_if_too_many_writes);
ATF_TC_BODY(mwritten__fills_buffer_and_returns_if_too_many_writes, tc) {
	size_t len = PAGE_SIZE * 10;
	char *heap = malloc(len);

	// Writes to 5 pages
	for (int i = 0; i < 5; i++) {
		heap[(i * 2) * PAGE_SIZE + 123] ^= 'a';
	}

	size_t naddr = 2;
	uintptr_t buf[naddr];
	size_t gran = 0;

	int res = mwritten(heap, len, 0, buf, &naddr, &gran);

	void *modified0 = (void *) buf[0];
	uintptr_t actualModifiedAddress0 = GET_PAGE_ADDR((void*)((uintptr_t) heap + 123));

	void *modified1 = (void *) buf[1];
	uintptr_t actualModifiedAddress1 = GET_PAGE_ADDR(
		(void*)((uintptr_t) heap + (2 * PAGE_SIZE) + 123));

	ATF_CHECK_MSG(res == 0,
		"mwritten failed with error code %d", res);
	ATF_CHECK_MSG(naddr == 2,
		"naddr = 2 expected, but %zu was returned", naddr);
	ATF_CHECK_MSG(modified0 == (void*)actualModifiedAddress0,
		"first modified page was %p; expected %p",
		modified0, (void*)actualModifiedAddress0);
	ATF_CHECK_MSG(modified1 == (void*)actualModifiedAddress1,
		"second modified page was %p; expected %p",
		modified1, (void*)actualModifiedAddress1);
	ATF_CHECK_MSG((int)gran == PAGE_SIZE,
		"returned page size, %zu, was incorrect", gran);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__clear_flag_clears_writes);
ATF_TC_BODY(mwritten__clear_flag_clears_writes, tc) {
	size_t len = sizeof(char) * 256;
	char *heap = malloc(len);

	int modifiedIndex = 1;
	heap[modifiedIndex] ^= 'a';

	size_t naddr = 3;
	uintptr_t buf[naddr];
	size_t gran = 0;

	mwritten(heap, len, MWRITTEN_CLEAR, buf, &naddr, &gran);

	naddr = 0;
	int res = mwritten(heap, len, 0, buf, &naddr, &gran);

	ATF_CHECK_MSG(res == 0,
		"mwritten failed after clear with error code %d", res);
	ATF_CHECK_MSG(naddr == 0,
		"writes were not cleared: naddr = %zu was returned", naddr);
	ATF_CHECK_MSG((int)gran == PAGE_SIZE,
		"returned page size, %zu, was incorrect", gran);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__detects_writes_after_clear);
ATF_TC_BODY(mwritten__detects_writes_after_clear, tc) {
	size_t len = sizeof(char) * PAGE_SIZE * 32;
	char *heap = malloc(len);

	int modifiedIndex1 = PAGE_SIZE * 4 + 13;
	heap[modifiedIndex1] ^= 'a';

	int modifiedIndex2 = PAGE_SIZE * 8 + 17;
	heap[modifiedIndex2] ^= 'f';

	size_t naddr = 3;
	uintptr_t buf[naddr];
	size_t gran = 0;

	mwritten(heap, len, MWRITTEN_CLEAR, buf, &naddr, &gran);

	int modifiedIndex3 = PAGE_SIZE * 16 + 17;
	heap[modifiedIndex3] ^= 'b';
	gran = 0;

	int res = mwritten(heap, len, MWRITTEN_CLEAR, buf, &naddr, &gran);

	void *modified = (void *) buf[0];
	uintptr_t actualModifiedAddress = GET_PAGE_ADDR(
		(void*)((uintptr_t) heap + modifiedIndex3));

	ATF_CHECK_MSG(res == 0,
		"mwritten failed after clear with error code %d", res);
	ATF_CHECK_MSG(naddr == 1,
		"expected 1 address after clearing, but naddr = %zu returned", naddr);
	ATF_CHECK_MSG(modified == (void*) actualModifiedAddress,
		"address of modified page is incorrect");
	ATF_CHECK_MSG((int)gran == PAGE_SIZE,
		"returned page size, %zu, was incorrect", gran);

	free(heap);
}

ATF_TC_WITHOUT_HEAD(mwritten__clears_writes_if_clear_flag_set_and_buf_null);
ATF_TC_BODY(mwritten__clears_writes_if_clear_flag_set_and_buf_null, tc) {
	int len = PAGE_SIZE * 32;
	char *heap = malloc(len);

	int modifiedIndex1 = PAGE_SIZE * 4 + 13;
	heap[modifiedIndex1] ^= 'a';

	int modifiedIndex2 = PAGE_SIZE * 8 + 17;
	heap[modifiedIndex2] ^= 'f';

	size_t gran = 0;

	int res = mwritten(heap, len, MWRITTEN_CLEAR, NULL, NULL, &gran);
	
	ATF_CHECK_MSG(res == 0, "mwritten failed with error code %d", res);

	size_t naddr = 3;
	uintptr_t buf[naddr];
	gran = 0;

	res = mwritten(heap, len, 0, buf, &naddr, &gran);

	ATF_CHECK_MSG(res == 0,
		"mwritten failed after clear with error code %d", res);
	ATF_CHECK_MSG(naddr == 0,
		"all writes should be cleared, but naddr = %zu was returned", naddr);
	ATF_CHECK_MSG((int)gran == PAGE_SIZE,
		"returned page size, %zu, was incorrect", gran);

	free(heap);
}

ATF_TP_ADD_TCS(tp)
{
	ATF_TP_ADD_TC(tp, mwritten__returns_einval_when_len_zero);
	ATF_TP_ADD_TC(tp, mwritten__returns_einval_when_addr_illegal);
	ATF_TP_ADD_TC(tp, mwritten__returns_einval_when_len_beyond_range);
	ATF_TP_ADD_TC(tp, mwritten__returns_efault_when_bad_gran_addr_passed_in);
	ATF_TP_ADD_TC(tp, mwritten__returns_efault_when_bad_buffer_start_passed_in);
	ATF_TP_ADD_TC(tp, mwritten__returns_efault_when_zero_naddr_passed_in);
	ATF_TP_ADD_TC(tp, mwritten__returns_efault_when_too_large_naddr_passed_in);
	ATF_TP_ADD_TC(tp, mwritten__returns_einval_when_naddr_null_and_clear_flag_unset);
	ATF_TP_ADD_TC(tp, mwritten__returns_einval_when_buf_null_and_clear_flag_unset);
	ATF_TP_ADD_TC(tp, mwritten__sets_naddr_zero_with_no_writes);
	ATF_TP_ADD_TC(tp, mwritten__returns_einval_if_given_illegal_addr);
	ATF_TP_ADD_TC(tp, mwritten__correctly_detects_single_modified_address);
	ATF_TP_ADD_TC(tp, mwritten__correctly_detects_many_modified_addresses);
	ATF_TP_ADD_TC(tp, mwritten__fills_buffer_and_returns_if_too_many_writes);
	ATF_TP_ADD_TC(tp, mwritten__clear_flag_clears_writes);
	ATF_TP_ADD_TC(tp, mwritten__detects_writes_after_clear);
	ATF_TP_ADD_TC(tp, mwritten__clears_writes_if_clear_flag_set_and_buf_null);

	return atf_no_error();
}