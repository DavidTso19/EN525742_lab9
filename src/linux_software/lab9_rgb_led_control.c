#include <stdio.h>
#include <sys/mman.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <stdint.h>
#define _BSD_SOURCE

// GPIO register offsets and base address
#define GPIO_DIP_OFFSET 2
#define GPIO_LED_OFFSET 0 
#define DIPS_AND_LEDS_BASEADDR 0x41200000

// RGB LED offsets and base address
#define RGB_LED5_OFFSET 0
#define RGB_LED6_OFFSET 2
#define RGB_LEDS_BASEADDR 0x41210000

// the below code uses a device called /dev/mem to get a pointer to a physical
// address.  We will use this pointer to read/write the custom peripheral
volatile unsigned int * get_a_pointer(unsigned int phys_addr)
{

	int mem_fd = open("/dev/mem", O_RDWR | O_SYNC); 
	void *map_base = mmap(0, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, phys_addr); 
	volatile unsigned int *radio_base = (volatile unsigned int *)map_base; 
	return (radio_base);
}

int main()
{

	// Get pointers to the GPIO and RGB LED hardware.
    volatile unsigned int *dipsandleds_ptr = get_a_pointer(DIPS_AND_LEDS_BASEADDR);
	volatile unsigned int *rgb_leds_ptr = get_a_pointer(RGB_LEDS_BASEADDR);
	printf("\r\n\r\n\r\nStudent Name: David Tso\n\r");
    printf("Lab 9 Program\n\r");

	uint8_t led_counter = 0;
	uint8_t switches_value;
    while (1)
    {
		// Read the first three DIP switches.
        // The mask keeps only the three bits used to control RGB colors.
		switches_value = *(dipsandleds_ptr+GPIO_DIP_OFFSET) & 0x07;

		// Set RGB LEDs 5 and 6 to the selected color.
		*(rgb_leds_ptr+RGB_LED5_OFFSET) = switches_value;
		*(rgb_leds_ptr+RGB_LED6_OFFSET) = switches_value;

		// Display the counter value on the single-color LEDs.
	    *(dipsandleds_ptr+GPIO_LED_OFFSET) = led_counter;

		// Wait half a second before updating again.
	    usleep(500000);

	    // Print the current values of the switches and LEDs.
		fflush(stdout);
		printf("Current Single-Color LEDs = %d\r\n",*(dipsandleds_ptr+GPIO_LED_OFFSET));
	    printf("Current Switches = %d\r\n",*(dipsandleds_ptr+GPIO_DIP_OFFSET));
		printf("Current RGB LED5 = %d\r\n",*(rgb_leds_ptr+RGB_LED5_OFFSET));
		printf("Current RGB LED6 = %d\r\n",*(rgb_leds_ptr+RGB_LED6_OFFSET));

		// Increment the counter and reset it after 15.
		led_counter = (led_counter + 1) & 0x0F;
    }
    return 0;
}
