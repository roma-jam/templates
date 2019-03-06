/*
 * flash_update_test.h
 *
 *  Created on: 6 марта 2019 г.
 *      Author: RLeonov
 */

#ifndef FLASH_UPDATE_TEST_H_
#define FLASH_UPDATE_TEST_H_


int flash_update(unsigned int dst_addr, unsigned int src_addr, int bytes_to_copy, bool reset);


#endif /* FLASH_UPDATE_TEST_H_ */
