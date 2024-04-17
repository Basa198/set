#include <stdio.h>
#include <CUnit/Basic.h>
#include "set.h"

void test_create_set() {
    Set* set = create_set();
    CU_ASSERT_PTR_NOT_NULL(set);
    CU_ASSERT_EQUAL(set->size, 0);
    CU_ASSERT_PTR_NOT_NULL(set->_tree);
    free_set(set);
}

void test_set_insert() {
    Set* set = create_set();
    set_insert(set, 10);
    CU_ASSERT_EQUAL(set->size, 1);
    CU_ASSERT_TRUE(set_search(set, 10));
    set_insert(set, 20);
    CU_ASSERT_EQUAL(set->size, 2);
    CU_ASSERT_TRUE(set_search(set, 20));
    free_set(set);
}

void test_set_search() {
    Set* set = create_set();
    CU_ASSERT_FALSE(set_search(set, 10));
    set_insert(set, 10);
    CU_ASSERT_TRUE(set_search(set, 10));
    CU_ASSERT_FALSE(set_search(set, 20));
    free_set(set);
}

void test_set_remove() {
    Set* set = create_set();
    set_insert(set, 10);
    set_insert(set, 20);
    set_insert(set, 30);
    CU_ASSERT_EQUAL(set->size, 3);
    set_remove(set, 20);
    CU_ASSERT_EQUAL(set->size, 2);
    CU_ASSERT_FALSE(set_search(set, 20));
    set_remove(set, 10);
    CU_ASSERT_EQUAL(set->size, 1);
    CU_ASSERT_FALSE(set_search(set, 10));
    set_remove(set, 30);
    CU_ASSERT_EQUAL(set->size, 0);
    CU_ASSERT_FALSE(set_search(set, 30));
    free_set(set);
}

void test_edge_cases() {
    Set* set = create_set();
    set_remove(set, 10); // Removing a non-existent element
    CU_ASSERT_EQUAL(set->size, 0);
    free_set(NULL); // Freeing a NULL set
    free_set(set); // Freeing an empty set
}

int main() {
    CU_pSuite pSuite = NULL;

    /* Initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* Add a suite to the registry */
    pSuite = CU_add_suite("Set Test Suite", NULL, NULL);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Add the tests to the suite */
    if (
        (NULL == CU_add_test(pSuite, "test of create_set()", test_create_set)) ||
        (NULL == CU_add_test(pSuite, "test of set_insert()", test_set_insert)) ||
        (NULL == CU_add_test(pSuite, "test of set_search()", test_set_search)) ||
        (NULL == CU_add_test(pSuite, "test of set_remove()", test_set_remove)) ||
        (NULL == CU_add_test(pSuite, "test of edge cases", test_edge_cases))
    ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}