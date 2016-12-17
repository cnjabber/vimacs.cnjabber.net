#include <stdio.h>
#include <string.h>

int calc_key(char *s, char *t)
{
	int l = strlen(s);
	int i;
	int sum = 0;
	int q;
	char c;

	for (i=0; i<l; i++) {
		sum += s[i];
		q = sum / (i+1);
		c = s[l-1-i];
		t[i*2] = ((q^c)%93)+0x21;
		t[i*2+1] = (q%26)+'a';
	}
	t[l*2] = '-';
	t[l*2+1] = (sum%93)+0x21;
	t[l*2+2] = 0;
}

int main()
{
	char first[1000], last[1000], full[1000], serial[1000];
	scanf("%s%s", first, last);
	strcpy(full, first);
	strcat(full, last);
	calc_key(full, serial);
	printf("%s\n%s\n%s\n", first, last, serial);
	return 0;
}
