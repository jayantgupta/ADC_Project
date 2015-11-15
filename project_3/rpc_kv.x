/*
 * keyval.x Specification of the key-value.
 */

/*
	 Define two procedures
			PUT(key, value)	puts a value for a given key.
			GET(key) gets the value for the given key.
			DELETE(key) removes the value for the given key.
*/

/* structure for each record in the ley value table. */
struct row{
				int key;
				char value[25];
};

program KEYVAL_PROG{
	version KEYVAL_VERS{
		bool PUT(row) = 1; /* Procedure number 1 */
		string GET(int key) = 2; /* Procedure number 2 */
		bool DELETE(int key) = 3; /* Procedure number 3 */
	} = 1;	/* version number */
} = 0x32345678; /* program number = 0x32345678*/
