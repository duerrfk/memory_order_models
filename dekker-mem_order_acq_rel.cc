/**
 * Copyright 2018 University of Stuttgart
 * Author: Frank Duerr (frank.duerr@ipvs.uni-stuttgart.de)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * Implementation of Dekker's Algorithm using acquire/release memory order 
 * model. The purpose of this implementation is to practically show whether 
 * Dekker's algorithm is guaranteeing mutual exclusion with acquire/release 
 * memory access.
 */

#include <iostream>
#include <thread>
#include <atomic>

#define LOOPCNT 10000

// Global variables for synchronizing thread 1 and 2.
volatile std::atomic_bool want1 = {false};
volatile std::atomic_bool want2 = {false};
volatile std::atomic_int turn = {0};

volatile uint16_t x = 0;

void *thread1_run()
{
     printf("Thread 1 starting\n");

     while (true) {
	  // Pre-protocol
	  want1.store(true, std::memory_order_release);
	  while (want2.load(std::memory_order_acquire)) {
	       if (turn.load(std::memory_order_acquire) != 0) {
		    want1.store(false, std::memory_order_release);
		    while (turn.load(std::memory_order_acquire) != 0);
		    want1.store(true, std::memory_order_release);
	       }
	  }
	  
	  // Critical section
	  uint8_t *x_low = (uint8_t *) &x;
	  *x_low = 0xff;
	  for (int i = 0; i < LOOPCNT; i++);
	  *x_low = 0x00;

	  // Post-protocol
	  turn.store(1, std::memory_order_release);
	  want1.store(false, std::memory_order_release);
     }
     
     return NULL;
}

void *thread2_run()
{
     printf("Thread 2 starting\n");

     while (true) {
	  // Pre-protocol
	  want2.store(true, std::memory_order_release);
	  while (want1.load(std::memory_order_acquire)) {
	       if (turn.load(std::memory_order_acquire) != 1) {
		    want2.store(false, std::memory_order_release);
		    while (turn.load(std::memory_order_acquire) != 1);
		    want2.store(true, std::memory_order_release);
	       }
	  }
	  
	  // Critical section
	  uint8_t *x_high = ((uint8_t *) &x)+1;
	  *x_high = 0xff;
	  for (int i = 0; i < LOOPCNT; i++);
	  *x_high = 0x00;

	  // Post-protocol
	  turn.store(0, std::memory_order_release);
	  want2.store(false, std::memory_order_release);
     }
	  
     return NULL;
}

int main(int argc, char *argv[])
{
     std::thread t1(thread1_run);
     std::thread t2(thread2_run);

     while (true) {
	  if (x == 0xffff) {
	       std::cout << "Mutual exclusion violated" << std::endl;
	       exit(-1);
	  }
     }

     t1.join();
     t2.join();
}
