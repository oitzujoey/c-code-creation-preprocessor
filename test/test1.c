
/*cccp generate
	-- Create a sine LUT of length 1000.
	return '\n\tputs("Hello, world!");\n'
*/

#include <stdio.h>
#include <math.h>

int main() {

/*cccp begin*/
	puts("Hello, world!");
/*cccp end 61DD0743*/
	
/*cccp generate
	-- Create a sine LUT of length 1000.
	return '\n\tputs("Need more generation.");\n'
*/

/*cccp begin*/
	puts("Need more generation.");
/*cccp end B4030A83*/
	
	for (int i = 0; i < length; i++) {
		printf("%f", sineLUT[i]);
		if (i >= length - 1) {
			putc('\n', stdout);
		}
		else {
			putc(' ', stdout);
		}
	}
	
	return 0;
}
